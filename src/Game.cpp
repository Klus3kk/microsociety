#include "Game.hpp"
#include "FastNoiseLite.h"
#include "debug.hpp"
#include <random>
#include <set>
#include <unordered_map>
#include <algorithm>
#include <thread>
// Constructor
Game::Game()
    : window(sf::VideoMode(GameConfig::windowWidth, GameConfig::windowHeight), "MicroSociety", sf::Style::Titlebar | sf::Style::Close),
      house(TextureManager::getInstance().getTexture("house1", "../assets/objects/house1.png"), 1),
      market(), 
      clockGUI(700, 100) {
#ifdef _WIN32
    ui.adjustLayout(window);
#endif
    playerTexture.loadFromFile("../assets/npc/person1.png"); // Adjust path as needed
    if (!playerTexture.getSize().x) {
        std::cerr << "Failed to load player texture!" << std::endl;
    }

    market.setPrice("wood", 1 + std::rand() % 50);
    market.setPrice("stone", 1 + std::rand() % 50);
    market.setPrice("bush", 1 + std::rand() % 50);

    generateMap();
    npcs = generateNPCEntitys(); 

    ui.updateNPCEntityList(npcs); // Update UI with NPCEntity list
}

const std::vector<std::vector<std::unique_ptr<Tile>>>& Game::getTileMap() const {
    return tileMap;
}

bool Game::detectCollision(NPCEntity& NPCEntity) {
    int tileX = static_cast<int>(NPCEntity.getPosition().x / GameConfig::tileSize);
    int tileY = static_cast<int>(NPCEntity.getPosition().y / GameConfig::tileSize);

    if (tileX >= 0 && tileX < tileMap[0].size() && tileY >= 0 && tileY < tileMap.size()) {
        Tile& targetTile = *tileMap[tileY][tileX];
        
        // Pass Market and House references
        NPCEntity.performAction(ActionType::RegenerateEnergy, targetTile, tileMap, market, house);
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

        market.simulateMarketDynamics(deltaTime);
        // Update resource regeneration timer
        resourceRegenerationTimer += deltaTime;
        if (resourceRegenerationTimer >= regenerationInterval) {
            regenerateResources();
            resourceRegenerationTimer = 0.0f;
        }

        // Simulate NPC behavior
        simulateNPCEntityBehavior(deltaTime * simulationSpeed);
        simulateSocietalGrowth(deltaTime * simulationSpeed);

        // Other updates and rendering
        ui.updateMoney(MoneyManager::calculateTotalMoney(npcs));
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

        window.clear();
        render();
        clockGUI.render(window, isClockVisible);
        ui.render(window, market, npcs);
        getDebugConsole().render(window);
        window.display();
    }
    // getDebugConsole().saveLogsToFile("logs/simulation_log.txt");
}

void Game::simulateNPCEntityBehavior(float deltaTime) {
    static std::unordered_map<std::string, int> stuckCounter;
    static std::unordered_map<std::string, int> idleCounter;  

    for (auto it = npcs.begin(); it != npcs.end(); ) {
        // getDebugConsole().log("DEBUG", "Processing " + it->getName());
        NPCEntity& npc = *it;

        npc.update(deltaTime);

        if (it->isDead() || it->getHealth() <= 0 || it->getEnergy() <= 0) {
            getDebugConsole().log("DEATH", it->getName() + " has died.");
            it = npcs.erase(it);
            continue;
        }

        switch (it->getState()) {
            case NPCState::Idle: {
                ActionType actionType = it->decideNextAction(tileMap, house, market);
                it->setCurrentAction(actionType);

                Tile* nearestTile = nullptr;

                if (actionType == ActionType::ChopTree) nearestTile = it->findNearestTile(tileMap, ObjectType::Tree);
                else if (actionType == ActionType::MineRock) nearestTile = it->findNearestTile(tileMap, ObjectType::Rock);
                else if (actionType == ActionType::GatherBush) nearestTile = it->findNearestTile(tileMap, ObjectType::Bush);
                else if (actionType == ActionType::BuyItem || actionType == ActionType::SellItem)
                    nearestTile = it->findNearestTile(tileMap, ObjectType::Market);
                else if (actionType == ActionType::RegenerateEnergy || actionType == ActionType::UpgradeHouse)
                    nearestTile = it->findNearestTile(tileMap, ObjectType::House);

                if (nearestTile) {
                    npc.setTarget(nearestTile);
                    npc.setState(NPCState::Walking);
                    stuckCounter[npc.getName()] = 0;
                    idleCounter[npc.getName()] = 0;
                    getDebugConsole().log("NPC", npc.getName() + " is walking to action: " +
                                          std::to_string(static_cast<int>(actionType)));
                } else {
                    stuckCounter[npc.getName()]++;
                    if (stuckCounter[npc.getName()] > 3) {
                        ActionType randomAction = static_cast<ActionType>(1 + rand() % (static_cast<int>(ActionType::SellItem)));
                        npc.setCurrentAction(randomAction);
                        stuckCounter[npc.getName()] = 0;
                        getDebugConsole().log("FIX", npc.getName() + " was stuck, forced random action.");
                    }
                    npc.setState(NPCState::Idle);
                    // getDebugConsole().log("ERROR", it->getName() + " has no valid target.");
                }
                break;
            }

            case NPCState::Walking:
                if (it->getTarget() && !it->isAtTarget()) {
                    performPathfinding(*it);
                    it->reduceHealth(0.05f);
                    it->consumeEnergy(0.1f);  // ✅ Ensures NPCs consume energy when moving
                } else {
                    it->setState(NPCState::PerformingAction);
                }
                break;

            case NPCState::PerformingAction:
                if (it->getTarget()) {
                    it->performAction(it->getCurrentAction(), *it->getTarget(), tileMap, market, house);
                    it->setTarget(nullptr);
                }
                it->setState(NPCState::Idle);
                break;
        }

        // ✅ Enforce activity if idle too long
        if (it->getState() == NPCState::Idle) {
            idleCounter[it->getName()]++;
            if (idleCounter[it->getName()] > 5) {
                ActionType randomAction = static_cast<ActionType>(1 + rand() % (static_cast<int>(ActionType::SellItem)));
                it->setCurrentAction(randomAction);
                idleCounter[it->getName()] = 0;
                getDebugConsole().log("FIX", it->getName() + " was idle too long, forced random action.");
            }
        } else {
            idleCounter[it->getName()] = 0;
        }

        ++it;
    }

    if (npcs.empty()) {
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

    int numResourcesToRegenerate = GameConfig::mapWidth * GameConfig::mapHeight * 0.05; // 🔥 Increased to 5% of map tiles

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
        getDebugConsole().log("ERROR", npc.getName() + " has no target. Staying idle.");
        return;
    }

    sf::Vector2f targetPos = targetTile->getPosition();
    sf::Vector2f direction = targetPos - npc.getPosition();
    float distance = std::sqrt(direction.x * direction.x + direction.y * direction.y);

    if (distance > GameConfig::tileSize / 2.0f) {
        direction /= distance;
        sf::Vector2f newPosition = npc.getPosition() + direction * npc.getSpeed() * deltaTime * simulationSpeed;

        if (newPosition.x >= 0 && newPosition.x < GameConfig::mapWidth * GameConfig::tileSize &&
            newPosition.y >= 0 && newPosition.y < GameConfig::mapHeight * GameConfig::tileSize) {
            npc.setPosition(newPosition.x, newPosition.y);
            getDebugConsole().log("Pathfinding", npc.getName() + " moved to (" +
                                std::to_string(newPosition.x) + ", " + std::to_string(newPosition.y) + ")");
        }
    } else {
        npc.setState(NPCState::PerformingAction);
        getDebugConsole().log("Pathfinding", npc.getName() + " reached the target.");
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

        NPCEntity npc("NPC" + std::to_string(i + 1), statDist(gen), statDist(gen), statDist(gen),
                      150.0f, 10, statDist(gen), enableQLearning);
        npc.setTexture(playerTexture, NPCEntityColor);
        npc.setPosition(x * GameConfig::tileSize, y * GameConfig::tileSize);

        if (enableQLearning) {
            getDebugConsole().log("DEBUG", npc.getName() + " has Q-Learning enabled.");
        } else {
            getDebugConsole().log("DEBUG", npc.getName() + " uses simple behavior.");
        }

        npcs.emplace_back(std::move(npc));
    }
    return npcs;
}


void Game::render() {
    for (const auto& row : tileMap) {
        for (const auto& tile : row) {
            tile->draw(window);
        }
    }

    for (const auto& NPCEntity : npcs) {
        NPCEntity.draw(window);
    }

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

#include <fstream>
#include "nlohmann/json.hpp"

void Game::resetSimulation() {
    static int iterationCounter = 0;
    iterationCounter++;

    getDebugConsole().log("SYSTEM", "Simulation resetting... Iteration " + std::to_string(iterationCounter));

    // Save previous stats before reset
    nlohmann::json statsJson;
    statsJson["iteration"] = iterationCounter;
    statsJson["total_npcs"] = GameConfig::NPCEntityCount;
    statsJson["total_resources"] = aggregateResources(npcs);

    std::ofstream file("stats.json");
    file << statsJson.dump(4);
    file.close();

    // Clear map safely
    for (auto& row : tileMap) {
        for (auto& tile : row) {
            tile.reset();
        }
    }
    tileMap.clear();
    tileMap.shrink_to_fit();

    // Clear NPCs safely
    npcs.clear();
    npcs.shrink_to_fit();

    // Re-generate everything
    generateMap();
    npcs = generateNPCEntitys();
    ui.updateNPCEntityList(npcs);

    // Reset parameters
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