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

class NPCEntity;
class PlayerEntity;

class Game {
private:
    UI ui;
    Market market;
    House house;
    ClockGUI clockGUI;
    sf::RenderWindow window;
    sf::Texture playerTexture;
    std::unordered_map<std::string, int> aggregateResources(const std::vector<NPCEntity>& npcs) const;

    // simulation control
    float deltaTime;
    bool showTileBorders = false;
    bool isClockVisible = true;
    float simulationSpeed = 1.0f;
    float resourceRegenerationTimer = 0.0f;
    const float regenerationInterval = 7.0f; // regenerate resources every 7 seconds

    // map and tiles
    std::vector<std::vector<std::unique_ptr<Tile>>> tileMap;
    int mapWidth;
    int mapHeight;
    int tileSize;

    // time/resources management
    TimeManager timeManager;
    MoneyManager moneyManager;

    // NPC management
    std::vector<NPCEntity> npcs;
    
    // Player Entity (for single player mode) - TO REMOVE!
    PlayerEntity* player = nullptr;
    bool singlePlayerMode = false;
    
    // AI Settings
    bool reinforcementLearningEnabled = true;
    bool tensorFlowEnabled = false;

    // generate NPCs
    std::vector<NPCEntity> generateNPCEntities() const;  

    // render
    void render();
    void drawTileBorders();
    void regenerateResources();

    // TensorFlow initialization
    void initializeNPCTensorFlow();
    
    // data collection
    void checkDataCollectionProgress();
    
    // Single player mode helpers - TO REMOVE!
    void handlePlayerInput(); 
    void updatePlayer();
    
    // Player cleanup - TO REMOVE!
    void cleanupPlayer(); // I think it's for single-player, so remove!

    // Statistics across simulations
    struct SimulationStats {
        int totalItemsGatheredAllTime = 0;
        int totalItemsSoldAllTime = 0;
        int totalIterations = 0;
        int totalMoneySpentAllTime = 0;
        int totalMoneyEarnedAllTime = 0;
    } persistentStats;

    // Update persistent stats
    void updatePersistentStats();

public:
    Game();
    ~Game();

    // main loop
    void run();

    // simulation management
    void generateMap();
    void resetSimulation();
    void logIterationStats(int iteration);
    void setSimulationSpeed(float speedFactor);
    void toggleTileBorders();
    const std::vector<std::vector<std::unique_ptr<Tile>>>& getTileMap() const;

    int getTotalItemsGathered() const;
    int getTotalItemsMined() const; 
    
    void storeItems(NPCEntity& npc, Tile& tile);
    bool detectCollision(Entity& entity);
    void simulateNPCEntityBehavior(float deltaTime);
    void simulateSocietalGrowth(float deltaTime);
    void evaluateNPCEntityState(NPCEntity& NPCEntity);
    void performPathfinding(NPCEntity& NPCEntity);
    void moveToResource(NPCEntity& npc, ActionType actionType);
    void handleMarketActions(NPCEntity& npc, Tile& targetTile, ActionType actionType);

    // simulation mode settings
    void enableSinglePlayerMode(bool enable); // TO REMOVE!
    void enableReinforcementLearning(bool enable) { reinforcementLearningEnabled = enable; }
    void enableTensorFlow(bool enable);
    
    bool isSinglePlayerMode() const { return singlePlayerMode; } // TO REMOVE!
    bool isReinforcementLearningEnabled() const { return reinforcementLearningEnabled; }
    bool isTensorFlowEnabled() const { return tensorFlowEnabled; }
};

#endif
