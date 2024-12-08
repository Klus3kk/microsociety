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
#include "Market.hpp"
#include "TimeManager.hpp"
#include "MoneyManager.hpp"
#include "ClockGUI.hpp"

class Game {
private:
    UI ui;
    Market market;
    sf::RenderWindow window;
    TimeManager timeManager; 
    MoneyManager moneyManager;
    ClockGUI clockGUI;
    float deltaTime;
    std::vector<std::vector<std::unique_ptr<Tile>>> tileMap;
    bool showTileBorders = false;
    float simulationSpeed = 1.0f;
    int mapWidth;   
    int mapHeight;
    int tileSize;
    bool isClockVisible = true;
    std::vector<PlayerEntity> npcs; 

public:
    Game();
    void run();
    void generateMap();
    void render();
    void drawTileBorders();
    bool detectCollision(const PlayerEntity& npc);
    void resetSimulation();
    void toggleTileBorders();
    void setSimulationSpeed(float speedFactor);

    std::unordered_map<std::string, int> aggregateResources(const std::vector<PlayerEntity>& npcs) const;
    std::vector<PlayerEntity> generateNPCs() const;  // For generating npcs
    // Might remove later
    const std::vector<std::vector<std::unique_ptr<Tile>>>& getTileMap() const;
};

#endif 
