#ifndef ACTIONS_HPP
#define ACTIONS_HPP

#include "Player.hpp"
#include "Tile.hpp"
#include <iostream>
#include <string>

enum class ActionType {
    None,
    ChopTree,
    MineRock,
    GatherBush,
    Move,             // New type for move actions
    Trade             // New type for trading actions
};

// Existing Base Action class for resource gathering
class Action {
public:
    virtual ~Action() = default;
    virtual void perform(PlayerEntity& player, Tile& tile) = 0; // Pure virtual function for action
    virtual std::string getActionName() const = 0;              // Name of the action
};

// TreeAction for chopping trees
class TreeAction : public Action {
public:
    void perform(PlayerEntity& player, Tile& tile) override {
        if (player.addToInventory("wood", 1)) {
            tile.removeObject(); // Remove the tree after chopping
        }
    }
    std::string getActionName() const override { return "Chop Tree"; }
};

// StoneAction for mining rocks
class StoneAction : public Action {
public:
    void perform(PlayerEntity& player, Tile& tile) override {
        if (player.addToInventory("stone", 1)) {
            tile.removeObject(); // Remove the rock after mining
        }
    }
    std::string getActionName() const override { return "Mine Stone"; }
};

// BushAction for gathering from bushes
class BushAction : public Action {
public:
    void perform(PlayerEntity& player, Tile& tile) override {
        if (player.addToInventory("food", 1)) {
            tile.removeObject(); // Remove the bush after gathering
        }
    }
    std::string getActionName() const override { return "Gather from Bush"; }
};

// New flexible MoveAction for navigation
class MoveAction : public Action {
public:
    void perform(PlayerEntity& player, Tile&) override {
        std::cout << "Executing Move Action\n";
    }
    std::string getActionName() const override { return "Move"; }
};

// New TradeAction for trading with other NPCs or the market
class TradeAction : public Action {
public:
    void perform(PlayerEntity& player, Tile&) override {
        std::cout << "Executing Trade Action\n";
    }
    std::string getActionName() const override { return "Trade"; }
};

#endif // ACTIONS_HPP
