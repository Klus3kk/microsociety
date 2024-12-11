#ifndef GAME_HPP
#define GAME_HPP

#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include <unordered_map>
#include "Tile.hpp"
#include "Actions.hpp"
#include "House.hpp"
#include "UI.hpp"
#include "Market.hpp"
#include "TimeManager.hpp"
#include "MoneyManager.hpp"
#include "ClockGUI.hpp"
#include "Configuration.hpp"

// Forward declaration for the NPCEntity class
class NPCEntity;

class Game {
private:
    // UI and Market
    UI ui;
    Market market;

    // Window and Graphics
    sf::RenderWindow window;
    ClockGUI clockGUI;

    // Simulation Control
    float deltaTime;
    bool showTileBorders = false;
    bool isClockVisible = true;
    float simulationSpeed = 1.0f;

    // Map and Environment
    std::vector<std::vector<std::unique_ptr<Tile>>> tileMap;
    int mapWidth;
    int mapHeight;
    int tileSize;

    // Time and Resources Management
    TimeManager timeManager;
    MoneyManager moneyManager;

    // NPCEntity Management
    std::vector<NPCEntity> npcs;

    // Private Helper Methods
    void generateMap();
    std::vector<NPCEntity> generateNPCs() const;  

    // Rendering Helpers
    void render();
    void drawTileBorders();

    // Utility
    std::unordered_map<std::string, int> aggregateResources() const;

public:
    // Constructor
    Game();

    // Main Run Loop
    void run();

    // Simulation Management
    void resetSimulation();
    void toggleTileBorders();
    void setSimulationSpeed(float speedFactor);
    void simulateNPCEntityBehavior(float deltaTime);
    void simulateSocietalGrowth(float deltaTime);

    // Collision Detection
    bool detectCollision(const NPCEntity& npc);

    // Accessor for TileMap
    const std::vector<std::vector<std::unique_ptr<Tile>>>& getTileMap() const;
};

#endif
