#ifndef CONFIGURATION_HPP
#define CONFIGURATION_HPP

// GameConfig stores all game-related constant values
struct GameConfig {
    // Window configuration
    static constexpr int windowWidth = 800;      
    static constexpr int windowHeight = 800;    
    static constexpr int WINDOW_FPS_LIMIT = 60;

    // Tile configuration
    static constexpr int tileSize = 32;          // Each tile's size in pixels
    static constexpr int mapWidth = windowWidth / tileSize;  // Number of tiles horizontally
    static constexpr int mapHeight = windowHeight / tileSize; // Number of tiles vertically

    // NPC & Inventory configuration
    static constexpr int NPCEntityCount = 10;    // Number of NPCs at game start
    static constexpr int maxInventorySize = 10;  // Maximum inventory size an NPC can carry

    // Resource requirements for basic actions
    static constexpr int BASE_WOOD_REQUIREMENT = 3;
    static constexpr int BASE_STONE_REQUIREMENT = 3;
    static constexpr int BASE_BUSH_REQUIREMENT = 3;

    // House-related configuration
    static constexpr float BASE_UPGRADE_COST = 100.0f; // Cost for house upgrades

    // NPC Attributes (Health, Energy)
    static constexpr float MAX_HEALTH = 100.0f; // Maximum NPC health
    static constexpr float MAX_ENERGY = 100.0f; // Maximum NPC energy
};

#endif
