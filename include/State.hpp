#ifndef STATE_HPP
#define STATE_HPP

#include <functional> // Required for std::hash

// State structure representing the environment for Q-learning
struct State {
    int posX, posY;         // NPC's position on the map
    int nearbyTrees;        // Number of trees in the vicinity
    int nearbyRocks;        // Number of rocks in the vicinity
    int nearbyBushes;       // Number of bushes in the vicinity
    int energyLevel;        // NPC's energy level (e.g., low, medium, high)
    int inventoryLevel;     // Inventory status (e.g., empty, partial, full)

    // Operator Overloading: Allows direct comparison of State objects
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

// Custom hashing function for unordered_map
struct StateHasher {
    std::size_t operator()(const State& state) const {
        std::size_t seed = 0; // Initialize seed for hash calculation

        // Hash each field of the state and combine them using bitwise XOR (^)
        seed ^= std::hash<int>()(state.posX) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        seed ^= std::hash<int>()(state.posY) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        seed ^= std::hash<int>()(state.nearbyTrees) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        seed ^= std::hash<int>()(state.nearbyRocks) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        seed ^= std::hash<int>()(state.nearbyBushes) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        seed ^= std::hash<int>()(state.energyLevel) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        seed ^= std::hash<int>()(state.inventoryLevel) + 0x9e3779b9 + (seed << 6) + (seed >> 2);

        return seed; // Return the unique hash value for the state
    }
};

#endif
