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
private:
    bool treeRemoved = false; // Track if tree removal was logged
public:
    void perform(PlayerEntity& player, Tile& tile) override {
        if (player.addToInventory("wood", 1)) {
            if (!treeRemoved) {
                tile.removeObject(); // Remove tree object
                getDebugConsole().log("Action", "Tree chopped! Wood added to inventory.");
                treeRemoved = true;
            }
        } else {
            getDebugConsole().logOnce("Action", "Failed to chop tree. Inventory is full.");
        }
    }
    std::string getActionName() const override { return "Chop Tree"; }
};

// StoneAction
class StoneAction : public Action {
private:
    bool rockRemoved = false;
public:
    void perform(PlayerEntity& player, Tile& tile) override {
        if (player.addToInventory("stone", 1)) {
            if (!rockRemoved) {
                tile.removeObject(); // Remove rock object
                getDebugConsole().log("Action", "Rock mined! Stone added to inventory.");
                rockRemoved = true;
            }
        } else {
            getDebugConsole().logOnce("Action", "Failed to mine rock. Inventory is full.");
        }
    }
    std::string getActionName() const override { return "Mine Rock"; }
};

// BushAction
class BushAction : public Action {
private:
    bool bushRemoved = false;
public:
    void perform(PlayerEntity& player, Tile& tile) override {
        if (player.addToInventory("food", 1)) {
            if (!bushRemoved) {
                tile.removeObject(); // Remove bush object
                getDebugConsole().log("Action", "Food gathered from bush!");
                bushRemoved = true;
            }
        } else {
            getDebugConsole().logOnce("Action", "Failed to gather food. Inventory is full.");
        }
    }
    std::string getActionName() const override { return "Gather Bush"; }
};

// MoveAction
class MoveAction : public Action {
private:
    bool movedLogged = false;
public:
    void perform(PlayerEntity& player, Tile&) override {
        if (!movedLogged) {
            getDebugConsole().log("Action", "Player moved.");
            movedLogged = true;
        }
    }
    std::string getActionName() const override { return "Move"; }
};

// TradeAction
class TradeAction : public Action {
private:
    bool tradeLogged = false;
public:
    void perform(PlayerEntity& player, Tile&) override {
        if (!tradeLogged) {
            getDebugConsole().log("Action", "Trade action performed.");
            tradeLogged = true;
        }
    }
    std::string getActionName() const override { return "Trade"; }
};

// RegenerateEnergyAction
class RegenerateEnergyAction : public Action {
private:
    bool energyRegenerated = false;
public:
    void perform(PlayerEntity& player, Tile& tile) override {
        if (auto house = dynamic_cast<House*>(tile.getObject())) {
            if (!energyRegenerated) {
                house->regenerateEnergy(player);
                getDebugConsole().log("Action", "Player energy regenerated.");
                energyRegenerated = true;
            }
        } else {
            getDebugConsole().logOnce("Action", "No house found to regenerate energy.");
        }
    }
    std::string getActionName() const override { return "Regenerate Energy"; }
};

// UpgradeHouseAction
class UpgradeHouseAction : public Action {
private:
    bool houseUpgraded = false;
public:
    void perform(PlayerEntity& player, Tile& tile) override {
        if (auto house = dynamic_cast<House*>(tile.getObject())) {
            int playerMoney = player.getMoney();
            if (house->upgrade(playerMoney)) {
                player.setMoney(playerMoney);
                if (!houseUpgraded) {
                    getDebugConsole().log("Action", "House upgraded successfully.");
                    houseUpgraded = true;
                }
            } else {
                getDebugConsole().logOnce("Action", "Not enough money to upgrade the house.");
            }
        } else {
            getDebugConsole().logOnce("Action", "No house found to upgrade.");
        }
    }
    std::string getActionName() const override { return "Upgrade House"; }
};

// StoreItemAction
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
                    getDebugConsole().logOnce("Action", "Stored " + std::to_string(quantity) + " " + item + " in the house.");
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
