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

// Base Action class
class Action {
public:
    virtual ~Action() = default;
    virtual void perform(PlayerEntity& player, Tile& tile) = 0; // Pure virtual function
    virtual std::string getActionName() const = 0;              // Name of the action
};

// TreeAction
class TreeAction : public Action {
public:
    void perform(PlayerEntity& player, Tile& tile) override {
        if (tile.hasObject() && player.addToInventory("wood", 1)) {
            tile.removeObject(); 
            getDebugConsole().log("Action", "Tree chopped! Wood added to inventory.");
        } else if (!tile.hasObject()) {
            getDebugConsole().logOnce("Action", "No tree to chop on this tile.");
        } else {
            getDebugConsole().logOnce("Action", "Failed to chop tree. Inventory is full.");
        }
    }
    std::string getActionName() const override { return "Chop Tree"; }
};

class StoneAction : public Action {
public:
    void perform(PlayerEntity& player, Tile& tile) override {
        if (tile.hasObject() && player.addToInventory("stone", 1)) {
            tile.removeObject(); 
            getDebugConsole().log("Action", "Rock mined! Stone added to inventory.");
        } else if (!tile.hasObject()) {
            getDebugConsole().logOnce("Action", "No rock to mine on this tile.");
        } else {
            getDebugConsole().logOnce("Action", "Failed to mine rock. Inventory is full.");
        }
    }
    std::string getActionName() const override { return "Mine Rock"; }
};

class BushAction : public Action {
public:
    void perform(PlayerEntity& player, Tile& tile) override {
        if (tile.hasObject() && player.addToInventory("food", 1)) {
            tile.removeObject(); 
            getDebugConsole().log("Action", "Food gathered from bush!");
        } else if (!tile.hasObject()) {
            getDebugConsole().logOnce("Action", "No bush to gather food from on this tile.");
        } else {
            getDebugConsole().logOnce("Action", "Failed to gather food. Inventory is full.");
        }
    }
    std::string getActionName() const override { return "Gather Bush"; }
};

class MoveAction : public Action {
public:
    void perform(PlayerEntity& player, Tile&) override {
        getDebugConsole().log("Action", "Player moved.");
    }
    std::string getActionName() const override { return "Move"; }
};

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
            getDebugConsole().logOnce("Action", "No house found to regenerate energy.");
        }
    }
    std::string getActionName() const override { return "Regenerate Energy"; }
};

class UpgradeHouseAction : public Action {
public:
    void perform(PlayerEntity& player, Tile& tile) override {
        if (auto house = dynamic_cast<House*>(tile.getObject())) {
            int playerMoney = player.getMoney();
            if (house->upgrade(playerMoney)) {
                player.setMoney(playerMoney);
                getDebugConsole().log("Action", "House upgraded successfully.");
            } else {
                getDebugConsole().logOnce("Action", "Not enough money to upgrade the house.");
            }
        } else {
            getDebugConsole().logOnce("Action", "No house found to upgrade.");
        }
    }
    std::string getActionName() const override { return "Upgrade House"; }
};

class StoreItemAction : public Action {
private:
    std::string item;  // Item to store
    int quantity;      // Quantity of the item

public:
    StoreItemAction(const std::string& item, int quantity)
        : item(item), quantity(quantity) {}

    void perform(PlayerEntity& player, Tile& tile) override {
        if (auto house = dynamic_cast<House*>(tile.getObject())) {
            const auto& inventory = player.getInventory();
            if (inventory.empty()) {
                getDebugConsole().logOnce("Action", "No items in inventory to store.");
                return;
            }

            if (inventory.count(item) > 0 && inventory.at(item) >= quantity) {
                if (house->storeItem(item, quantity)) {
                    player.addToInventory(item, -quantity); // Remove stored items
                    getDebugConsole().log("Action", "Stored " + std::to_string(quantity) + " " + item + " in the house.");
                } else {
                    getDebugConsole().logOnce("Action", "House storage is full! Could not store all items.");
                }
            } else {
                getDebugConsole().logOnce("Action", "Insufficient " + item + " in inventory.");
            }
        } else {
            getDebugConsole().logOnce("Action", "No house present on this tile.");
        }
    }

    std::string getActionName() const override {
        return "Store Items in House";
    }
};


#endif
