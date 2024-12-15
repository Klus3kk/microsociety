#ifndef CONFIGURATION_HPP
#define CONFIGURATION_HPP

struct GameConfig {
    static constexpr int windowWidth = 800;      // Window width in pixels
    static constexpr int windowHeight = 800;     // Window height in pixels
    static constexpr int tileSize = 32;          // Size of each tile in pixels
    static constexpr int maxInventorySize = 10;  // Maximum inventory size
    static constexpr int NPCEntityCount = 10;
    
    static constexpr int mapWidth = windowWidth / tileSize;  // Number of tiles horizontally
    static constexpr int mapHeight = windowHeight / tileSize; // Number of tiles vertically
};

#endif
