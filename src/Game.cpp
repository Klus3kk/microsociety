#include "Game.hpp"
#include "FastNoiseLite.h"
#include "Player.hpp"
#include <random>
Game::Game() : window(sf::VideoMode(mapWidth, mapHeight), "MicroSociety") { // overloading
    generateMap();
}

void Game::run() {
    sf::Clock clock;
    PlayerEntity player(100, 50, 50, 150.0f, 10, 100);
    player.setSize(2.0f, 2.0f);
    
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

    player.setPosition(mapWidth / 2, mapHeight / 2);

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
        player.handleInput(deltaTime);  // player movement input (it will be changed automatically in the future)

        int playerTileX = player.getPosition().x / 32;
        int playerTileY = player.getPosition().y / 32;

        if (tileMap[playerTileY][playerTileX]->hasObject() && 
            player.getSprite().getGlobalBounds().intersects(tileMap[playerTileY][playerTileX]->getObjectBounds())) {
            player.setPosition(previousPosition.x, previousPosition.y); // Revert to previous position if collision
        }


        window.clear();
        render();  // map render
        player.draw(window);   // draw player's entity
        window.display();
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
                

            tileMap[i][j]->setPosition(j * 32, i * 32);

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

