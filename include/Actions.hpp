#ifndef ACTIONS_HPP
#define ACTIONS_HPP

#include <string>
#include "Player.hpp"
#include "Tile.hpp"

enum class ActionType {
    None,
    ChopTree,
    MineRock,
    GatherBush
};

// Base class for actions
class Action {
public:
    virtual void perform(PlayerEntity& player) = 0; // Pure virtual function for action
    virtual std::string getActionName() const = 0;  // Name of the action
    virtual ~Action() = default;
};

// Specific action to chop down trees
class TreeAction : public Action {
public:
    void perform(PlayerEntity& player) override {
        // Implementation: add wood to player's inventory, decrease tool durability, etc.
        player.addToInventory("wood", 1);
    }
    std::string getActionName() const override { return "Chop Tree"; }
};

// Specific action to mine stones
class StoneAction : public Action {
public:
    void perform(PlayerEntity& player) override {
        // Implementation: add stone to player's inventory, decrease tool durability, etc.
        player.addToInventory("stone", 1);
    }
    std::string getActionName() const override { return "Mine Stone"; }
};

// Specific action to gather from bushes
class BushAction : public Action {
public:
    void perform(PlayerEntity& player) override {
        // Implementation: add food to player's inventory, possibly apply conditions (e.g., season)
        player.addToInventory("food", 1);
    }
    std::string getActionName() const override { return "Gather from Bush"; }
};

#endif
