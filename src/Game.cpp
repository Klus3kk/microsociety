#include "Game.hpp"
#include "FastNoiseLite.h"
#include "Player.hpp"
#include "debug.hpp"
#include "UI.hpp"
#include <random>
#include <set>
#include <unordered_map>

Game::Game() : window(sf::VideoMode(GameConfig::mapWidth, GameConfig::mapHeight), "MicroSociety") {
    generateMap();
}

const std::vector<std::vector<std::unique_ptr<Tile>>>& Game::getTileMap() const {
    return tileMap;
}

bool Game::detectCollision(const PlayerEntity& npc) {
    int tileX = static_cast<int>(npc.getPosition().x / GameConfig::tileSize);
    int tileY = static_cast<int>(npc.getPosition().y / GameConfig::tileSize);

    if (tileX >= 0 && tileX < tileMap[0].size() && tileY >= 0 && tileY < tileMap.size()) {
        auto& tile = tileMap[tileY][tileX];
        if (tile->hasObject()) {
            return npc.getSprite().getGlobalBounds().intersects(tile->getObjectBounds());
        }
    }
    return false;
}

void Game::run() {
    sf::Clock clock;
    PlayerEntity player(100, 50, 50, 150.0f, 10, 100);
    player.setSize(1.5f, 1.5f);
    
    // Set up the debug console
    DebugConsole& debugConsole = getDebugConsole();
    debugConsole.enable(); 
    std::set<std::pair<int, int>> shownMessages;

    sf::Image icon;
    if (icon.loadFromFile("../assets/icon/favicon.png")) {
        window.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());
    } else {
        debugConsole.log("Error loading icon!");
    }


    sf::Texture playerTexture;
    if (!playerTexture.loadFromFile("../assets/npc/person1.png")) {
        std::cerr << "Error loading player's texture\n";
    }

    // Generate random color values
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> colorDist(0, 255);

    sf::Color randomColor(colorDist(gen), colorDist(gen), colorDist(gen));

    // Set texture and color together
    player.setTexture(playerTexture, randomColor);
    player.setPosition(GameConfig::mapWidth / 2, GameConfig::mapHeight / 2);

    // Action testing
    std::vector<std::unique_ptr<Action>> actions;
    actions.emplace_back(std::make_unique<MoveAction>());
    actions.emplace_back(std::make_unique<TradeAction>());

    std::set<std::pair<int, int>> loggedTiles;

    // Generate NPCs
    std::vector<PlayerEntity> npcs = generateNPCs();

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) window.close();
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) window.close();
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::F1) {
                debugConsole.toggle();
            }

            ui.handleButtonClicks(window, event);
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

                // Log debug information for the target tile
                if (auto tree = dynamic_cast<Tree*>(targetTile->getObject())) {
                    debugConsole.log("Press 'T' to chop tree.");
                } else if (auto stone = dynamic_cast<Rock*>(targetTile->getObject())) {
                    debugConsole.log("Press 'M' to mine rock.");
                } else if (auto bush = dynamic_cast<Bush*>(targetTile->getObject())) {
                    debugConsole.log("Press 'G' to gather bush resources.");
                } else if (auto house = dynamic_cast<House*>(targetTile->getObject())) {
                    debugConsole.log("Press 'H' to regenerate energy.");
                    debugConsole.log("Press 'U' to upgrade the house.");
                    debugConsole.log("Press 'I' to store items in the house.");
                }
            }

            // Execute actions based on input (no continuous execution)
            static bool keyProcessed = false;

            if (!keyProcessed) {
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::T)) {
                    if (auto tree = dynamic_cast<Tree*>(targetTile->getObject())) {
                        std::unique_ptr<Action> action = std::make_unique<TreeAction>();
                        action->perform(player, *targetTile);
                        debugConsole.log("Performed action: " + action->getActionName());
                        keyProcessed = true;
                    }
                } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::M)) {
                    if (auto stone = dynamic_cast<Rock*>(targetTile->getObject())) {
                        std::unique_ptr<Action> action = std::make_unique<StoneAction>();
                        action->perform(player, *targetTile);
                        debugConsole.log("Performed action: " + action->getActionName());
                        keyProcessed = true;
                    }
                } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::G)) {
                    if (auto bush = dynamic_cast<Bush*>(targetTile->getObject())) {
                        std::unique_ptr<Action> action = std::make_unique<BushAction>();
                        action->perform(player, *targetTile);
                        debugConsole.log("Performed action: " + action->getActionName());
                        keyProcessed = true;
                    }
                } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::H)) {
                    if (auto house = dynamic_cast<House*>(targetTile->getObject())) {
                        std::unique_ptr<Action> action = std::make_unique<RegenerateEnergyAction>();
                        action->perform(player, *targetTile);
                        debugConsole.log("Energy regenerated.");
                    }
                } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::U)) {
                    if (auto house = dynamic_cast<House*>(targetTile->getObject())) {
                        std::unique_ptr<Action> action = std::make_unique<UpgradeHouseAction>();
                        action->perform(player, *targetTile);
                        debugConsole.log("House upgraded.");
                    }
                } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::I)) {
                    if (auto house = dynamic_cast<House*>(targetTile->getObject())) {
                        const auto& inventory = player.getInventory();
                        if (!inventory.empty()) {
                            auto it = inventory.begin();
                            std::unique_ptr<Action> action = std::make_unique<StoreItemAction>(it->first, it->second);
                            action->perform(player, *targetTile);
                            debugConsole.log("Stored items in the house.");
                        } else {
                            debugConsole.log("Inventory is empty.");
                        }
                    }
                }

            }

            // Reset the keyProcessed flag when no key is pressed
            if (!sf::Keyboard::isKeyPressed(sf::Keyboard::T) &&
                !sf::Keyboard::isKeyPressed(sf::Keyboard::M) &&
                !sf::Keyboard::isKeyPressed(sf::Keyboard::G) &&
                !sf::Keyboard::isKeyPressed(sf::Keyboard::H) &&
                !sf::Keyboard::isKeyPressed(sf::Keyboard::U) &&
                !sf::Keyboard::isKeyPressed(sf::Keyboard::I)) {
                keyProcessed = false;
            }
        }

        // Slow down on StoneTile
        if (auto stoneTile = dynamic_cast<StoneTile*>(targetTile)) { // Use targetTile directly, no .get()
            player.setSpeed(75.0f);  // Slow speed on stone tiles
        } else {
            player.setSpeed(150.0f); // Normal speed on other tiles
        }

        // Collision detection
        if (targetTile->hasObject()) {
            auto objectBounds = targetTile->getObjectBounds();
            if (player.getSprite().getGlobalBounds().intersects(objectBounds)) {
                player.setPosition(previousPosition.x, previousPosition.y);  // Revert position on collision
                debugConsole.log("Collision at tile (" + std::to_string(targetTileX) + ", " + std::to_string(targetTileY) + ").");
            }
        }

        // Aggregate resources from all NPCs
        std::unordered_map<std::string, int> allResources = aggregateResources(npcs);

        // Update UI
        ui.updateStatus(1, "12:00", npcs.size(), 1000, allResources);  // Example values for now

        // debugPlayerInfo(player);
        window.clear();
        render();          // Render the map
        ui.render(window); 
        player.draw(window);   // Draw player's entity
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

    for (int i = 0; i < 1; ++i) { // Number of NPCs to generate
        int x, y;
        do {
            x = distX(gen);
            y = distY(gen);
        } while (occupiedPositions.count({x, y}) > 0 || tileMap[y][x]->hasObject()); // Ensure position is unique and not blocked

        occupiedPositions.insert({x, y});

        PlayerEntity npc(100, 50, 50, 150.0f, 10, 100);
        npc.addToInventory("wood", i + 1);
        npc.addToInventory("stone", 2 * i);
        npc.addToInventory("food", 5 - i);

        npc.setPosition(x * GameConfig::tileSize, y * GameConfig::tileSize); // Place NPC on the map
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
        marketTextures[i].loadFromFile("../assets/objects/market" + std::to_string(i + 1) + ".png");
        treeTextures[i].loadFromFile("../assets/objects/tree" + std::to_string(i + 1) + ".png");
        rockTextures[i].loadFromFile("../assets/objects/rock" + std::to_string(i + 1) + ".png");
        houseTextures[i].loadFromFile("../assets/objects/house" + std::to_string(i + 1) + ".png");
    }

    for (int i = 0; i < 3; ++i) {
        grassTextures[i].loadFromFile("../assets/tiles/grass/grass" + std::to_string(i + 1) + ".png");
        stoneTextures[i].loadFromFile("../assets/tiles/stone/stone" + std::to_string(i + 1) + ".png");
    }

    for (int i = 0; i < 2; ++i) {
        bushTextures[i].loadFromFile("../assets/objects/bush" + std::to_string(i + 1) + ".png");
    }

    for (int i = 0; i < 5; ++i) {
        flowerTextures[i].loadFromFile("../assets/tiles/flower/flower" + std::to_string(i + 1) + ".png");
    }

    int rows = 64, cols = 64;
    tileMap.resize(rows);

    for (int i = 0; i < rows; ++i) {
        tileMap[i].resize(cols);
        for (int j = 0; j < cols; ++j) {
            float noiseValue = noise.GetNoise((float)i, (float)j);  // get Perlin Noise value
            noiseValue = (noiseValue + 1) / 2;  // normalize value between 0 and 1

            // determine terrain based on noise thresholds
            if (noiseValue < 0.18f) { 
                tileMap[i][j] = std::make_unique<FlowerTile>(flowerTextures[rand() % 5]);
            }else if (noiseValue < 0.6f)  
                tileMap[i][j] = std::make_unique<GrassTile>(grassTextures[rand() % 3]);
            else 
                tileMap[i][j] = std::make_unique<StoneTile>(stoneTextures[rand() % 3]);
                

            tileMap[i][j]->setPosition(j * GameConfig::tileSize, i * GameConfig::tileSize);

            int objChance = rand() % 100; // object chances for creation
            if (auto grassTile = dynamic_cast<GrassTile*>(tileMap[i][j].get())) {
                if (objChance < 10) {
                    tileMap[i][j]->placeObject(std::make_unique<Tree>(treeTextures[rand() % 3]));  // random tree texture
                } else if (objChance < 15) {
                    tileMap[i][j]->placeObject(std::make_unique<Bush>(bushTextures[rand() % 2]));  // random bush texture
                }
            } else if (auto stoneTile = dynamic_cast<StoneTile*>(tileMap[i][j].get())) {
                if (objChance < 20) {
                    tileMap[i][j]->placeObject(std::make_unique<Rock>(rockTextures[rand() % 3]));  // random rock texture
                }
            }
        }
    }

    int playerHouseX = GameConfig::mapWidth / 2;
    int playerHouseY = GameConfig::mapHeight / 2;
    tileMap[playerHouseY / GameConfig::tileSize][playerHouseX / GameConfig::tileSize]
    ->placeObject(std::make_unique<House>(houseTextures[0]));

    int marketCount = 2 + rand() % 2; // Place 2 or 3 markets
    for (int m = 0; m < marketCount; ++m) {
        int marketX = rand() % cols;
        int marketY = rand() % rows;
        tileMap[marketY][marketX]->placeObject(std::make_unique<Market>(marketTextures[m % 3]));  
    }
}

void Game::render() {
    for (const auto& row : tileMap) {
        for (const auto& tile : row) {
            tile->draw(window);
        }
    }
}



