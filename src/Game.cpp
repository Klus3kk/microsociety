#include "Game.hpp"
#include "FastNoiseLite.h"
#include "debug.hpp"

#include <nlohmann/json.hpp>
#include "NPCEntity.hpp"
#include "PlayerEntity.hpp"
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

// Constructor
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
    npcs = generateNPCEntitys(); 

    ui.updateNPCEntityList(npcs);
    
    // Initialize TensorFlow if enabled
    if (tensorFlowEnabled) {
        getDebugConsole().log("TensorFlow", "Initializing TensorFlow integration...");
        getDebugConsole().log("DataCollection", "Starting data collection mode for training...");
        
        // Start data collection
        getDataCollector().startCollection();
        getDataCollector().setMaxExperiencesPerFile(5000); // Save every 5000 experiences
        
        initializeNPCTensorFlow();
    }
    
    // Player will be created when enableSinglePlayerMode(true) is called
}

// Destructor
Game::~Game() {
    cleanupPlayer();
    
    // Save collected training data when exiting
    if (tensorFlowEnabled) {
        getDebugConsole().log("DataCollection", "Saving collected training data...");
        getDataCollector().stopCollection();
        
        // Export data in multiple formats for Python training
        getDataCollector().exportToJSON("training_data.json");
        getDataCollector().exportToCSV("training_data.csv");
        
        // Print collection statistics
        getDataCollector().printStatistics();
        getDataCollector().analyzeDataQuality();
        
        getDebugConsole().log("DataCollection", "Data collection complete. Total experiences: " + 
                            std::to_string(getDataCollector().getTotalExperiences()));
    }
}

void Game::cleanupPlayer() {
    if (player) {
        delete player;
        player = nullptr;
    }
}

void Game::enableSinglePlayerMode(bool enable) { 
    singlePlayerMode = enable; 
    
    if (enable) {
        getDebugConsole().log("Player", "Enabling single player mode...");
        
        // Clean up existing player first
        cleanupPlayer();
        
        // Clear existing NPCs
        npcs.clear();
        
        // Create exactly one NPC for single player mode
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> distX(1, GameConfig::mapWidth - 2);
        std::uniform_int_distribution<> distY(1, GameConfig::mapHeight - 2);
        
        try {
            // Create NPC companion with proper initialization
            NPCEntity singleNPC("Companion", 100, 50, 100, 150.0f, 10, 100, reinforcementLearningEnabled);
            singleNPC.setTexture(playerTexture, sf::Color::Blue); // Blue color to distinguish from player
            
            float npcX = distX(gen) * GameConfig::tileSize;
            float npcY = distY(gen) * GameConfig::tileSize;
            singleNPC.setPosition(npcX, npcY);
            singleNPC.setHouse(&house); // Assign house reference
            
            npcs.emplace_back(std::move(singleNPC));
            getDebugConsole().log("Player", "Created NPC companion at (" + std::to_string(npcX) + ", " + std::to_string(npcY) + ")");
        } catch (const std::exception& e) {
            getDebugConsole().log("ERROR", "Failed to create NPC companion: " + std::string(e.what()));
            return;
        }
        
        // Create player
        try {
            player = new PlayerEntity(
                "Player", 
                120.0f,     // Health
                60.0f,      // Hunger  
                100.0f,     // Energy
                200.0f,     // Speed
                15.0f,      // Strength
                200.0f      // Money
            );
            
            // Set player texture and position
            player->setTexture(playerTexture, sf::Color::White);
            
            float playerX = distX(gen) * GameConfig::tileSize;
            float playerY = distY(gen) * GameConfig::tileSize;
            player->setPosition(playerX, playerY);
            
            getDebugConsole().log("Player", "Created player at (" + std::to_string(playerX) + ", " + std::to_string(playerY) + ")");
            getDebugConsole().log("SUCCESS", "Single player mode setup complete");
        } catch (const std::exception& e) {
            getDebugConsole().log("ERROR", "Failed to create player: " + std::string(e.what()));
            cleanupPlayer();
            return;
        }
        
    } else if (!enable && player) {
        getDebugConsole().log("Player", "Disabling single player mode...");
        
        // Reset to normal NPC count for other modes
        npcs.clear();
        npcs = generateNPCEntitys();
        
        // Clean up player
        cleanupPlayer();
        getDebugConsole().log("Game", "Returned to multi-NPC mode with " + std::to_string(npcs.size()) + " NPCs");
    }
    
    // Update UI
    ui.updateNPCList(npcs);
    getDebugConsole().log("UI", "Updated NPC list for single player mode: " + std::string(enable ? "ON" : "OFF"));
}

void Game::initializeNPCTensorFlow() {
    #ifdef USE_TENSORFLOW
        getDebugConsole().log("TensorFlow", "TensorFlow C API version: " + std::string(TF_Version()));
        
        // Check if TF model file exists
        std::ifstream modelFile("models/npc_rl_model.tflite");
        if (!modelFile.good()) {
            getDebugConsole().log("TensorFlow", "No pre-trained model found. Running in DATA COLLECTION mode.", LogLevel::Warning);
            getDebugConsole().log("TensorFlow", "NPCs will use random/exploration actions to gather training data.");
            
            // Enable TensorFlow mode on NPCs but without model (for data collection)
            for (auto& npc : npcs) {
                npc.setTensorFlowModel(nullptr); // No model = data collection mode
                npc.enableTensorFlow(true);      // Enable TF flag for data collection
            }
            
            return;
        }
        
        getDebugConsole().log("TensorFlow", "Pre-trained model found, loading for inference...");
        
        // Initialize TensorFlow models for NPCs
        auto tfModel = std::make_shared<TensorFlowWrapper>();
        if (tfModel->initialize("models/npc_rl_model.tflite")) {
            getDebugConsole().log("TensorFlow", "TensorFlow model loaded successfully.");
            
            // Apply TF model to NPCs
            for (auto& npc : npcs) {
                npc.setTensorFlowModel(tfModel);
                npc.enableTensorFlow(true);
            }
        } else {
            getDebugConsole().log("TensorFlow", "Failed to load TensorFlow model, switching to data collection mode", LogLevel::Error);
            
            // Fallback to data collection mode
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

void Game::checkDataCollectionProgress() {
    if (tensorFlowEnabled && getDataCollector().isCollectingData()) {
        size_t totalExperiences = getDataCollector().getTotalExperiences();
        
        // Auto-export every 10,000 experiences for training
        if (totalExperiences > 0 && totalExperiences % 10000 == 0) {
            getDebugConsole().log("DataCollection", "Reached " + std::to_string(totalExperiences) + 
                                " experiences. Exporting batch for training...");
            
            std::string filename = "batch_" + std::to_string(totalExperiences / 10000) + "_data.csv";
            getDataCollector().exportToCSV(filename);
            getDataCollector().printStatistics();
        }
    }
}

void Game::handlePlayerInput() {
    if (!singlePlayerMode || !player) return;
    
    // Handle player movement
    player->handleInput(deltaTime * simulationSpeed);
    
    // Handle interaction with E key
    static bool eKeyPressed = false;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::E)) {
        if (!eKeyPressed) {
            eKeyPressed = true;
            
            // Find target tile under player's position
            sf::Vector2f playerPos = player->getPosition();
            Tile* targetTile = player->getTargetTileAt(tileMap, playerPos.x, playerPos.y);
            
            if (targetTile) {
                player->interactWithTile(*targetTile, tileMap, market, house);
            }
        }
    } else {
        eKeyPressed = false;
    }
}

void Game::updatePlayer() {
    if (!singlePlayerMode || !player) return;
    
    player->update(deltaTime * simulationSpeed);
    
    // Check if player died
    if (player->isDead()) {
        getDebugConsole().log("Player", "Player has died. Restarting...");
        
        // Reset player
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> distX(1, GameConfig::mapWidth - 2);
        std::uniform_int_distribution<> distY(1, GameConfig::mapHeight - 2);
        
        player->setHealth(120.0f);
        player->setEnergy(100.0f);
        player->setDead(false);
        player->setPosition(distX(gen) * GameConfig::tileSize, distY(gen) * GameConfig::tileSize);
    }
    
    // Check collision
    detectCollision(*player);
}

const std::vector<std::vector<std::unique_ptr<Tile>>>& Game::getTileMap() const {
    return tileMap;
}

// FIXED: Now works with Entity base class - supports both NPCs and Players
bool Game::detectCollision(Entity& entity) {
    int tileX = static_cast<int>(entity.getPosition().x / GameConfig::tileSize);
    int tileY = static_cast<int>(entity.getPosition().y / GameConfig::tileSize);

    if (tileX >= 0 && tileX < tileMap[0].size() && tileY >= 0 && tileY < tileMap.size()) {
        Tile& targetTile = *tileMap[tileY][tileX];
        
        // Handle collision with tile objects
        if (targetTile.hasObject()) {
            ObjectType objType = targetTile.getObject()->getType();
            
            // Different handling for NPCs vs Players
            if (auto* npc = dynamic_cast<NPCEntity*>(&entity)) {
                // NPC-specific collision handling
                npc->performAction(ActionType::RegenerateEnergy, targetTile, tileMap, market, house);
            } else if (auto* player = dynamic_cast<PlayerEntity*>(&entity)) {
                // Player-specific collision handling (if needed)
                getDebugConsole().log("Player", "Player collision detected with object type: " + 
                                    std::to_string(static_cast<int>(objType)));
            }
        }
    }
    return false;
}

void Game::run() {
    sf::Clock clock;
    window.setFramerateLimit(60);
    
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

        // Handle player input in single player mode
        if (singlePlayerMode) {
            handlePlayerInput();
            updatePlayer();
        }

        // Always simulate market
        market.simulateMarketDynamics(deltaTime);
        
        // Update resource regeneration
        resourceRegenerationTimer += deltaTime;
        if (resourceRegenerationTimer >= regenerationInterval) {
            regenerateResources();
            resourceRegenerationTimer = 0.0f;
        }

        // Always simulate NPCs (1 in single player, many in other modes)
        simulateNPCEntityBehavior(deltaTime * simulationSpeed);
        simulateSocietalGrowth(deltaTime * simulationSpeed);

        // NEW: Check data collection progress
        checkDataCollectionProgress();

        // Update UI
        ui.updateMoney(singlePlayerMode && player ? 
                      static_cast<int>(player->getMoney()) : 
                      MoneyManager::calculateTotalMoney(npcs));
        timeManager.update(deltaTime);
        clockGUI.update(timeManager.getElapsedTime());
        ui.updateStatus(
            timeManager.getCurrentDay(),
            timeManager.getFormattedTime(),
            timeManager.getSocietyIteration()
        );
        ui.updateStats(npcs, timeManager);
        ui.updateMarketPanel(market);
        ui.updateNPCList(npcs);

        // ALWAYS render everything - no view switching complexity
        window.clear();
        render(); // Uses default view only
        clockGUI.render(window, isClockVisible);
        ui.render(window, market, npcs);
        getDebugConsole().render(window);
        window.display();
    }
}

void Game::simulateNPCEntityBehavior(float deltaTime) {
    static std::unordered_map<std::string, int> stuckCounter;
    static std::unordered_map<std::string, sf::Vector2f> lastPosition;
    static std::unordered_map<std::string, float> stuckTimer;
    
    for (auto it = npcs.begin(); it != npcs.end(); ) {
        NPCEntity& npc = *it;
        
        // FIXED: Always update NPCs regardless of state
        npc.update(deltaTime);
        
        // Check if NPC is dead
        if (npc.isDead() || npc.getHealth() <= 0 || npc.getEnergy() <= 0) {
            getDebugConsole().log("DEATH", npc.getName() + " has died.");
            it = npcs.erase(it);
            continue;
        }
        
        // FIXED: Anti-stuck mechanism - check if NPC hasn't moved
        sf::Vector2f currentPos = npc.getPosition();
        if (lastPosition.find(npc.getName()) != lastPosition.end()) {
            sf::Vector2f lastPos = lastPosition[npc.getName()];
            float distanceMoved = std::hypot(currentPos.x - lastPos.x, currentPos.y - lastPos.y);
            
            if (distanceMoved < 5.0f) { // If moved less than 5 pixels
                stuckTimer[npc.getName()] += deltaTime;
                if (stuckTimer[npc.getName()] > 3.0f) { // Stuck for 3 seconds
                    // Force NPC to move to random position
                    std::random_device rd;
                    std::mt19937 gen(rd());
                    std::uniform_int_distribution<> distX(1, GameConfig::mapWidth - 2);
                    std::uniform_int_distribution<> distY(1, GameConfig::mapHeight - 2);
                    
                    float newX = distX(gen) * GameConfig::tileSize;
                    float newY = distY(gen) * GameConfig::tileSize;
                    npc.setPosition(newX, newY);
                    npc.setState(NPCState::Idle);
                    npc.setTarget(nullptr);
                    
                    stuckTimer[npc.getName()] = 0.0f;
                    getDebugConsole().log("UNSTUCK", npc.getName() + " was teleported to unstuck at (" + 
                                        std::to_string(newX) + ", " + std::to_string(newY) + ")");
                }
            } else {
                stuckTimer[npc.getName()] = 0.0f; // Reset timer if moving
            }
        }
        lastPosition[npc.getName()] = currentPos;
        
        // FIXED: Better state machine with forced transitions
        switch (npc.getState()) {
            case NPCState::Idle: {
                ActionType actionType = npc.decideNextAction(tileMap, house, market);
                npc.setCurrentAction(actionType);
                
                Tile* nearestTile = nullptr;
                
                // FIXED: Better target selection
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
                        // Rest in place - no movement needed
                        npc.setState(NPCState::PerformingAction);
                        break;
                    default:
                        // For unknown actions, just rest
                        npc.setCurrentAction(ActionType::Rest);
                        npc.setState(NPCState::PerformingAction);
                        break;
                }
                
                if (nearestTile) {
                    npc.setTarget(nearestTile);
                    npc.setState(NPCState::Walking);
                    stuckCounter[npc.getName()] = 0;
                    getDebugConsole().log("NPC", npc.getName() + " walking to " + 
                                        std::to_string(static_cast<int>(actionType)));
                } else if (npc.getCurrentAction() != ActionType::Rest) {
                    // FIXED: If no target found, force rest action
                    npc.setCurrentAction(ActionType::Rest);
                    npc.setState(NPCState::PerformingAction);
                    getDebugConsole().log("NPC", npc.getName() + " no target found, resting instead");
                }
                break;
            }
            
            case NPCState::Walking: {
                if (npc.getTarget() && !npc.isAtTarget()) {
                    // FIXED: Improved pathfinding
                    performPathfinding(npc);
                    
                    // FIXED: Reasonable costs for movement
                    npc.reduceHealth(0.01f * deltaTime); // Very small health cost
                    npc.consumeEnergy(0.5f * deltaTime); // Small energy cost
                    
                    // FIXED: Check if we're close enough to the target
                    sf::Vector2f targetPos = npc.getTarget()->getPosition();
                    sf::Vector2f npcPos = npc.getPosition();
                    float distance = std::hypot(targetPos.x - npcPos.x, targetPos.y - npcPos.y);
                    
                    if (distance < GameConfig::tileSize * 1.5f) { // Close enough
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
                    // Perform action without target (like Rest)
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
                // FIXED: Don't stay in this state - move to idle
                npc.setState(NPCState::Idle);
                break;
            }
        }
        
        // FIXED: Force state transition if stuck in same state too long
        static std::unordered_map<std::string, float> stateTimer;
        static std::unordered_map<std::string, NPCState> lastState;
        
        if (lastState[npc.getName()] == npc.getState()) {
            stateTimer[npc.getName()] += deltaTime;
            if (stateTimer[npc.getName()] > 5.0f) { // Stuck in state for 5 seconds
                npc.setState(NPCState::Idle);
                npc.setTarget(nullptr);
                stateTimer[npc.getName()] = 0.0f;
                getDebugConsole().log("STATE", npc.getName() + " was stuck in state, forced to idle");
            }
        } else {
            stateTimer[npc.getName()] = 0.0f;
        }
        lastState[npc.getName()] = npc.getState();
        
        ++it;
    }
    
    // FIXED: Restart simulation if all NPCs die (only in non-single player mode)
    if (npcs.empty() && !singlePlayerMode) {
        getDebugConsole().log("SYSTEM", "All NPCs have died. Restarting simulation...");
        resetSimulation();
    }
}


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

void Game::moveToResource(NPCEntity& npc, ActionType actionType) {
    int currentX = static_cast<int>(npc.getPosition().x / GameConfig::tileSize);
    int currentY = static_cast<int>(npc.getPosition().y / GameConfig::tileSize);

    // Locate the target resource
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

        // Use ActionType for actions instead of std::make_unique
        npc.performAction(actionType, targetTile, tileMap, market, house);
    }
}

void Game::storeItems(NPCEntity& npc, Tile& tile) {
    auto inventory = npc.getInventory();
    std::string mostAbundantResource;
    int maxQuantity = 0;

    // Find the most abundant resource
    for (const auto& [item, quantity] : inventory) {
        if (quantity > maxQuantity) {
            mostAbundantResource = item;
            maxQuantity = quantity;
        }
    }

    if (!mostAbundantResource.empty()) {
        // Use ActionType::StoreItem
        npc.performAction(ActionType::StoreItem, tile, tileMap, market, house);
    }
}

void Game::evaluateNPCEntityState(NPCEntity& NPCEntity) {
    if (NPCEntity.getEnergy() <= 0.0f) {
        getDebugConsole().log("NPCEntity", NPCEntity.getName() + " ran out of energy and died.");
        // Handle NPCEntity death (remove or reset state)
    }
}

void Game::regenerateResources() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> distX(0, GameConfig::mapWidth - 1);
    std::uniform_int_distribution<int> distY(0, GameConfig::mapHeight - 1);
    std::uniform_real_distribution<float> resourceChance(0.0f, 1.0f); // For probability-based spawning

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

    int numResourcesToRegenerate = GameConfig::mapWidth * GameConfig::mapHeight * 0.05; // Increased to 5% of map tiles

    for (int i = 0; i < numResourcesToRegenerate; ++i) {
        int x = distX(gen);
        int y = distY(gen);

        if (!tileMap[y][x]->hasObject()) {
            float chance = resourceChance(gen);

            // Higher chance for Trees/Bushes on GrassTile
            if (auto grassTile = dynamic_cast<GrassTile*>(tileMap[y][x].get())) {
                if (chance < 0.4f) { // 40% chance for Trees
                    grassTile->placeObject(std::make_unique<Tree>(*treeTextures[rand() % treeTextures.size()]));
                    getDebugConsole().log("Resource Regen", "Tree spawned at (" + std::to_string(x) + ", " + std::to_string(y) + ")");
                } else if (chance < 0.7f) { // 30% chance for Bushes
                    grassTile->placeObject(std::make_unique<Bush>(*bushTextures[rand() % bushTextures.size()]));
                    getDebugConsole().log("Resource Regen", "Bush spawned at (" + std::to_string(x) + ", " + std::to_string(y) + ")");
                }
            }
            
            // Higher chance for Rocks on StoneTile
            else if (auto stoneTile = dynamic_cast<StoneTile*>(tileMap[y][x].get())) {
                if (chance < 0.8f) { // 80% chance for Rocks on StoneTile
                    stoneTile->placeObject(std::make_unique<Rock>(*rockTextures[rand() % rockTextures.size()]));
                    getDebugConsole().log("Resource Regen", "Rock spawned at (" + std::to_string(x) + ", " + std::to_string(y) + ")");
                }
            }
        }
    }
}

void Game::simulateSocietalGrowth(float deltaTime) {
    // Example societal growth logic: increase market prices as demand rises
    static float timeAccumulator = 0.0f;
    timeAccumulator += deltaTime;

    if (timeAccumulator >= 60.0f) { // Adjust market prices every 60 seconds
        for (const auto& [item, currentPrice] : market.getPrices()) {
            int demand = market.getBuyTransactions(item);
            int supply = market.getSellTransactions(item);
            float buyFactor = 1.05f;

            float newPrice = market.adjustPriceOnBuy(currentPrice, demand, supply, buyFactor);
            market.setPrice(item, newPrice); // Update the price
        }
        getDebugConsole().log("Society", "Market prices adjusted due to societal growth.");
        timeAccumulator = 0.0f;
    }
}

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

    if (distance > GameConfig::tileSize * 0.8f) { // FIXED: Better distance threshold
        // Normalize direction
        if (distance > 0) {
            direction /= distance;
        }
        
        // FIXED: Reasonable movement speed
        float moveSpeed = npc.getSpeed() * deltaTime * simulationSpeed;
        sf::Vector2f newPosition = npcPos + direction * moveSpeed;

        // FIXED: Boundary checking
        float mapWidth = GameConfig::mapWidth * GameConfig::tileSize;
        float mapHeight = GameConfig::mapHeight * GameConfig::tileSize;
        
        newPosition.x = std::clamp(newPosition.x, 0.0f, mapWidth - GameConfig::tileSize);
        newPosition.y = std::clamp(newPosition.y, 0.0f, mapHeight - GameConfig::tileSize);
        
        npc.setPosition(newPosition.x, newPosition.y);
        
        getDebugConsole().log("Pathfinding", npc.getName() + " moved to (" +
                            std::to_string(newPosition.x) + ", " + std::to_string(newPosition.y) + 
                            "), distance to target: " + std::to_string(distance));
    } else {
        // Close enough to target
        npc.setState(NPCState::PerformingAction);
        getDebugConsole().log("Pathfinding", npc.getName() + " reached target");
    }
}

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

std::vector<NPCEntity> Game::generateNPCEntitys() const {
    std::vector<NPCEntity> npcs;
    std::set<std::pair<int, int>> occupiedPositions;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distX(0, GameConfig::mapWidth - 1);
    std::uniform_int_distribution<> distY(0, GameConfig::mapHeight - 1);
    std::uniform_int_distribution<> statDist(50, 150);

    for (int i = 0; i < GameConfig::NPCEntityCount; ++i) {
        int x, y;
        do {
            x = distX(gen);
            y = distY(gen);
        } while (occupiedPositions.count({x, y}));

        occupiedPositions.insert({x, y});

        sf::Color NPCEntityColor(rand() % 256, rand() % 256, rand() % 256);
        bool enableQLearning = (i % 2 == 0); // Enable Q-learning for every other NPC

        try {
            NPCEntity npc("NPC" + std::to_string(i + 1), statDist(gen), statDist(gen), statDist(gen),
                          150.0f, 10, statDist(gen), enableQLearning);
            npc.setTexture(playerTexture, NPCEntityColor);
            npc.setPosition(x * GameConfig::tileSize, y * GameConfig::tileSize);
            
            // FIXED: Assign house reference - use const_cast for this special case
            npc.setHouse(const_cast<House*>(&house));

            if (enableQLearning) {
                getDebugConsole().log("DEBUG", npc.getName() + " has Q-Learning enabled.");
            } else {
                getDebugConsole().log("DEBUG", npc.getName() + " uses simple behavior.");
            }

            npcs.emplace_back(std::move(npc));
        } catch (const std::exception& e) {
            getDebugConsole().log("ERROR", "Failed to create NPC " + std::to_string(i + 1) + ": " + std::string(e.what()));
        }
    }
    return npcs;
}


void Game::render() {
    // ALWAYS render the world with default view first
    window.setView(window.getDefaultView());
    
    // Render tiles
    for (const auto& row : tileMap) {
        for (const auto& tile : row) {
            tile->draw(window);
        }
    }

    // Render entities based on mode
    if (singlePlayerMode) {
        // Single player mode: render the player first
        if (player && !player->isDead()) {
            player->draw(window);
            getDebugConsole().log("Render", "Player rendered at (" + 
                                std::to_string(player->getPosition().x) + ", " + 
                                std::to_string(player->getPosition().y) + ")");
        } else {
            getDebugConsole().log("ERROR", "Player is NULL or dead in single player mode!");
        }
        
        // Single player mode: render exactly ONE NPC companion if it exists
        if (!npcs.empty() && !npcs[0].isDead()) {
            npcs[0].draw(window); 
            getDebugConsole().log("Render", "NPC companion rendered");
        } else if (npcs.empty()) {
            getDebugConsole().log("ERROR", "No NPC companion found in single player mode!");
        }
    } else {
        // Multi-NPC modes: render all NPCs
        for (const auto& npc : npcs) {
            if (!npc.isDead()) {
                npc.draw(window);
            }
        }
        getDebugConsole().log("Render", "Rendered " + std::to_string(npcs.size()) + " NPCs");
    }

    // Render tile borders if enabled
    if (showTileBorders) drawTileBorders();
}

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

void Game::logIterationStats(int iteration) {
    nlohmann::json statsJson;
    statsJson["iteration"] = iteration;
    statsJson["total_npcs"] = npcs.size();
    statsJson["total_money_spent"] = MoneyManager::getTotalMoneySpent();
    statsJson["total_money_earned"] = MoneyManager::getTotalMoneyEarned();
    statsJson["items_sold"] = market.getTotalItemsSold();
    statsJson["items_bought"] = market.getTotalItemsBought();
    statsJson["items_gathered"] = getTotalItemsGathered();
    statsJson["market_prices"] = market.getPrices();
    
    std::ofstream file("stats.json", std::ios::app);
    file << statsJson.dump(4) << std::endl;
}

int Game::getTotalItemsGathered() const {
    int totalGathered = 0;
    for (const auto& npc : npcs) {
        totalGathered += npc.getGatheredResources();
    }
    return totalGathered;
}

int Game::getTotalItemsMined() const {
    // For future implementation if needed
    return 0;
}


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
    logIterationStats(iterationCounter);

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
    npcs = generateNPCEntitys();
    ui.updateNPCEntityList(npcs);
    getDebugConsole().log("NPC", "NPCs reset with fresh random stats.");

    tileMap.clear();
    tileMap.shrink_to_fit();
    generateMap();
    getDebugConsole().log("MAP", "Map reset and regenerated.");

    regenerateResources();
    getDebugConsole().log("RESOURCES", "Resources regenerated.");

    // Reset player in single player mode
    if (singlePlayerMode && player) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> distX(1, GameConfig::mapWidth - 2);
        std::uniform_int_distribution<> distY(1, GameConfig::mapHeight - 2);
        
        player->setHealth(120.0f);
        player->setEnergy(100.0f);
        player->setDead(false);
        player->setPosition(distX(gen) * GameConfig::tileSize, distY(gen) * GameConfig::tileSize);
        getDebugConsole().log("PLAYER", "Player reset to new position.");
    }

    ui.updateStatus(timeManager.getCurrentDay(), timeManager.getFormattedTime(), timeManager.getSocietyIteration());

    simulationSpeed = 1.0f;

    getDebugConsole().log("SYSTEM", "Simulation reset complete.");
}

void Game::toggleTileBorders() {
    showTileBorders = !showTileBorders;
    getDebugConsole().log("Options", "Tile borders toggled: " + std::string(showTileBorders ? "ON" : "OFF"));
}

void Game::setSimulationSpeed(float speedFactor) {
    simulationSpeed = std::clamp(speedFactor, 0.1f, 3.0f); // Clamp to a valid range
    getDebugConsole().log("Options", "Simulation speed set to: " + std::to_string(simulationSpeed));
}