#include "Game.hpp"
#include "FastNoiseLite.h"

Game::Game() : window(sf::VideoMode(1024, 768), "MicroSociety") {
    generateMap();
}

void Game::run() {
    sf::Clock clock;  // Create a clock for delta time
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        // Calculate deltaTime
        sf::Time dt = clock.restart();
        deltaTime = dt.asSeconds();

        window.clear();
        render();
        window.display();
    }
}

void Game::generateMap() {
    // Initialize FastNoiseLite
    FastNoiseLite noise;
    noise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
    noise.SetFrequency(0.1f);  // Adjust for scaling of noise
    noise.SetSeed(static_cast<int>(time(nullptr))); // Random map every time
    // Load textures for grass, stone, and flower tiles
    std::vector<sf::Texture> grassTextures(3), stoneTextures(3), flowerTextures(5);
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

    int rows = 64, cols = 64;
    tileMap.resize(rows);

    for (int i = 0; i < rows; ++i) {
        tileMap[i].resize(cols);
        for (int j = 0; j < cols; ++j) {
            float noiseValue = noise.GetNoise((float)i, (float)j);  // Get Perlin Noise value
            noiseValue = (noiseValue + 1) / 2;  // Normalize value between 0 and 1

            // Determine terrain based on noise thresholds
            if (noiseValue < 0.18f) { 
                tileMap[i][j] = std::make_unique<FlowerTile>(flowerTextures[rand() % 5]);
            }else if (noiseValue < 0.6f)  // Mostly grass
                tileMap[i][j] = std::make_unique<GrassTile>(grassTextures[rand() % 3]);
            else 
                tileMap[i][j] = std::make_unique<StoneTile>(stoneTextures[rand() % 3]);
                

            tileMap[i][j]->setPosition(j * 32, i * 32);
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
