#include "Game.hpp"
#include "FastNoiseLite.h"
#include "Player.hpp"
#include "debug.hpp"
#include <random>
#include <set>


Game::Game() : window(sf::VideoMode(GameConfig::mapWidth, GameConfig::mapHeight), "MicroSociety") {
    generateMap();
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
    // player.setSize(1.5f, 1.5f);
    
    sf::Image icon;
    if (icon.loadFromFile("../assets/icon/favicon.png")) {
        window.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());
    } else {
        std::cerr << "Error loading icon!\n";
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

    // Initial debug info
    // debugMapInfo(*this);
    // debugObjectBoundaries(*this);

    // Action testing
    std::vector<std::unique_ptr<Action>> actions;
    actions.emplace_back(std::make_unique<MoveAction>());
    actions.emplace_back(std::make_unique<TradeAction>());

    std::set<std::pair<int, int>> loggedTiles;
    
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) {
                exit(1);
            }
        }

        sf::Time dt = clock.restart();
        deltaTime = dt.asSeconds();

        sf::Vector2f previousPosition = player.getPosition();
        player.handleInput(deltaTime);

        // Calculate target tile based on the tile size and player position
        int targetTileX = static_cast<int>(std::round(player.getPosition().x / GameConfig::tileSize));
        int targetTileY = static_cast<int>(std::round(player.getPosition().y / GameConfig::tileSize));


        if (targetTileX >= 0 && targetTileX < tileMap[0].size() &&
            targetTileY >= 0 && targetTileY < tileMap.size()) {

            auto& targetTile = tileMap[targetTileY][targetTileX];

            // Actions
            ActionType actionType = ActionType::None;

            if (auto tree = dynamic_cast<Tree*>(targetTile->getObject())) {
                actionType = ActionType::ChopTree;
                std::cout << "Press 'T' to chop tree.\n";
            } else if (auto stone = dynamic_cast<Rock*>(targetTile->getObject())) {
                actionType = ActionType::MineRock;
                std::cout << "Press 'M' to mine rock.\n";
            } else if (auto bush = dynamic_cast<Bush*>(targetTile->getObject())) {
                actionType = ActionType::GatherBush;
                std::cout << "Press 'G' to gather bush resources.\n";
            }



            // Check for user input to perform action
            if (actionType != ActionType::None) {
                if ((actionType == ActionType::ChopTree && sf::Keyboard::isKeyPressed(sf::Keyboard::T)) ||
                    (actionType == ActionType::MineRock && sf::Keyboard::isKeyPressed(sf::Keyboard::M)) ||
                    (actionType == ActionType::GatherBush && sf::Keyboard::isKeyPressed(sf::Keyboard::G))) {
                    
                    std::unique_ptr<Action> action;
                    if (actionType == ActionType::ChopTree) {
                        action = std::make_unique<TreeAction>();
                    } else if (actionType == ActionType::MineRock) {
                        action = std::make_unique<StoneAction>();
                    } else if (actionType == ActionType::GatherBush) {
                        action = std::make_unique<BushAction>();
                    }

                    // Perform the action on the player and the tile
                    if (action) {
                        action->perform(player, *targetTile); // Call action with tile reference
                        std::cout << "Performed action: " << action->getActionName() << "\n";
                        player.displayInventory();
                    }
                }
            }

            // Slow down on StoneTile
            if (auto stoneTile = dynamic_cast<StoneTile*>(targetTile.get())) {
                player.setSpeed(75.0f);  // Slow speed on stone tiles
            } else {
                player.setSpeed(150.0f); // Normal speed on other tiles
            }

            // Collision detection
            if (targetTile->hasObject()) {
                auto objectBounds = targetTile->getObjectBounds();
                if (player.getSprite().getGlobalBounds().intersects(objectBounds)) {
                    player.setPosition(previousPosition.x, previousPosition.y);  // Revert position on collision
                    std::cout << "Collision at tile (" << targetTileX << ", " << targetTileY << ")\n";
                }
            }
        }

        // Debug each step
        bool debugMode = true; // Add this at the top or make it a class member

        if (debugMode) {
            // debugPlayerInfo(player);
            debugTileInfo(targetTileX, targetTileY, *this);
            drawTileBorders();
        }

        window.clear();
        render();          // Render the map
        player.draw(window);   // Draw player's entity
        window.display();
    }
}

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


void Game::generateMap() {
    // initialize FastNoiseLite for Perlin Noise
    FastNoiseLite noise;
    noise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
    noise.SetFrequency(0.1f);  // adjust for scaling of noise
    noise.SetSeed(static_cast<int>(time(nullptr))); // random map every time
    // loading textures
    std::vector<sf::Texture> grassTextures(3), stoneTextures(3), flowerTextures(5); // tiles textures
    std::vector<sf::Texture> bushTextures(2), rockTextures(3), treeTextures(5); // object textures

    grassTextures[0].loadFromFile("../assets/tiles/grass/grass1.png");
    grassTextures[1].loadFromFile("../assets/tiles/grass/grass2.png");
    grassTextures[2].loadFromFile("../assets/tiles/grass/grass3.png");

    stoneTextures[0].loadFromFile("../assets/tiles/stone/stone1.png");
    stoneTextures[1].loadFromFile("../assets/tiles/stone/stone2.png");
    stoneTextures[2].loadFromFile("../assets/tiles/stone/stone3.png");

    flowerTextures[0].loadFromFile("../assets/tiles/flower/flower1.png");
    flowerTextures[1].loadFromFile("../assets/tiles/flower/flower2.png");
    flowerTextures[2].loadFromFile("../assets/tiles/flower/flower3.png");
    flowerTextures[3].loadFromFile("../assets/tiles/flower/flower4.png");
    flowerTextures[4].loadFromFile("../assets/tiles/flower/flower5.png");

    treeTextures[0].loadFromFile("../assets/objects/tree1.png");
    treeTextures[1].loadFromFile("../assets/objects/tree2.png");
    treeTextures[2].loadFromFile("../assets/objects/tree3.png");

    rockTextures[0].loadFromFile("../assets/objects/rock1.png");
    rockTextures[1].loadFromFile("../assets/objects/rock2.png");
    rockTextures[2].loadFromFile("../assets/objects/rock3.png");

    bushTextures[0].loadFromFile("../assets/objects/bush1.png");
    bushTextures[1].loadFromFile("../assets/objects/bush2.png");

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
}

void Game::render() {
    for (const auto& row : tileMap) {
        for (const auto& tile : row) {
            tile->draw(window);
        }
    }
}

