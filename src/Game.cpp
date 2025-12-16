#include "Game.hpp"
#include "FastNoiseLite.h"
#include "debug.hpp"

#include <nlohmann/json.hpp>
#include "NPCEntity.hpp"
#include "House.hpp"
#include "Market.hpp"
#include "Actions.hpp"
#include "DataCollector.hpp"

#include <random>
#include <set>
#include <unordered_map>
#include <algorithm>
#include <thread>
#ifdef USE_TENSORFLOW
#include <tensorflow/c/c_api.h>
#endif


Game::Game()
    : window(sf::VideoMode(GameConfig::windowWidth, GameConfig::windowHeight), "MicroSociety", sf::Style::Titlebar | sf::Style::Close),
      house(TextureManager::getInstance().getTexture("house1", "../assets/objects/house1.png"), 1),
      market(), 
      clockGUI(700, 100) {
#ifdef _WIN32
    ui.adjustLayout(window);
#endif
    playerTexture.loadFromFile("../assets/npc/person1.png");
    if (!playerTexture.getSize().x) {
        std::cerr << "Failed to load player texture!" << std::endl;
    }

    market.setPrice("wood", 1 + std::rand() % 50); 
    market.setPrice("stone", 1 + std::rand() % 50);
    market.setPrice("bush", 1 + std::rand() % 50);

    generateMap();
    npcs = generateNPCEntities(); 

    ui.updateNPCEntityList(npcs);

    if (tensorFlowEnabled) {
        initializeNPCTensorFlow();
    }
}

Game::~Game() {
    if (getDataCollector().isCollectingData()) {
        getDebugConsole().log("DataCollector", "Saving collected training data...");
        getDataCollector().stopCollection();
    }

    const bool hasData = getDataCollector().getTotalExperiences() > 0 ||
                         getDataCollector().getCurrentBatchSize() > 0;

    if (hasData) {
        getDataCollector().exportToJSON("training_data.json");
        getDataCollector().exportToCSV("training_data.csv");
        
        // print statistics and analysis
        getDataCollector().printStatistics();
        getDataCollector().analyzeDataQuality();
        
        getDebugConsole().log("DataCollector", "Data collection complete. Total experiences: " + 
                            std::to_string(getDataCollector().getTotalExperiences()));
    }
}

// enable TensorFlow mode for NPCs
void Game::enableTensorFlow(bool enable) {
    tensorFlowEnabled = enable;

    if (enable) {
        if (!getDataCollector().isCollectingData()) {
            getDebugConsole().log("DataCollection", "Starting data collection for TensorFlow mode...");
            getDataCollector().setMaxExperiencesPerFile(100); // Lower threshold for faster flushing
            getDataCollector().startCollection();
        }
        initializeNPCTensorFlow();
    } else {
        if (getDataCollector().isCollectingData()) {
            getDataCollector().stopCollection();
        }
    }
}

// initialize TensorFlow models for NPCs
void Game::initializeNPCTensorFlow() {
    #ifdef USE_TENSORFLOW
        getDebugConsole().log("TensorFlow", "TensorFlow C API version: " + std::string(TF_Version()));
        
        // check if TF model exists
        std::ifstream modelFile("models/npc_rl_model.tflite");
        if (!modelFile.good()) {
            getDebugConsole().log("TensorFlow", "No pre-trained model found. Running in DATA COLLECTOR mode.", LogLevel::Warning);
            getDebugConsole().log("TensorFlow", "NPCs will use random actions to gather training data.");
            
            // enable TensorFlow mode on NPCs but without model (for data collection)
            for (auto& npc : npcs) {
                npc.setTensorFlowModel(nullptr); // no model = data collection mode
                npc.enableTensorFlow(true);      // enable TF flag for data collection
            }
            
            return;
        }
        
        getDebugConsole().log("TensorFlow", "Pre-trained model found, loading for inference...");
        
        // initialize TensorFlow models for NPCs
        auto tfModel = std::make_shared<TensorFlowWrapper>();
        if (tfModel->initialize("models/npc_rl_model.tflite")) {
            getDebugConsole().log("TensorFlow", "TensorFlow model loaded successfully.");
            
            // apply TF model to NPCs
            for (auto& npc : npcs) {
                npc.setTensorFlowModel(tfModel);
                npc.enableTensorFlow(true);
            }
        } else {
            getDebugConsole().log("TensorFlow", "Failed to load TensorFlow model, switching to data collection mode", LogLevel::Error);
            
            // fallback to data collection mode
            for (auto& npc : npcs) {
                npc.setTensorFlowModel(nullptr);
                npc.enableTensorFlow(true);
            }
        }
        
    #else
        getDebugConsole().log("TensorFlow", "TensorFlow support not compiled in. Using default Q-learning instead.", LogLevel::Warning);
        tensorFlowEnabled = false;
    #endif
}

// check data collection progress for TensorFlow training
void Game::checkDataCollectionProgress() {
    if (tensorFlowEnabled && getDataCollector().isCollectingData()) {
        size_t totalExperiences = getDataCollector().getTotalExperiences();
        
        // auto-export every 1000 experiences for training
        if (totalExperiences > 0 && totalExperiences % 1000 == 0) {
            getDebugConsole().log("DataCollection", "Reached " + std::to_string(totalExperiences) + 
                                " experiences. Exporting batch for training...");
            
            std::string filename = "batch_" + std::to_string(totalExperiences / 10000) + "_data.csv";
            getDataCollector().exportToCSV(filename);
            getDataCollector().printStatistics();
        }
    }
}

// update persistent statistics across simulations
void Game::updatePersistentStats() {
    persistentStats.totalItemsGatheredAllTime += getTotalItemsGathered();
    persistentStats.totalItemsSoldAllTime += market.getTotalItemsSold();
    persistentStats.totalMoneySpentAllTime += MoneyManager::getTotalMoneySpent();
    persistentStats.totalMoneyEarnedAllTime += MoneyManager::getTotalMoneyEarned();
    persistentStats.totalIterations++;
}

// get the tile map
const std::vector<std::vector<std::unique_ptr<Tile>>>& Game::getTileMap() const {
    return tileMap;
}

// detect collision for an entity
bool Game::detectCollision(Entity& entity) {
    int tileX = static_cast<int>(entity.getPosition().x / GameConfig::tileSize);
    int tileY = static_cast<int>(entity.getPosition().y / GameConfig::tileSize);

    if (tileX >= 0 && tileX < tileMap[0].size() && tileY >= 0 && tileY < tileMap.size()) {
        Tile& targetTile = *tileMap[tileY][tileX];
        
        // Handle collision with tile objects
        if (targetTile.hasObject()) {
            ObjectType objType = targetTile.getObject()->getType();
            
            if (auto* npc = dynamic_cast<NPCEntity*>(&entity)) {
                // NPC-specific collision handling
                npc->performAction(ActionType::RegenerateEnergy, targetTile, tileMap, market, house);
            }
        }
    }
    return false;
}


// run the main game loop
void Game::run() {
    sf::Clock clock;
    window.setFramerateLimit(GameConfig::WINDOW_FPS_LIMIT);
    
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed || sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) {
                window.close();
                getDebugConsole().saveLogsToFile("logs/simulation_log.txt"); 
            }
            if (event.type == sf::Event::Resized) ui.adjustLayout(window);

            ui.handleButtonClicks(window, event, npcs, timeManager, market);
            ui.handleNPCEntityPanel(window, event, npcs);
            ui.handleStatsPanel(window, event);
            ui.handleOptionsEvents(window, event, *this);
        }

        sf::Time dt = clock.restart();
        deltaTime = dt.asSeconds();

        // apply simulation speed to market dynamics
        market.simulateMarketDynamics(deltaTime * simulationSpeed);
        
        // apply simulation speed to resource regeneration
        resourceRegenerationTimer += deltaTime * simulationSpeed;
        if (resourceRegenerationTimer >= regenerationInterval) {
            regenerateResources();
            resourceRegenerationTimer = 0.0f;
        }

        // simulate NPCs with simulation speed
        simulateNPCEntityBehavior(deltaTime * simulationSpeed);
        simulateSocietalGrowth(deltaTime * simulationSpeed);

        checkDataCollectionProgress();

        // update UI with total money
        ui.updateMoney(MoneyManager::calculateTotalMoney(npcs));
                      
        // pass simulation speed to time manager
        timeManager.update(deltaTime, simulationSpeed);
        
        // clock GUI should also respect simulation speed for consistency
        clockGUI.update(timeManager.getElapsedTime());
        
        ui.updateStatus(
            timeManager.getCurrentDay(),
            timeManager.getFormattedTime(),
            timeManager.getSocietyIteration()
        );
        ui.updateStats(npcs, timeManager);
        ui.updateMarketPanel(market);
        ui.updateNPCList(npcs);

        // render everything
        window.clear();
        render();
        clockGUI.render(window, isClockVisible);
        ui.render(window, market, npcs);
        getDebugConsole().render(window);
        window.display();
    }
}


// simulate NPC behavior with stuck detection and handling
void Game::simulateNPCEntityBehavior(float deltaTime) {
    static std::unordered_map<std::string, int> stuckCounter;
    static std::unordered_map<std::string, sf::Vector2f> lastPosition;
    static std::unordered_map<std::string, float> stuckTimer;
    
    for (auto it = npcs.begin(); it != npcs.end(); ) {
        NPCEntity& npc = *it;
        
        npc.update(deltaTime);
        
        // check if NPC ded
        if (npc.isDead() || npc.getHealth() <= 0 || npc.getEnergy() <= 0) {
            getDebugConsole().log("DEATH", npc.getName() + " has died.");
            it = npcs.erase(it);
            continue;
        }
        // stuck detection
        sf::Vector2f currentPos = npc.getPosition();
        if (lastPosition.find(npc.getName()) != lastPosition.end()) {
            sf::Vector2f lastPos = lastPosition[npc.getName()];
            float distanceMoved = std::hypot(currentPos.x - lastPos.x, currentPos.y - lastPos.y);
            
            // stuck if moved less than 1 pixel while walking
            if (distanceMoved < 1.0f && npc.getState() == NPCState::Walking) {
                stuckTimer[npc.getName()] += deltaTime;
                if (stuckTimer[npc.getName()] > 4.0f) {
                    // reset state to idle if stuck for more than 3 seconds
                    npc.setState(NPCState::Idle);
                    npc.setTarget(nullptr);
                    stuckTimer[npc.getName()] = 0.0f;
                    
                    getDebugConsole().log("UNSTUCK", npc.getName() + " was stuck walking, reset to idle");
                    
                    // teleport if severely stuck
                    if (stuckTimer[npc.getName()] > 10.0f) {
                        std::random_device rd;
                        std::mt19937 gen(rd());
                        std::uniform_int_distribution<> distX(1, GameConfig::mapWidth - 2);
                        std::uniform_int_distribution<> distY(1, GameConfig::mapHeight - 2);
                        
                        float newX = distX(gen) * GameConfig::tileSize;
                        float newY = distY(gen) * GameConfig::tileSize;
                        npc.setPosition(newX, newY);
                        
                        getDebugConsole().log("TELEPORT", npc.getName() + " was severely stuck, teleported to (" + 
                                            std::to_string(newX) + ", " + std::to_string(newY) + ")");
                    }
                }
            } else {
                stuckTimer[npc.getName()] = 0.0f; // reset if moved
            }
        }
        lastPosition[npc.getName()] = currentPos;
        
        // NPC state machine
        switch (npc.getState()) {
            case NPCState::Idle: {
                ActionType actionType = npc.decideNextAction(tileMap, house, market);
                npc.setCurrentAction(actionType);
                
                Tile* nearestTile = nullptr;
                
                switch (actionType) {
                    case ActionType::ChopTree:
                        nearestTile = npc.findNearestTile(tileMap, ObjectType::Tree);
                        break;
                    case ActionType::MineRock:
                        nearestTile = npc.findNearestTile(tileMap, ObjectType::Rock);
                        break;
                    case ActionType::GatherBush:
                        nearestTile = npc.findNearestTile(tileMap, ObjectType::Bush);
                        break;
                    case ActionType::BuyItem:
                    case ActionType::SellItem:
                        nearestTile = npc.findNearestTile(tileMap, ObjectType::Market);
                        break;
                    case ActionType::RegenerateEnergy:
                    case ActionType::UpgradeHouse:
                    case ActionType::StoreItem:
                        nearestTile = npc.findNearestTile(tileMap, ObjectType::House);
                        break;
                    case ActionType::Rest:
                        npc.setState(NPCState::PerformingAction);
                        break;
                    default:
                        npc.setCurrentAction(ActionType::Rest);
                        npc.setState(NPCState::PerformingAction);
                        break;
                }
                
                if (nearestTile) {
                    npc.setTarget(nearestTile);
                    npc.setState(NPCState::Walking);
                } else if (npc.getCurrentAction() != ActionType::Rest) {
                    npc.setCurrentAction(ActionType::Rest);
                    npc.setState(NPCState::PerformingAction);
                }
                break;
            }
            
            case NPCState::Walking: { // i added reduce health and energy while walking because npcs were immortal while moving or when being stuck, might remove later
                if (npc.getTarget() && !npc.isAtTarget()) {
                    performPathfinding(npc);

                    npc.reduceHealth(0.001f * deltaTime); 
                    npc.consumeEnergy(0.1f * deltaTime);  
                    
                    // check if reached target
                    sf::Vector2f targetPos = npc.getTarget()->getPosition();
                    sf::Vector2f npcPos = npc.getPosition();
                    float distance = std::hypot(targetPos.x - npcPos.x, targetPos.y - npcPos.y);
                    
                    if (distance < GameConfig::tileSize * 1.5f) {
                        npc.setState(NPCState::PerformingAction);
                    }
                } else {
                    npc.setState(NPCState::PerformingAction);
                }
                break;
            }
            
            case NPCState::PerformingAction: {
                if (npc.getTarget()) {
                    npc.performAction(npc.getCurrentAction(), *npc.getTarget(), tileMap, market, house);
                } else {
                    sf::Vector2f npcPos = npc.getPosition();
                    int tileX = static_cast<int>(npcPos.x / GameConfig::tileSize);
                    int tileY = static_cast<int>(npcPos.y / GameConfig::tileSize);
                    
                    if (tileX >= 0 && tileX < tileMap[0].size() && tileY >= 0 && tileY < tileMap.size()) {
                        npc.performAction(npc.getCurrentAction(), *tileMap[tileY][tileX], tileMap, market, house);
                    }
                }
                
                npc.setTarget(nullptr);
                npc.setState(NPCState::Idle);
                break;
            }
            
            case NPCState::EvaluatingState: {
                npc.setState(NPCState::Idle);
                break;
            }
        }
        
        ++it;
    }
    
    if (npcs.empty()) {
        getDebugConsole().log("SYSTEM", "All NPCs died. Processing final data...");
        
        // save and export all data before reset
        if (getDataCollector().isCollectingData()) {
            size_t currentBatchSize = getDataCollector().getCurrentBatchSize();
            size_t totalExp = getDataCollector().getTotalExperiences();
            
            getDebugConsole().log("DataCollection", 
                "Final data state: " + std::to_string(currentBatchSize) + " in current batch, " +
                std::to_string(totalExp) + " total saved experiences");
            
            if (currentBatchSize > 0) {
                getDebugConsole().log("DataCollection", "Force-saving final batch...");
                getDataCollector().forceSaveCurrentBatch();
            }
            
            // export everything 
            std::string timestamp = std::to_string(std::time(nullptr));
            getDataCollector().exportToCSV("final_training_data_" + timestamp + ".csv");
            getDataCollector().exportToJSON("final_training_data_" + timestamp + ".json");
            
            // print final statistics and analysis
            getDataCollector().printStatistics();
            getDataCollector().analyzeDataQuality();
        }
        
        // log stats while data is still available
        logIterationStats(timeManager.getSocietyIteration() + 1);
        
        getDebugConsole().log("SYSTEM", "Restarting simulation...");
        resetSimulation();
    }
}

// handle market actions for NPCs
void Game::handleMarketActions(NPCEntity& npc, Tile& targetTile, ActionType actionType) {
    if (!targetTile.hasObject()) {
        getDebugConsole().log("ERROR", npc.getName() + " tried to access a NON-EXISTENT market.");
        return;
    }

    auto* market = dynamic_cast<Market*>(targetTile.getObject());
    if (!market || market->getPrices().empty()) {
        getDebugConsole().log("ERROR", "Market reference is NULL or has no prices.");
        return;
    }

    if (actionType == ActionType::BuyItem) {
        std::string bestItemToBuy = market->suggestBestResourceToBuy();
        if (bestItemToBuy.empty()) {
            getDebugConsole().log("MARKET", npc.getName() + " found nothing worth buying.");
            return;
        }

        float itemPrice = market->calculateBuyPrice(bestItemToBuy);
        if (npc.getMoney() >= itemPrice) {
            market->buyItem(npc, bestItemToBuy, 5);
            npc.reduceHealth(2.5f);  // Buying reduces health
            getDebugConsole().log("MARKET", npc.getName() + " bought 5 " + bestItemToBuy);
        }
    } 
    else if (actionType == ActionType::SellItem) {
        std::string bestItemToSell = market->suggestBestResourceToSell();
        if (bestItemToSell.empty()) {
            getDebugConsole().log("MARKET", npc.getName() + " has nothing to sell.");
            return;
        }

        market->sellItem(npc, bestItemToSell, 5);
        npc.restoreHealth(5.0f); // Selling increases health
        getDebugConsole().log("MARKET", npc.getName() + " sold 5 " + bestItemToSell);
    } 
    else if (actionType == ActionType::UpgradeHouse) {
        npc.restoreHealth(20.0f); // Upgrading restores health
        getDebugConsole().log("MARKET", npc.getName() + " upgraded house and restored health.");
    }
}

// move NPC to resource tile and perform action
void Game::moveToResource(NPCEntity& npc, ActionType actionType) {
    int currentX = static_cast<int>(npc.getPosition().x / GameConfig::tileSize);
    int currentY = static_cast<int>(npc.getPosition().y / GameConfig::tileSize);
    int targetX = -1, targetY = -1;
    ObjectType targetType;

    if (actionType == ActionType::ChopTree) targetType = ObjectType::Tree;
    else if (actionType == ActionType::MineRock) targetType = ObjectType::Rock;
    else if (actionType == ActionType::GatherBush) targetType = ObjectType::Bush;
    else return;

    float shortestDistance = std::numeric_limits<float>::max();
    for (int y = 0; y < tileMap.size(); ++y) {
        for (int x = 0; x < tileMap[y].size(); ++x) {
            if (tileMap[y][x]->hasObject() && tileMap[y][x]->getObject()->getType() == targetType) {
                float distance = std::hypot(x - currentX, y - currentY);
                if (distance < shortestDistance) {
                    shortestDistance = distance;
                    targetX = x;
                    targetY = y;
                }
            }
        }
    }

    if (targetX != -1 && targetY != -1) {
        Tile& targetTile = *tileMap[targetY][targetX];
        npc.performAction(actionType, targetTile, tileMap, market, house);
    }
}

// store most abundant item from NPC inventory to house
void Game::storeItems(NPCEntity& npc, Tile& tile) {
    auto inventory = npc.getInventory();
    std::string mostAbundantResource;
    int maxQuantity = 0;

    // find the most abundant resource
    for (const auto& [item, quantity] : inventory) {
        if (quantity > maxQuantity) {
            mostAbundantResource = item;
            maxQuantity = quantity;
        }
    }

    if (!mostAbundantResource.empty()) {
        npc.performAction(ActionType::StoreItem, tile, tileMap, market, house);
    }
}

void Game::evaluateNPCEntityState(NPCEntity& NPCEntity) {
    if (NPCEntity.getEnergy() <= 0.0f) {
        getDebugConsole().log("NPCEntity", NPCEntity.getName() + " ran out of energy and died.");
        // Handle NPCEntity death (remove or reset state) todod
    }
}


// regenerate resources on the map
void Game::regenerateResources() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> distX(0, GameConfig::mapWidth - 1);
    std::uniform_int_distribution<int> distY(0, GameConfig::mapHeight - 1);
    std::uniform_real_distribution<float> resourceChance(0.0f, 1.0f); // for probability-based spawning

    auto& textureManager = TextureManager::getInstance();

    std::vector<const sf::Texture*> rockTextures = {
        &textureManager.getTexture("rock1", "../assets/objects/rock1.png"),
        &textureManager.getTexture("rock2", "../assets/objects/rock2.png"),
        &textureManager.getTexture("rock3", "../assets/objects/rock3.png")
    };

    std::vector<const sf::Texture*> bushTextures = {
        &textureManager.getTexture("bush1", "../assets/objects/bush1.png"),
        &textureManager.getTexture("bush2", "../assets/objects/bush2.png")
    };

    std::vector<const sf::Texture*> treeTextures = {
        &textureManager.getTexture("tree1", "../assets/objects/tree1.png"),
        &textureManager.getTexture("tree2", "../assets/objects/tree2.png"),
        &textureManager.getTexture("tree3", "../assets/objects/tree3.png")
    };

    int numResourcesToRegenerate = GameConfig::mapWidth * GameConfig::mapHeight * 0.05; // increased to 5% of map tiles

    for (int i = 0; i < numResourcesToRegenerate; ++i) {
        int x = distX(gen);
        int y = distY(gen);

        if (!tileMap[y][x]->hasObject()) {
            float chance = resourceChance(gen);

            // higher chance for trees/bushes on GrassTile
            if (auto grassTile = dynamic_cast<GrassTile*>(tileMap[y][x].get())) {
                if (chance < 0.4f) { // 40% chance for trees
                    grassTile->placeObject(std::make_unique<Tree>(*treeTextures[rand() % treeTextures.size()]));
                    getDebugConsole().log("Resource Regen", "Tree spawned at (" + std::to_string(x) + ", " + std::to_string(y) + ")");
                } else if (chance < 0.7f) { // 30% chance for bushes
                    grassTile->placeObject(std::make_unique<Bush>(*bushTextures[rand() % bushTextures.size()]));
                    getDebugConsole().log("Resource Regen", "Bush spawned at (" + std::to_string(x) + ", " + std::to_string(y) + ")");
                }
            }
            
            // higher chance for rocks on StoneTile
            else if (auto stoneTile = dynamic_cast<StoneTile*>(tileMap[y][x].get())) {
                if (chance < 0.8f) { // 80% chance for rocks on StoneTile
                    stoneTile->placeObject(std::make_unique<Rock>(*rockTextures[rand() % rockTextures.size()]));
                    getDebugConsole().log("Resource Regen", "Rock spawned at (" + std::to_string(x) + ", " + std::to_string(y) + ")");
                }
            }
        }
    }
}

// simulate societal growth affecting market dynamics
void Game::simulateSocietalGrowth(float deltaTime) {
    // example societal growth logic: increase market prices as demand rises
    static float timeAccumulator = 0.0f;
    timeAccumulator += deltaTime;

    if (timeAccumulator >= 30.0f) { // adjust market prices every 30 seconds
        for (const auto& [item, currentPrice] : market.getPrices()) {
            int demand = market.getBuyTransactions(item);
            int supply = market.getSellTransactions(item);
            float buyFactor = 1.05f;

            float newPrice = market.adjustPriceOnBuy(currentPrice, demand, supply, buyFactor);
            market.setPrice(item, newPrice); // update the price
        }
        getDebugConsole().log("Society", "Market prices adjusted due to societal growth.");
        timeAccumulator = 0.0f;
    }
}

// perform pathfinding for NPC to reach target tile
void Game::performPathfinding(NPCEntity& npc) {
    Tile* targetTile = npc.getTarget();
    if (!targetTile) {
        getDebugConsole().log("ERROR", npc.getName() + " has no target. Setting to idle.");
        npc.setState(NPCState::Idle);
        return;
    }

    sf::Vector2f targetPos = targetTile->getPosition();
    sf::Vector2f npcPos = npc.getPosition();
    sf::Vector2f direction = targetPos - npcPos;
    float distance = std::sqrt(direction.x * direction.x + direction.y * direction.y);

    if (distance > GameConfig::tileSize * 0.8f) { 
        if (distance > 0) {
            direction /= distance;
        }
        
        float moveSpeed = npc.getSpeed() * deltaTime * simulationSpeed;
        sf::Vector2f newPosition = npcPos + direction * moveSpeed;

        // boundary checking
        float mapWidth = GameConfig::mapWidth * GameConfig::tileSize;
        float mapHeight = GameConfig::mapHeight * GameConfig::tileSize;
        
        newPosition.x = std::clamp(newPosition.x, 0.0f, mapWidth - GameConfig::tileSize);
        newPosition.y = std::clamp(newPosition.y, 0.0f, mapHeight - GameConfig::tileSize);
        
        npc.setPosition(newPosition.x, newPosition.y);
        
        getDebugConsole().log("Pathfinding", npc.getName() + " moved to (" +
                            std::to_string(newPosition.x) + ", " + std::to_string(newPosition.y) + 
                            "), distance to target: " + std::to_string(distance));
    } else {
        // close enough to target
        npc.setState(NPCState::PerformingAction);
        getDebugConsole().log("Pathfinding", npc.getName() + " reached target");
    }
}

// aggregate resources from all NPC inventories
std::unordered_map<std::string, int> Game::aggregateResources(const std::vector<NPCEntity>& npcs) const {
    std::unordered_map<std::string, int> allResources;

    for (const auto& npc : npcs) {
        const auto& inventory = npc.getInventory();
        for (const auto& [item, quantity] : inventory) {
            allResources[item] += quantity;
        }
    }

    return allResources;
}

// generate the game map using Perlin noise
void Game::generateMap() {
    FastNoiseLite noise;
    noise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
    noise.SetFrequency(0.1f);
    noise.SetSeed(static_cast<int>(time(nullptr)));

    auto& textureManager = TextureManager::getInstance();

    // Load textures using TextureManager
    std::vector<const sf::Texture*> grassTextures = {
        &textureManager.getTexture("grass1", "../assets/tiles/grass/grass1.png"),
        &textureManager.getTexture("grass2", "../assets/tiles/grass/grass2.png"),
        &textureManager.getTexture("grass3", "../assets/tiles/grass/grass3.png")
    };

    std::vector<const sf::Texture*> rockTextures = {
        &textureManager.getTexture("rock1", "../assets/objects/rock1.png"),
        &textureManager.getTexture("rock2", "../assets/objects/rock2.png"),
        &textureManager.getTexture("rock3", "../assets/objects/rock3.png")
    };


    std::vector<const sf::Texture*> stoneTextures = {
        &textureManager.getTexture("stone1", "../assets/tiles/stone/stone1.png"),
        &textureManager.getTexture("stone2", "../assets/tiles/stone/stone2.png"),
        &textureManager.getTexture("stone3", "../assets/tiles/stone/stone3.png")
    };

    std::vector<const sf::Texture*> flowerTextures = {
        &textureManager.getTexture("flower1", "../assets/tiles/flower/flower1.png"),
        &textureManager.getTexture("flower2", "../assets/tiles/flower/flower2.png"),
        &textureManager.getTexture("flower3", "../assets/tiles/flower/flower3.png"),
        &textureManager.getTexture("flower4", "../assets/tiles/flower/flower4.png"),
        &textureManager.getTexture("flower5", "../assets/tiles/flower/flower5.png")
    };

    std::vector<const sf::Texture*> bushTextures = {
        &textureManager.getTexture("bush1", "../assets/objects/bush1.png"),
        &textureManager.getTexture("bush2", "../assets/objects/bush2.png")
    };

    std::vector<const sf::Texture*> treeTextures = {
        &textureManager.getTexture("tree1", "../assets/objects/tree1.png"),
        &textureManager.getTexture("tree2", "../assets/objects/tree2.png"),
        &textureManager.getTexture("tree3", "../assets/objects/tree3.png")
    };

    std::vector<const sf::Texture*> houseTextures = {
        &textureManager.getTexture("house1", "../assets/objects/house1.png"),
        &textureManager.getTexture("house2", "../assets/objects/house2.png"),
        &textureManager.getTexture("house3", "../assets/objects/house3.png")
    };

    std::vector<const sf::Texture*> marketTextures = {
        &textureManager.getTexture("market1", "../assets/objects/market1.png"),
        &textureManager.getTexture("market2", "../assets/objects/market2.png"),
        &textureManager.getTexture("market3", "../assets/objects/market3.png")
    };

    tileMap.resize(GameConfig::mapHeight);
    for (auto& row : tileMap) {
        row.resize(GameConfig::mapWidth);
        for (auto& tile : row) {
            tile = std::make_unique<Tile>();
        }
    }

    for (int i = 0; i < GameConfig::mapHeight; ++i) {
        for (int j = 0; j < GameConfig::mapWidth; ++j) {
            float noiseValue = noise.GetNoise(static_cast<float>(i), static_cast<float>(j));
            noiseValue = (noiseValue + 1.0f) / 2.0f;

            if (noiseValue < 0.1f) {
                tileMap[i][j] = std::make_unique<FlowerTile>(*flowerTextures[rand() % flowerTextures.size()]);
            } else if (noiseValue < 0.6f) {
                tileMap[i][j] = std::make_unique<GrassTile>(*grassTextures[rand() % grassTextures.size()]);
            } else {
                tileMap[i][j] = std::make_unique<StoneTile>(*stoneTextures[rand() % stoneTextures.size()]);
            }

            tileMap[i][j]->setPosition(j * GameConfig::tileSize, i * GameConfig::tileSize);

            int objectChance = rand() % 100;
            if (auto grassTile = dynamic_cast<GrassTile*>(tileMap[i][j].get())) {
                if (objectChance < 10) {
                    grassTile->placeObject(std::make_unique<Tree>(*treeTextures[rand() % treeTextures.size()]));
                } else if (objectChance < 20) {
                    grassTile->placeObject(std::make_unique<Bush>(*bushTextures[rand() % bushTextures.size()]));
                }
            } else if (auto stoneTile = dynamic_cast<StoneTile*>(tileMap[i][j].get())) {
                if (objectChance < 20) { 
                    stoneTile->placeObject(std::make_unique<Rock>(*rockTextures[rand() % rockTextures.size()]));
                }
            }
        }
    }

    std::set<std::pair<int, int>> occupiedPositions;
    for (int i = 0; i < GameConfig::NPCEntityCount; ++i) {
        int houseX, houseY;
        do {
            houseX = rand() % GameConfig::mapWidth;
            houseY = rand() % GameConfig::mapHeight;
        } while (occupiedPositions.count({houseX, houseY}) || tileMap[houseY][houseX]->hasObject());

        occupiedPositions.insert({houseX, houseY});

        sf::Color houseColor(rand() % 256, rand() % 256, rand() % 256);
        auto house = std::make_unique<House>(*houseTextures[i % houseTextures.size()]);
        house->getSprite().setColor(houseColor);

        tileMap[houseY][houseX]->placeObject(std::move(house));
    }

    int marketCount = 2 + rand() % 2;
    for (int m = 0; m < marketCount; ++m) {
        int marketX, marketY;
        do {
            marketX = rand() % GameConfig::mapWidth;
            marketY = rand() % GameConfig::mapHeight;
        } while (occupiedPositions.count({marketX, marketY}) || tileMap[marketY][marketX]->hasObject());

        occupiedPositions.insert({marketX, marketY});
        tileMap[marketY][marketX]->placeObject(std::make_unique<Market>(*marketTextures[m % marketTextures.size()]));
    }
}

// generate NPC entities with improved stat distribution and logging
std::vector<NPCEntity> Game::generateNPCEntities() const {
    std::vector<NPCEntity> npcs;
    std::set<std::pair<int, int>> occupiedPositions;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distX(0, GameConfig::mapWidth - 1);
    std::uniform_int_distribution<> distY(0, GameConfig::mapHeight - 1);
    // better stat distribution - higher minimums
    std::uniform_int_distribution<> healthDist(80, 120);  
    std::uniform_int_distribution<> energyDist(80, 120);  
    std::uniform_int_distribution<> moneyDist(100, 200);  

    for (int i = 0; i < GameConfig::NPCEntityCount; ++i) {
        int x, y;
        do {
            x = distX(gen);
            y = distY(gen);
        } while (occupiedPositions.count({x, y}));

        occupiedPositions.insert({x, y});

        sf::Color NPCEntityColor(rand() % 256, rand() % 256, rand() % 256);
        bool enableQLearning = true; // enable for all NPCs 

        try {
            NPCEntity npc("NPC" + std::to_string(i + 1), 
                         healthDist(gen),    // Health
                         70.0f,              // Hunger
                         energyDist(gen),    // Energy
                         150.0f,             // Speed
                         10,                 // Strength
                         moneyDist(gen),     // Money
                         enableQLearning);
            
            npc.setTexture(playerTexture, NPCEntityColor);
            npc.setPosition(x * GameConfig::tileSize, y * GameConfig::tileSize);
            npc.setHouse(const_cast<House*>(&house));

            getDebugConsole().log("NPC", "Created " + npc.getName() + 
                                " with Health=" + std::to_string(npc.getHealth()) + 
                                ", Energy=" + std::to_string(npc.getEnergy()) + 
                                ", Money=" + std::to_string(npc.getMoney()));

            npcs.emplace_back(std::move(npc));
        } catch (const std::exception& e) {
            getDebugConsole().log("ERROR", "Failed to create NPC " + std::to_string(i + 1) + ": " + std::string(e.what()));
        }
    }
    return npcs;
}

// render the game world
void Game::render() {
    // ALWAYS render the world with default view first
    window.setView(window.getDefaultView());
    
    // render tiles
    for (const auto& row : tileMap) {
        for (const auto& tile : row) {
            tile->draw(window);
        }
    }

    // render all NPCs
    for (const auto& npc : npcs) {
        if (!npc.isDead()) {
            npc.draw(window);
        }
    }
    getDebugConsole().log("Render", "Rendered " + std::to_string(npcs.size()) + " NPCs");

    // render tile borders if enabled
    if (showTileBorders) drawTileBorders();
}

// draw borders around each tile for debugging
void Game::drawTileBorders() {
    for (int i = 0; i < tileMap.size(); ++i) {
        for (int j = 0; j < tileMap[i].size(); ++j) {
            sf::RectangleShape border(sf::Vector2f(GameConfig::tileSize, GameConfig::tileSize));
            border.setPosition(j * GameConfig::tileSize, i * GameConfig::tileSize);
            border.setOutlineThickness(1);
            border.setOutlineColor(sf::Color::Black);
            border.setFillColor(sf::Color::Transparent);
            window.draw(border);
        }
    }
}

// log statistics at the end of each iteration
void Game::logIterationStats(int iteration) {
    updatePersistentStats();
    
    nlohmann::json statsJson;
    statsJson["iteration"] = iteration;
    statsJson["total_npcs"] = npcs.size();
    statsJson["total_money_spent"] = MoneyManager::getTotalMoneySpent();
    statsJson["total_money_earned"] = MoneyManager::getTotalMoneyEarned();
    statsJson["items_sold"] = market.getTotalItemsSold();
    statsJson["items_bought"] = market.getTotalItemsBought();
    statsJson["items_gathered"] = getTotalItemsGathered();
    statsJson["market_prices"] = market.getPrices();
    
    // persistent stats
    statsJson["persistent_stats"] = {
        {"total_iterations", persistentStats.totalIterations},
        {"total_items_gathered_all_time", persistentStats.totalItemsGatheredAllTime},
        {"total_items_sold_all_time", persistentStats.totalItemsSoldAllTime},
        {"total_money_spent_all_time", persistentStats.totalMoneySpentAllTime},
        {"total_money_earned_all_time", persistentStats.totalMoneyEarnedAllTime}
    };
    
    // data collection stats
    if (getDataCollector().isCollectingData()) {
        statsJson["data_collection"] = {
            {"total_experiences", getDataCollector().getTotalExperiences()},
            {"current_batch_size", getDataCollector().getCurrentBatchSize()}
        };
    }
    
    std::ofstream file("stats.json", std::ios::app);
    file << statsJson.dump(4) << std::endl;
    
    getDebugConsole().log("STATS", "Logged iteration " + std::to_string(iteration) + " stats: " +
                        "NPCs=" + std::to_string(npcs.size()) + 
                        ", Items=" + std::to_string(getTotalItemsGathered()) +
                        ", Experiences=" + std::to_string(getDataCollector().getTotalExperiences()));
}

// update persistent statistics across iterations
int Game::getTotalItemsGathered() const {
    int totalGathered = 0;
    
    // count from living NPCs
    for (const auto& npc : npcs) {
        if (!npc.isDead()) {
            totalGathered += npc.getTotalItemsGathered();
        }
    }
    
    // also count items in house storage and market transactions
    const auto& houseStorage = house.getStorage();
    for (const auto& [item, quantity] : houseStorage) {
        totalGathered += quantity;
    }
    
    // add items sold to market
    totalGathered += market.getTotalItemsSold();
    
    getDebugConsole().log("Stats", "Total items gathered (NPCs: " + std::to_string(totalGathered - market.getTotalItemsSold()) + 
                        ", Sold: " + std::to_string(market.getTotalItemsSold()) + 
                        ", Total: " + std::to_string(totalGathered) + ")");
    return totalGathered;
}

int Game::getTotalItemsMined() const {
    // for future implementation TODO
    return 0;
}

// reset the simulation 
void Game::resetSimulation() {
    static int iterationCounter = 0;
    iterationCounter++;

    getDebugConsole().log("SYSTEM", "Resetting simulation... Iteration " + std::to_string(iterationCounter));

    if (tensorFlowEnabled) {
        getDataCollector().stopCollection();
        getDataCollector().exportToJSON("iteration_" + std::to_string(iterationCounter) + "_data.json");
        getDataCollector().startCollection(); // Restart for next iteration
        getDebugConsole().log("DataCollection", "Saved iteration " + std::to_string(iterationCounter) + " training data");
    }

    clockGUI.reset();
    timeManager.incrementSocietyIteration();
    timeManager.reset();
    getDebugConsole().log("TIME", "Time and clock reset.");

    market.resetTransactions();
    market.randomizePrices();
    ui.resetMarketGraph();
    ui.updateMarketPanel(market);
    getDebugConsole().log("MARKET", "Market reset with new randomized prices.");

    npcs.clear();
    npcs.shrink_to_fit();
    npcs = generateNPCEntities();
    ui.updateNPCEntityList(npcs);
    getDebugConsole().log("NPC", "NPCs reset with fresh random stats.");

    tileMap.clear();
    tileMap.shrink_to_fit();
    generateMap();
    getDebugConsole().log("MAP", "Map reset and regenerated.");

    regenerateResources();
    getDebugConsole().log("RESOURCES", "Resources regenerated.");

    ui.updateStatus(timeManager.getCurrentDay(), timeManager.getFormattedTime(), timeManager.getSocietyIteration());

    simulationSpeed = 1.0f;

    getDebugConsole().log("SYSTEM", "Simulation reset complete.");
}

// toggle tile border visibility
void Game::toggleTileBorders() {
    showTileBorders = !showTileBorders;
    getDebugConsole().log("Options", "Tile borders toggled: " + std::string(showTileBorders ? "ON" : "OFF"));
}

// set simulation speed factor
void Game::setSimulationSpeed(float speedFactor) {
    simulationSpeed = std::clamp(speedFactor, 0.1f, 3.0f); // clamp to a valid range
    getDebugConsole().log("Options", "Simulation speed set to: " + std::to_string(simulationSpeed));
}
