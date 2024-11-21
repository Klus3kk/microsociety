#ifndef GAME_HPP
#define GAME_HPP

#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include <unordered_map>
#include "Tile.hpp"
#include "Player.hpp"  
#include "Configuration.hpp" 
#include "Actions.hpp"
#include "House.hpp"
#include "UI.hpp"

class Game {
private:
    UI ui;
    sf::RenderWindow window;
    float deltaTime;
    std::vector<std::vector<std::unique_ptr<Tile>>> tileMap;

    int mapWidth;
    int mapHeight;
    int tileSize;

public:
    Game();
    void run();
    void generateMap();
    void render();
    void drawTileBorders();
    bool detectCollision(const PlayerEntity& npc);

    std::unordered_map<std::string, int> aggregateResources(const std::vector<PlayerEntity>& npcs) const;
    std::vector<PlayerEntity> generateNPCs() const;  // For generating npcs
    // Might remove later
    const std::vector<std::vector<std::unique_ptr<Tile>>>& getTileMap() const;


};

#endif 
