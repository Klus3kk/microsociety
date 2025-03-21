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
class PlayerEntity;

class Game {
private:
    // UI and Market
    UI ui;
    Market market;
    House house;
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
    
    // Player Entity (for single player mode)
    std::unique_ptr<PlayerEntity> player;
    bool singlePlayerMode = false;
    
    // AI Settings
    bool reinforcementLearningEnabled = true;
    bool tensorFlowEnabled = false;

    // Private Helper Methods
    std::vector<NPCEntity> generateNPCEntitys() const;  

    // Rendering Helpers
    void render();
    void drawTileBorders();

    float resourceRegenerationTimer = 0.0f;
    const float regenerationInterval = 7.0f; // Regenerate every 3 seconds
    void regenerateResources();
    
    // TensorFlow initialization
    void initializeNPCTensorFlow();
    
    // Single player mode helpers
    void handlePlayerInput();
    void updatePlayer();

public:
    // Constructor
    Game();

    // Main Run Loop
    void run();

    // Simulation Management
    void resetSimulation();
    void logIterationStats(int iteration);
    int getTotalItemsGathered() const;
    int getTotalItemsMined() const; 
    void toggleTileBorders();
    void setSimulationSpeed(float speedFactor);
    void simulateNPCEntityBehavior(float deltaTime);
    void simulateSocietalGrowth(float deltaTime);
    void evaluateNPCEntityState(NPCEntity& NPCEntity);
    void performPathfinding(NPCEntity& NPCEntity);
    void handleMarketActions(NPCEntity& npc, Tile& targetTile, ActionType actionType);
    // Collision Detection  
    bool detectCollision(NPCEntity& npc);
    void generateMap();
    void storeItems(NPCEntity& npc, Tile& tile);
    void moveToResource(NPCEntity& npc, ActionType actionType);
    // Accessor for TileMap
    const std::vector<std::vector<std::unique_ptr<Tile>>>& getTileMap() const;
    
    // Simulation Mode Settings
    void enableSinglePlayerMode(bool enable) { singlePlayerMode = enable; }
    void enableReinforcementLearning(bool enable) { reinforcementLearningEnabled = enable; }
    void enableTensorFlow(bool enable) { tensorFlowEnabled = enable; }
    
    bool isSinglePlayerMode() const { return singlePlayerMode; }
    bool isReinforcementLearningEnabled() const { return reinforcementLearningEnabled; }
    bool isTensorFlowEnabled() const { return tensorFlowEnabled; }
};

#endif