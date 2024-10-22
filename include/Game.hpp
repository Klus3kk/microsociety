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
    std::vector<std::vector<std::unique_ptr<Tile>>> tileMap;

public:
    Game();
    void run();
    void generateMap();
    void render();
};

#endif // GAME_HPP
