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
#include "TextureManager.hpp"

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
    std::unordered_map<std::string, int> aggregateResources(const std::vector<NPCEntity>& npcs) const;
    sf::Texture playerTexture;


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
    std::vector<NPCEntity> generateNPCEntitys() const;  

    // Rendering Helpers
    void render();
    void drawTileBorders();


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
    void evaluateNPCEntityState(NPCEntity& NPCEntity);
    void performPathfinding(NPCEntity& NPCEntity);
    // Collision Detection  
    bool detectCollision(NPCEntity& npc);
    void generateMap();
    void storeItems(NPCEntity& npc, Tile& tile);
    void moveToResource(NPCEntity& npc, ActionType actionType);
    // Accessor for TileMap
    const std::vector<std::vector<std::unique_ptr<Tile>>>& getTileMap() const;
};

#endif
