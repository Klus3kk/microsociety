#ifndef CONFIGURATION_HPP
#define CONFIGURATION_HPP

// GameConfig stores core, mostly static game parameters.
// Behaviour/tuning parameters (rewards, decay rates, etc.) live in SimulationConfig.
struct GameConfig {
    // Window configuration
    static constexpr int windowWidth      = 800;
    static constexpr int windowHeight     = 800;
    static constexpr int WINDOW_FPS_LIMIT = 60;

    // Tile configuration
    static constexpr int tileSize  = 32;                      // Tile size in pixels
    static constexpr int mapWidth  = windowWidth / tileSize;  // Tiles horizontally
    static constexpr int mapHeight = windowHeight / tileSize; // Tiles vertically

    // NPC & Inventory configuration
    static constexpr int NPCEntityCount   = 10; // NPCs at game start
    static constexpr int maxInventorySize = 10; // Max inventory size per NPC

    // Resource requirements for basic actions
    static constexpr int BASE_WOOD_REQUIREMENT  = 3;
    static constexpr int BASE_STONE_REQUIREMENT = 3;
    static constexpr int BASE_BUSH_REQUIREMENT  = 3;

    // House-related configuration
    static constexpr float BASE_UPGRADE_COST = 100.0f; // Base house upgrade cost

    // NPC attributes (health, energy)
    static constexpr float MAX_HEALTH = 100.0f;
    static constexpr float MAX_ENERGY = 100.0f;
};

#endif
