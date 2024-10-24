#ifndef GAME_HPP
#define GAME_HPP

#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include "Tile.hpp"
#include "GrassTile.hpp"
#include "StoneTile.hpp"
#include "FlowerTile.hpp"

class Game {
private:
    sf::RenderWindow window;
    float deltaTime;
    std::vector<std::vector<std::unique_ptr<Tile>>> tileMap;

public:
    int mapWidth = 1024;
    int mapHeight = 768;
    Game();
    void run();
    void generateMap();
    void render();
    
    // Getter to access tileMap for testing
    const std::vector<std::vector<std::unique_ptr<Tile>>>& getTileMap() const {
        return tileMap;
    }
};

#endif // GAME_HPP
