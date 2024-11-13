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
    void perform(PlayerEntity& player, Tile& tile) override {
        if (player.addToInventory("wood", 1)) {
            tile.removeObject(); // Remove the tree after chopping
        }
    }
    std::string getActionName() const override { return "Chop Tree"; }
};

class StoneAction : public Action {
public:
    void perform(PlayerEntity& player, Tile& tile) override {
        if (player.addToInventory("stone", 1)) {
            tile.removeObject(); // Remove the rock after mining
        }
    }
    std::string getActionName() const override { return "Mine Stone"; }
};

class BushAction : public Action {
public:
    void perform(PlayerEntity& player, Tile& tile) override {
        if (player.addToInventory("food", 1)) {
            tile.removeObject(); // Remove the bush after gathering
        }
    }
    std::string getActionName() const override { return "Gather from Bush"; }
};


#endif
