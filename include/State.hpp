#ifndef STATE_HPP
#define STATE_HPP

#include <functional>

// State structure for Q-learning
struct State {
    int posX, posY;         // NPC's position
    int nearbyTrees;        // Number of nearby trees
    int nearbyRocks;        // Number of nearby rocks
    int nearbyBushes;       // Number of nearby bushes
    int energyLevel;        // Energy level (low, medium, high)
    int inventoryLevel;     // Inventory level (empty, partial, full)

    // Comparison operators for unordered_map
    bool operator==(const State& other) const {
        return posX == other.posX &&
               posY == other.posY &&
               nearbyTrees == other.nearbyTrees &&
               nearbyRocks == other.nearbyRocks &&
               nearbyBushes == other.nearbyBushes &&
               energyLevel == other.energyLevel &&
               inventoryLevel == other.inventoryLevel;
    }
};

// Hashing function for State
struct StateHasher {
    std::size_t operator()(const State& state) const {
        std::size_t seed = 0;
        seed ^= std::hash<int>()(state.posX) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        seed ^= std::hash<int>()(state.posY) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        seed ^= std::hash<int>()(state.nearbyTrees) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        seed ^= std::hash<int>()(state.nearbyRocks) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        seed ^= std::hash<int>()(state.nearbyBushes) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        seed ^= std::hash<int>()(state.energyLevel) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        seed ^= std::hash<int>()(state.inventoryLevel) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        return seed;
    }
};

#endif 
