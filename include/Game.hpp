#ifndef GAME_HPP
#define GAME_HPP

#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include "Tile.hpp"

class PlayerEntity;  // Forward declaration

class Game {
private:
    sf::RenderWindow window;
    float deltaTime;
    std::vector<std::vector<std::unique_ptr<Tile>>> tileMap;

public:
    static constexpr int mapWidth = 800;
    static constexpr int mapHeight = 800;
    static constexpr int tileSize = 32; 
    Game();
    void run();
    void generateMap();
    void render();
    void drawTileBorders();

    const std::vector<std::vector<std::unique_ptr<Tile>>>& getTileMap() const {
        return tileMap;
    }

    bool detectCollisionWithTile(const PlayerEntity& player, int tileX, int tileY) const;
};

#endif
