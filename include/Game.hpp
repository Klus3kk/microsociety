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
    static constexpr int mapWidth = 800; // damn, thanks chat lol
    static constexpr int mapHeight = 800;
    static constexpr int tileSize = 32; 
    Game();
    void run();
    void generateMap();
    void render();
    
    // getter to access tileMap for testing
    const std::vector<std::vector<std::unique_ptr<Tile>>>& getTileMap() const {
        return tileMap;
    }
};

#endif 
