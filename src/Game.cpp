#include "Game.hpp"
#include "FastNoiseLite.h"
#include "Player.hpp"
#include "debug.hpp"
#include "UI.hpp"
#include <random>
#include <set>
#include <unordered_map>
#include "Market.hpp"

Game::Game() 
    : window(sf::VideoMode(GameConfig::windowWidth, GameConfig::windowHeight), "MicroSociety"), clockGUI(700, 100) {
    generateMap();
    npcs = generateNPCs();

    // Initialize Market
    market.setPrice("wood", 10.0f);
    market.setPrice("stone", 15.0f);

    // Extract NPC names for UI
    std::vector<std::string> npcNames;
    for (const auto& npc : npcs) {
        npcNames.push_back(npc.getName());
    }
    ui.updateNPCList(npcs); // Update UI with NPC names
}



const std::vector<std::vector<std::unique_ptr<Tile>>>& Game::getTileMap() const {
    return tileMap;
}

bool Game::detectCollision(const PlayerEntity& npc) {
    int tileX = static_cast<int>(npc.getPosition().x / GameConfig::tileSize);
    int tileY = static_cast<int>(npc.getPosition().y / GameConfig::tileSize);

    if (tileX >= 0 && tileX < tileMap[0].size() && tileY >= 0 && tileY < tileMap.size()) {
        auto& tile = tileMap[tileY][tileX];
        if (tile->hasObject() && npc.getSprite().getGlobalBounds().intersects(tile->getObjectBounds())) {
            return true;
        }
    }
    return false;
}



void Game::run() {
    sf::Clock clock;
    PlayerEntity player("Player", 100, 50, 50, 150.0f, 10, 10000);
    // player.setSize(1.5f, 1.5f);
    
    // Set up the debug console
    DebugConsole& debugConsole = getDebugConsole();
    debugConsole.enable(); 
    std::set<std::pair<int, int>> shownMessages;

    sf::Image icon;
    if (icon.loadFromFile("../assets/icon/favicon.png")) {
        window.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());
    } else {
        debugConsole.logOnce("Error", "Failed to load icon!");
    }

    sf::Texture playerTexture;
    if (!playerTexture.loadFromFile("../assets/npc/person1.png")) {
        debugConsole.logOnce("Error", "Failed to load player texture.");
    }

    // Generate random color values
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> colorDist(0, 255);

    sf::Color randomColor(colorDist(gen), colorDist(gen), colorDist(gen));

    // Set texture and color together
    player.setTexture(playerTexture, randomColor);
    player.setPosition(GameConfig::mapWidth / 2 * GameConfig::tileSize,
                       GameConfig::mapHeight / 2 * GameConfig::tileSize);

    // Action testing
    std::vector<std::unique_ptr<Action>> actions;
    actions.emplace_back(std::make_unique<MoveAction>());
    actions.emplace_back(std::make_unique<TradeAction>());

    std::set<std::pair<int, int>> loggedTiles;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) window.close();
            if (event.type == sf::Event::Resized) ui.adjustLayout(window);
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) window.close();
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::F1) {
                debugConsole.toggle();
            }

            ui.handleButtonClicks(window, event, npcs, timeManager);
            ui.handleNPCPanel(window, event, npcs);
            ui.handleStatsPanel(window, event);
            ui.handleMarketButton(window, event, market);
            ui.handleOptionsEvents(window, event, *this);
        }


        sf::Time dt = clock.restart();
        deltaTime = dt.asSeconds();

        sf::Vector2f previousPosition = player.getPosition();
        player.handleInput(deltaTime);




        // Calculate target tile based on the tile size and player position
        int targetTileX = static_cast<int>(std::round(player.getPosition().x / GameConfig::tileSize));
        int targetTileY = static_cast<int>(std::round(player.getPosition().y / GameConfig::tileSize));

        // Declare a raw pointer to the tile for use throughout the logic
        Tile* targetTile = nullptr;

        if (targetTileX >= 0 && targetTileX < tileMap[0].size() &&
            targetTileY >= 0 && targetTileY < tileMap.size()) {
            
            targetTile = tileMap[targetTileY][targetTileX].get();
            
            // Keep track of whether we showed debug info for this tile
            static std::pair<int, int> lastDebugTile = {-1, -1};
            if (std::make_pair(targetTileX, targetTileY) != lastDebugTile) {
                lastDebugTile = {targetTileX, targetTileY};

                if (auto tree = dynamic_cast<Tree*>(targetTile->getObject())) {
                    debugConsole.log("Hint", "Press 'T' to chop tree.");
                } else if (auto stone = dynamic_cast<Rock*>(targetTile->getObject())) {
                    debugConsole.log("Hint", "Press 'M' to mine rock.");
                } else if (auto bush = dynamic_cast<Bush*>(targetTile->getObject())) {
                    debugConsole.log("Hint", "Press 'G' to gather bush resources.");
                } else if (auto house = dynamic_cast<House*>(targetTile->getObject())) {
                    debugConsole.log("Hint", "Press 'H' to regenerate energy, 'U' to upgrade, or 'I' to store items or 'O' to see storage or 'P' for taking wood for now");
                } else if (auto market = dynamic_cast<Market*>(targetTile->getObject())) {
                    debugConsole.log("Hint", "Press '1,2,3' to buy or '4,5,6' to sell items.");
                }
            }

            // Execute actions based on input (no continuous execution)
            static bool keyProcessed = false;

            if (!keyProcessed) {
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::T)) {
                    if (auto tree = dynamic_cast<Tree*>(targetTile->getObject())) {
                        std::unique_ptr<Action> action = std::make_unique<TreeAction>();
                        action->perform(player, *targetTile);
                        debugConsole.logOnce("Action", "Chopped tree.");
                        keyProcessed = true;
                    }
                } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::M)) {
                    if (auto stone = dynamic_cast<Rock*>(targetTile->getObject())) {
                        std::unique_ptr<Action> action = std::make_unique<StoneAction>();
                        action->perform(player, *targetTile);
                        debugConsole.logOnce("Action", "Mined rock.");
                        keyProcessed = true;
                    }
                } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::G)) {
                    if (auto bush = dynamic_cast<Bush*>(targetTile->getObject())) {
                        std::unique_ptr<Action> action = std::make_unique<BushAction>();
                        action->perform(player, *targetTile);
                        debugConsole.logOnce("Action", "Gathered bush resources.");
                        keyProcessed = true;
                    }
                } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::H)) {
                    if (auto house = dynamic_cast<House*>(targetTile->getObject())) {
                        std::unique_ptr<Action> action = std::make_unique<RegenerateEnergyAction>();
                        action->perform(player, *targetTile);
                        debugConsole.logOnce("Action", "Regenerated energy.");
                    }
                } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::U)) {
                    if (auto house = dynamic_cast<House*>(targetTile->getObject())) {
                        float playerMoney = player.getMoney(); // Copy the money
                        if (house->upgrade(playerMoney, player)) { // Pass the player to apply bonuses
                            player.setMoney(playerMoney); // Update player's money after upgrade
                            debugConsole.log("Action", "House upgraded successfully!");
                        } else {
                            debugConsole.log("Action", "Not enough money to upgrade the house.");
                        }
                        keyProcessed = true;
                    }
                } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::I)) {
                    if (auto house = dynamic_cast<House*>(targetTile->getObject())) {
                        const auto& inventory = player.getInventory();
                        if (!inventory.empty()) {
                            auto it = inventory.begin();
                            std::unique_ptr<Action> action = std::make_unique<StoreItemAction>(it->first, it->second);
                            action->perform(player, *targetTile);
                            debugConsole.logOnce("Action", "Stored items.");
                        } else {
                            debugConsole.logOnce("Action", "Inventory is empty.");
                        }
                    }
                } else if (auto market = dynamic_cast<Market*>(targetTile->getObject())) {
                    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num1)) {
                        market->buyItem(player, "wood", 1);
                        debugConsole.log("Market", "Bought 1 wood.");
                        keyProcessed = true;
                    } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num2)) {
                        market->buyItem(player, "stone", 1);
                        debugConsole.log("Market", "Bought 1 stone.");
                        keyProcessed = true;
                    } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num3)) {
                        market->buyItem(player, "bushes", 1);
                        debugConsole.log("Market", "Bought 1 bush.");
                        keyProcessed = true;
                    } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num4)) {
                        market->sellItem(player, "wood", 1);
                        debugConsole.log("Market", "Sold 1 wood.");
                        keyProcessed = true;
                    } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num5)) {
                        market->sellItem(player, "stone", 1);
                        debugConsole.log("Market", "Sold 1 stone.");
                        keyProcessed = true;
                    } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num6)) {
                        market->sellItem(player, "bushes", 1);
                        debugConsole.log("Market", "Sold 1 bush.");
                        keyProcessed = true;
                    }
                } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::O)) {
                    if (auto house = dynamic_cast<House*>(targetTile->getObject())) {
                        house->displayStorage(); // Log storage contents to the debug console
                        debugConsole.logOnce("Action", "Displayed house storage.");
                        keyProcessed = true;
                    }
                } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::P)) {
                    if (auto house = dynamic_cast<House*>(targetTile->getObject())) {
                        if (!house->takeFromStorage("wood", 1, player)) {
                            debugConsole.log("Action", "Failed to take items from house storage.");
                        } else {
                            debugConsole.log("Action", "Took 1 wood from house storage.");
                        }
                        keyProcessed = true;
                    }
                }
            }

            // Reset the keyProcessed flag when no key is pressed
            if (!sf::Keyboard::isKeyPressed(sf::Keyboard::T) &&
                !sf::Keyboard::isKeyPressed(sf::Keyboard::M) &&
                !sf::Keyboard::isKeyPressed(sf::Keyboard::G) &&
                !sf::Keyboard::isKeyPressed(sf::Keyboard::H) &&
                !sf::Keyboard::isKeyPressed(sf::Keyboard::U) &&
                !sf::Keyboard::isKeyPressed(sf::Keyboard::I) &&
                !sf::Keyboard::isKeyPressed(sf::Keyboard::Num1) &&
                !sf::Keyboard::isKeyPressed(sf::Keyboard::Num2) &&
                !sf::Keyboard::isKeyPressed(sf::Keyboard::Num3) &&
                !sf::Keyboard::isKeyPressed(sf::Keyboard::Num4) &&
                !sf::Keyboard::isKeyPressed(sf::Keyboard::Num5) &&
                !sf::Keyboard::isKeyPressed(sf::Keyboard::Num6) &&
                !sf::Keyboard::isKeyPressed(sf::Keyboard::O) &&
                !sf::Keyboard::isKeyPressed(sf::Keyboard::P)) {
                keyProcessed = false;
            }
            }

        // Slow down on StoneTile
        if (player.getEnergy() > 0) {
            float energyFactor = player.getEnergyPercentage();

            if (targetTile) {
                if (auto stoneTile = dynamic_cast<StoneTile*>(targetTile)) {
                    player.setSpeed(player.getBaseSpeed() * 0.2f * energyFactor); // Slower on stone tiles
                } else {
                    player.setSpeed(player.getBaseSpeed() * energyFactor); // Normal speed elsewhere
                }
            } else {
                player.setSpeed(player.getBaseSpeed() * energyFactor); // Fallback if no tile detected
            }

            player.consumeEnergy(deltaTime * 0.5f); // Energy drains over time
        } else {
            player.setSpeed(0.0f); // Prevent movement if out of energy
            // No additional regeneration logic here; it's handled by H action.
        }

 
        // Collision detection
        if (targetTile && targetTile->hasObject()) {
            if (player.getSprite().getGlobalBounds().intersects(targetTile->getObjectBounds())) {
                player.setPosition(previousPosition.x, previousPosition.y);
                debugConsole.logThrottled("Collision", "Collision detected at tile.", 1000); 
            }
        }


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
        // debugPlayerInfo(player);
        window.clear();
        render();          // Render the map

        player.draw(window);   // Draw player's entity
        // market.renderPriceGraph(window, "wood", {50, 50}, {200, 100});
        clockGUI.render(window);
        ui.render(window, market);
        debugConsole.render(window);
        window.display();
    }
}
/* MIGHT DELETE LATER*/
void Game::drawTileBorders() {
    for (int i = 0; i < tileMap.size(); ++i) {
        for (int j = 0; j < tileMap[i].size(); ++j) {
                sf::RectangleShape border(sf::Vector2f(GameConfig::tileSize, GameConfig::tileSize));
                border.setPosition(j * GameConfig::tileSize, i * GameConfig::tileSize);
                border.setOutlineThickness(1);
                border.setOutlineColor(sf::Color::Black);
                border.setFillColor(sf::Color::Transparent); // Make the border itself transparent
                window.draw(border);
        }
    }
}

std::unordered_map<std::string, int> Game::aggregateResources(const std::vector<PlayerEntity>& npcs) const {
    std::unordered_map<std::string, int> allResources;

    for (const auto& npc : npcs) {
        const auto& inventory = npc.getInventory();
        for (const auto& [item, quantity] : inventory) {
            allResources[item] += quantity;
        }
    }

    return allResources;
}

std::vector<PlayerEntity> Game::generateNPCs() const {
    std::vector<PlayerEntity> npcs;
    std::set<std::pair<int, int>> occupiedPositions; // Keep track of occupied tiles

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distX(0, tileMap[0].size() - 1);
    std::uniform_int_distribution<> distY(0, tileMap.size() - 1);

    for (int i = 0; i < 1; ++i) { // Number of NPCs
        int x, y;
        do {
            x = distX(gen);
            y = distY(gen);
        } while (occupiedPositions.count({x, y}) > 0 || tileMap[y][x]->hasObject()); // Ensure unique position

        occupiedPositions.insert({x, y});

        PlayerEntity npc("NPC" + std::to_string(i + 1), 100, 50, 50, 150.0f, 10, 100);

        // Populate inventory with random items
        npc.addToInventory("wood", rand() % 10 + 1);
        npc.addToInventory("stone", rand() % 10 + 1);
        npc.addToInventory("bushes", rand() % 5 + 1);

        npc.setPosition(x * GameConfig::tileSize, y * GameConfig::tileSize);
        npcs.push_back(npc);
    }


    return npcs;
}


void Game::generateMap() {
    // initialize FastNoiseLite for Perlin Noise
    FastNoiseLite noise;
    noise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
    noise.SetFrequency(0.1f);  // adjust for scaling of noise
    noise.SetSeed(static_cast<int>(time(nullptr))); // random map every time
    // loading textures
    std::vector<sf::Texture> grassTextures(3), stoneTextures(3), flowerTextures(5); // tiles textures
    std::vector<sf::Texture> bushTextures(2), rockTextures(3), treeTextures(3); // object textures
    std::vector<sf::Texture> houseTextures(3), marketTextures(3);

    for (int i = 0; i < 3; ++i) {
        grassTextures[i].loadFromFile("../assets/tiles/grass/grass" + std::to_string(i + 1) + ".png");
        stoneTextures[i].loadFromFile("../assets/tiles/stone/stone" + std::to_string(i + 1) + ".png");
        marketTextures[i].loadFromFile("../assets/objects/market" + std::to_string(i + 1) + ".png");
        treeTextures[i].loadFromFile("../assets/objects/tree" + std::to_string(i + 1) + ".png");
        rockTextures[i].loadFromFile("../assets/objects/rock" + std::to_string(i + 1) + ".png");
        houseTextures[i].loadFromFile("../assets/objects/house" + std::to_string(i + 1) + ".png");
    }
    for (int i = 0; i < 2; ++i) {
        bushTextures[i].loadFromFile("../assets/objects/bush" + std::to_string(i + 1) + ".png");
    }
    for (int i = 0; i < 5; ++i) {
        flowerTextures[i].loadFromFile("../assets/tiles/flower/flower" + std::to_string(i + 1) + ".png");
    }

    tileMap.resize(GameConfig::mapHeight);
    for (auto& row : tileMap) {
        row.resize(GameConfig::mapWidth);
    }

    for (int i = 0; i < GameConfig::mapHeight; ++i) {
        for (int j = 0; j < GameConfig::mapWidth; ++j) {
            float noiseValue = noise.GetNoise(static_cast<float>(i), static_cast<float>(j)); // get Perlin Noise value
            noiseValue = (noiseValue + 1) / 2;  // normalize value between 0 and 1

            // determine terrain based on noise thresholds
            if (noiseValue < 0.18f) { 
                tileMap[i][j] = std::make_unique<FlowerTile>(flowerTextures[rand() % 5]);
            } else if (noiseValue < 0.6f)  
                tileMap[i][j] = std::make_unique<GrassTile>(grassTextures[rand() % 3]);
            else 
                tileMap[i][j] = std::make_unique<StoneTile>(stoneTextures[rand() % 3]);
                
            
            if (!tileMap[i][j]) { // Debugging output
                std::cerr << "Error: TileMap[" << i << "][" << j << "] not initialized!" << std::endl;
                throw std::runtime_error("TileMap initialization failed.");
            }

            tileMap[i][j]->setPosition(j * GameConfig::tileSize, i * GameConfig::tileSize);

            int objChance = rand() % 100; // object chances for creation
            if (auto grassTile = dynamic_cast<GrassTile*>(tileMap[i][j].get())) {
                if (objChance < 20) {
                    grassTile->placeObject(std::make_unique<Tree>(treeTextures[rand() % 3]));
                } else if (objChance < 30) {
                    grassTile->placeObject(std::make_unique<Bush>(bushTextures[rand() % 2]));
                }
            } else if (auto stoneTile = dynamic_cast<StoneTile*>(tileMap[i][j].get())) {
                if (objChance < 20) {
                    stoneTile->placeObject(std::make_unique<Rock>(rockTextures[rand() % 3]));
                }
            }
        }
    }

    if (tileMap.size() != GameConfig::mapHeight) {
        std::cerr << "Error: tileMap height mismatch" << std::endl;
    }
    if (tileMap[0].size() != GameConfig::mapWidth) {
        std::cerr << "Error: tileMap width mismatch" << std::endl;
    }
    std::cout << "tileMap Height: " << tileMap.size() << std::endl;
    if (!tileMap.empty()) {
        std::cout << "tileMap Width: " << tileMap[0].size() << std::endl;
    }



    // Place unique houses for each NPC
    std::set<std::pair<int, int>> occupiedPositions;
    for (int i = 0; i < 1; ++i) { // Assuming 5 NPCs
        int houseX, houseY;
        do {
            houseX = rand() % GameConfig::mapWidth;
            houseY = rand() % GameConfig::mapHeight;
        } while (occupiedPositions.count({houseX, houseY}) || tileMap[houseY][houseX]->hasObject());

        occupiedPositions.insert({houseX, houseY});

        sf::Color houseColor(rand() % 256, rand() % 256, rand() % 256); // Random house color
        auto house = std::make_unique<House>(houseTextures[i % 3]);
        house->getSprite().setColor(houseColor);

        tileMap[houseY][houseX]->placeObject(std::move(house));

        // Debugging
        getDebugConsole().logOnce("MapGen", "House for NPC " + std::to_string(i + 1) +
                                            " placed at (" + std::to_string(houseX) + ", " + std::to_string(houseY) + ")");
    }

    // Place markets with conflict resolution
    int marketCount = 2 + rand() % 2; // 2 or 3 markets
    for (int m = 0; m < marketCount; ++m) {
        int marketX, marketY;
        do {
            marketX = rand() % GameConfig::mapWidth;
            marketY = rand() % GameConfig::mapHeight;
        } while (occupiedPositions.count({marketX, marketY}) || tileMap[marketY][marketX]->hasObject());

        occupiedPositions.insert({marketX, marketY});
        tileMap[marketY][marketX]->placeObject(std::make_unique<Market>(marketTextures[m % 3]));

        getDebugConsole().logOnce("MapGen", "Market placed at (" + std::to_string(marketX) + ", " + std::to_string(marketY) + ")");
    }
}

void Game::render() {
    for (const auto& row : tileMap) {
        for (const auto& tile : row) {
            tile->draw(window);
        }
    }
}

void Game::resetSimulation() {
    generateMap();
    npcs = generateNPCs();
    ui.updateNPCList(npcs);
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
