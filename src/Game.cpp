#include "Game.hpp"
#include "FastNoiseLite.h"
#include "debug.hpp"
#include <random>
#include <set>
#include <unordered_map>
#include <algorithm>

// Constructor
Game::Game()
    : window(sf::VideoMode(GameConfig::windowWidth, GameConfig::windowHeight), "MicroSociety", sf::Style::Titlebar | sf::Style::Close),
      clockGUI(700, 100), debugConsole(800.0f, 600.0f) {
#ifdef _WIN32
    ui.adjustLayout(window);
#endif
    playerTexture.loadFromFile("../assets/npc/person1.png"); // Adjust path as needed
    if (!playerTexture.getSize().x) {
        std::cerr << "Failed to load player texture!" << std::endl;
    }

    generateMap();
    npcs = generateNPCEntitys(); 

    // Initialize market prices
    market.setPrice("wood", 10.0f);
    market.setPrice("stone", 10.0f);
    market.setPrice("bush", 5.0f);

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
        NPCEntity.performAction(std::make_unique<RegenerateEnergyAction>(), targetTile);
    }
    return false;
}

void Game::run() {
    sf::Clock clock;
    
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed || sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) window.close();
            if (event.type == sf::Event::Resized) ui.adjustLayout(window);

            ui.handleButtonClicks(window, event, npcs, timeManager, market);
            ui.handleNPCEntityPanel(window, event, npcs);
            ui.handleStatsPanel(window, event);
            // ui.handleMarketButton(window, event, market);
            ui.handleOptionsEvents(window, event, *this);
        }

        sf::Time dt = clock.restart();
        deltaTime = dt.asSeconds();
        // Aggregate resources from all NPCs
        std::unordered_map<std::string, int> allResources = aggregateResources(npcs);
        // debugConsole.logThrottled("Energy", "Energy: " + std::to_string(player.getEnergy()), 500);

        ui.updateMoney(MoneyManager::calculateTotalMoney(npcs));
        // Update day and iteration logic
        timeManager.update(deltaTime);
        clockGUI.update(timeManager.getElapsedTime());

        ui.updateStatus(
            timeManager.getCurrentDay(),
            timeManager.getFormattedTime(),
            timeManager.getSocietyIteration()
        );

        window.clear();
        render();          // Render the map

        // player.draw(window);   // Draw player's entity
        // market.renderPriceGraph(window, "wood", {50, 50}, {200, 100});
        clockGUI.render(window, isClockVisible);
        ui.render(window, market, npcs);
        debugConsole.render(window);
        window.display();
    }
}

void Game::simulateNPCEntityBehavior(float deltaTime) {
    for (auto& NPCEntity : npcs) {
        evaluateNPCEntityState(NPCEntity);

        int x = static_cast<int>(NPCEntity.getPosition().x / GameConfig::tileSize);
        int y = static_cast<int>(NPCEntity.getPosition().y / GameConfig::tileSize);

        if (x >= 0 && x < tileMap[0].size() && y >= 0 && y < tileMap.size()) {
            Tile& targetTile = *tileMap[y][x];

            // Let NPC decide the next action dynamically
            ActionType actionType = NPCEntity.decideNextAction(tileMap);

            // Debug log to verify actions
            getDebugConsole().log("Behavior", NPCEntity.getName() + " chose action: " + std::to_string(static_cast<int>(actionType)));

            // Perform the chosen action
            switch (actionType) {
                case ActionType::ChopTree:
                case ActionType::MineRock:
                case ActionType::GatherBush:
                    moveToResource(NPCEntity, actionType);  // Move toward the resource first
                    break;
                case ActionType::RegenerateEnergy:
                    NPCEntity.performAction(std::make_unique<RegenerateEnergyAction>(), targetTile);
                    break;
                case ActionType::StoreItem:
                    storeItems(NPCEntity, targetTile);
                    break;
                default:
                    performPathfinding(NPCEntity); // Default fallback movement
                    break;
            }
        }

        NPCEntity.update(deltaTime);
    }
}

void Game::moveToResource(NPCEntity& npc, ActionType actionType) {
    int currentX = static_cast<int>(npc.getPosition().x / GameConfig::tileSize);
    int currentY = static_cast<int>(npc.getPosition().y / GameConfig::tileSize);

    int targetX = -1, targetY = -1;
    ObjectType targetType;

    // Determine target object type based on action
    if (actionType == ActionType::ChopTree) targetType = ObjectType::Tree;
    else if (actionType == ActionType::MineRock) targetType = ObjectType::Rock;
    else if (actionType == ActionType::GatherBush) targetType = ObjectType::Bush;
    else return;  // No valid target

    // Search for the nearest target object
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

    // If a valid target is found, move toward it
    if (targetX != -1 && targetY != -1) {
        int moveX = (targetX > currentX) ? 1 : (targetX < currentX) ? -1 : 0;
        int moveY = (targetY > currentY) ? 1 : (targetY < currentY) ? -1 : 0;

        sf::Vector2f newPosition((currentX + moveX) * GameConfig::tileSize, (currentY + moveY) * GameConfig::tileSize);
        npc.setPosition(newPosition.x, newPosition.y);
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
        npc.performAction(std::make_unique<StoreItemAction>(mostAbundantResource, maxQuantity), tile);
    }
}



    //         int targetTileX = static_cast<int>(std::round(player.getPosition().x / GameConfig::tileSize));
    //     int targetTileY = static_cast<int>(std::round(player.getPosition().y / GameConfig::tileSize));

    //     // Declare a raw pointer to the tile for use throughout the logic
    //     Tile* targetTile = nullptr;

    //     if (targetTileX >= 0 && targetTileX < tileMap[0].size() &&
    //         targetTileY >= 0 && targetTileY < tileMap.size()) {
            
    //         targetTile = tileMap[targetTileY][targetTileX].get();
    // if (player.getEnergy() > 0) {
    //         float energyFactor = player.getEnergyPercentage();

    //         if (targetTile) {
    //             if (auto stoneTile = dynamic_cast<StoneTile*>(targetTile)) {
    //                 player.setSpeed(player.getBaseSpeed() * 0.2f * energyFactor); // Slower on stone tiles
    //             } else {
    //                 player.setSpeed(player.getBaseSpeed() * energyFactor); // Normal speed elsewhere
    //             }
    //         } else {
    //             player.setSpeed(player.getBaseSpeed() * energyFactor); // Fallback if no tile detected
    //         }

    //         player.consumeEnergy(deltaTime * 0.5f); // Energy drains over time
    //     } else {
    //         player.setSpeed(0.0f); // Prevent movement if out of energy
    //         // No additional regeneration logic here; it's handled by H action.
    //     }
// }

void Game::evaluateNPCEntityState(NPCEntity& NPCEntity) {
    if (NPCEntity.getEnergy() <= 0.0f) {
        getDebugConsole().log("NPCEntity", NPCEntity.getName() + " ran out of energy and died.");
        // Handle NPCEntity death (remove or reset state)
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

void Game::performPathfinding(NPCEntity& NPCEntity) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<> dist(-1, 1);

    int moveX = dist(gen);
    int moveY = dist(gen);

    int currentX = static_cast<int>(NPCEntity.getPosition().x / GameConfig::tileSize);
    int currentY = static_cast<int>(NPCEntity.getPosition().y / GameConfig::tileSize);

    int newX = currentX + moveX;
    int newY = currentY + moveY;

    // Ensure new position is within map bounds and not blocked
    if (newX >= 0 && newX < GameConfig::mapWidth &&
        newY >= 0 && newY < GameConfig::mapHeight &&
        !tileMap[newY][newX]->hasObject()) {

        sf::Vector2f newPosition(newX * GameConfig::tileSize, newY * GameConfig::tileSize);
        NPCEntity.setPosition(newPosition.x, newPosition.y);
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

            if (noiseValue < 0.2f) {
                tileMap[i][j] = std::make_unique<FlowerTile>(*flowerTextures[rand() % flowerTextures.size()]);
            } else if (noiseValue < 0.6f) {
                tileMap[i][j] = std::make_unique<GrassTile>(*grassTextures[rand() % grassTextures.size()]);
            } else {
                tileMap[i][j] = std::make_unique<StoneTile>(*stoneTextures[rand() % stoneTextures.size()]);
            }

            tileMap[i][j]->setPosition(j * GameConfig::tileSize, i * GameConfig::tileSize);

            int objectChance = rand() % 100;
            if (auto grassTile = dynamic_cast<GrassTile*>(tileMap[i][j].get())) {
                if (objectChance < 20) {
                    grassTile->placeObject(std::make_unique<Tree>(*treeTextures[rand() % treeTextures.size()]));
                } else if (objectChance < 30) {
                    grassTile->placeObject(std::make_unique<Bush>(*bushTextures[rand() % bushTextures.size()]));
                }
            } else if (auto stoneTile = dynamic_cast<StoneTile*>(tileMap[i][j].get())) {
                if (objectChance < 20) {
                    stoneTile->placeObject(std::make_unique<Rock>(*stoneTextures[rand() % stoneTextures.size()]));
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
        NPCEntity NPCEntity("NPCEntity" + std::to_string(i + 1), statDist(gen), statDist(gen), statDist(gen), 150.0f, 10, statDist(gen));
        NPCEntity.setTexture(playerTexture, NPCEntityColor);
        NPCEntity.setPosition(x * GameConfig::tileSize, y * GameConfig::tileSize);

        npcs.push_back(NPCEntity);
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

void Game::resetSimulation() {
    generateMap();
    npcs = generateNPCEntitys();
    ui.updateNPCEntityList(npcs);
    getDebugConsole().log("Options", "Simulation reset.");
}

void Game::toggleTileBorders() {
    showTileBorders = !showTileBorders;
    getDebugConsole().log("Options", "Tile borders toggled: " + std::string(showTileBorders ? "ON" : "OFF"));
}

void Game::setSimulationSpeed(float speedFactor) {
    simulationSpeed = speedFactor;
    getDebugConsole().log("Options", "Simulation speed set to: " + std::to_string(speedFactor));
}

// void Game::saveGame(const std::string& saveFile);
// void Game::loadGame(const std::string& saveFile);


// void Game::setAIDifficulty(int level);

// void Game::applySeason(const std::string& season);
