#include "Game.hpp"

Game::Game() : window(sf::VideoMode(1024, 768), "MicroSociety") {
    generateMap();
}

void Game::run() {
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear();
        render();
        window.display();
    }
}

void Game::generateMap() {
    // Load textures for grass, stone, and flower tiles
    std::vector<sf::Texture> grassTextures(3), stoneTextures(3), flowerTextures(5);
    grassTextures[0].loadFromFile("assets/tiles/grass/grass1.png");
    grassTextures[1].loadFromFile("assets/tiles/grass/grass2.png");
    grassTextures[2].loadFromFile("assets/tiles/grass/grass3.png");

    stoneTextures[0].loadFromFile("assets/tiles/stone/stone1.png");
    stoneTextures[1].loadFromFile("assets/tiles/stone/stone2.png");
    stoneTextures[2].loadFromFile("assets/tiles/stone/stone3.png");

    flowerTextures[0].loadFromFile("assets/tiles/flower/flower1.png");
    flowerTextures[1].loadFromFile("assets/tiles/flower/flower2.png");
    flowerTextures[2].loadFromFile("assets/tiles/flower/flower3.png");
    flowerTextures[3].loadFromFile("assets/tiles/flower/flower4.png");
    flowerTextures[4].loadFromFile("assets/tiles/flower/flower5.png");

    int rows = 64, cols = 64;
    tileMap.resize(rows);

    for (int i = 0; i < rows; ++i) {
        tileMap[i].resize(cols);
        for (int j = 0; j < cols; ++j) {
            int random = rand() % 11;  // Random value for grass, stone, or flower
            if (random < 3)
                tileMap[i][j] = std::make_unique<GrassTile>(grassTextures[random]);
            else if (random < 6)
                tileMap[i][j] = std::make_unique<StoneTile>(stoneTextures[random - 3]);
            else
                tileMap[i][j] = std::make_unique<FlowerTile>(flowerTextures[random - 6]);

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
