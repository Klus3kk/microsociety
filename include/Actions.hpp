#ifndef ACTIONS_HPP
#define ACTIONS_HPP

#include "Player.hpp"
#include "Tile.hpp"
#include <iostream>
#include <string>
#include "House.hpp"
#include "debug.hpp"

enum class ActionType {
    None,
    ChopTree,
    MineRock,
    GatherBush,
    Move,
    Trade,
    RegenerateEnergy,
    UpgradeHouse,
    StoreItem             
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
            tile.removeObject(); // Remove tree object from the tile
            getDebugConsole().log("Action", "Tree chopped! Wood added to inventory.");
        } else {
            getDebugConsole().log("Action", "Failed to chop tree. Inventory is full.");
        }
    }

    std::string getActionName() const override { return "Chop Tree"; }
};

// StoneAction for mining rocks
class StoneAction : public Action {
public:
    void perform(PlayerEntity& player, Tile& tile) override {
        if (player.addToInventory("stone", 1)) {
            tile.removeObject(); // Remove rock object from the tile
            getDebugConsole().log("Action", "Rock mined! Stone added to inventory.");
        } else {
            getDebugConsole().log("Action", "Failed to mine rock. Inventory is full.");
        }
    }

    std::string getActionName() const override { return "Mine Rock"; }
};

// BushAction for gathering from bushes
class BushAction : public Action {
public:
    void perform(PlayerEntity& player, Tile& tile) override {
        if (player.addToInventory("food", 1)) {
            tile.removeObject(); // Remove bush object from the tile
            getDebugConsole().log("Action", "Food gathered from bush!");
        } else {
            getDebugConsole().log("Action", "Failed to gather food. Inventory is full.");
        }
    }

    std::string getActionName() const override { return "Gather Bush"; }
};

// Flexible MoveAction for navigation
class MoveAction : public Action {
public:
    void perform(PlayerEntity& player, Tile&) override {
        getDebugConsole().log("Action", "Player moved.");
    }
    std::string getActionName() const override { return "Move"; }
};

// TradeAction for trading with other NPCs or the market
class TradeAction : public Action {
public:
    void perform(PlayerEntity& player, Tile&) override {
        getDebugConsole().log("Action", "Trade action performed.");
    }
    std::string getActionName() const override { return "Trade"; }
};


class RegenerateEnergyAction : public Action {
public:
    void perform(PlayerEntity& player, Tile& tile) override {
        if (auto house = dynamic_cast<House*>(tile.getObject())) {
            house->regenerateEnergy(player);
            getDebugConsole().log("Action", "Player energy regenerated.");
        } else {
            getDebugConsole().log("Action", "No house found to regenerate energy.");
        }
    }
    std::string getActionName() const override { return "Regenerate Energy"; }
};


// UpgradeHouseAction for upgrading the house
class UpgradeHouseAction : public Action {
public:
    void perform(PlayerEntity& player, Tile& tile) override {
        if (auto house = dynamic_cast<House*>(tile.getObject())) {
            int playerMoney = player.getMoney();
            if (house->upgrade(playerMoney)) {
                player.setMoney(playerMoney); 
                getDebugConsole().log("Action", "House upgraded successfully.");
            } else {
                getDebugConsole().log("Action", "Not enough money to upgrade the house.");
            }
        } else {
            getDebugConsole().log("Action", "No house found to upgrade.");
        }
    }
    std::string getActionName() const override { return "Upgrade House"; }
};


// StoreItemAction for storing items in the house
class StoreItemAction : public Action {
private:
    std::string item; 
    int quantity;     

public:
    StoreItemAction(const std::string& item, int quantity)
        : item(item), quantity(quantity) {}

    void perform(PlayerEntity& player, Tile& tile) override {
        if (auto house = dynamic_cast<House*>(tile.getObject())) {
            const auto& inventory = player.getInventory();
            if (inventory.empty()) {
                getDebugConsole().log("Action", "No items in inventory to store.");
                return;
            }

            if (inventory.count(item) > 0 && inventory.at(item) >= quantity) {
                if (house->storeItem(item, quantity)) {
                    player.addToInventory(item, -quantity); // Remove stored items from inventory
                    getDebugConsole().log("Action", "Stored " + std::to_string(quantity) + " " + item + " in the house.");
                } else {
                    getDebugConsole().log("Action", "House storage is full! Could not store all items.");
                }
            } else {
                getDebugConsole().log("Action", "Insufficient " + item + " in inventory.");
            }
        } else {
            getDebugConsole().log("Action", "No house present on this tile.");
        }
    }

    std::string getActionName() const override {
        return "Store Items in House";
    }
};


#endif 
