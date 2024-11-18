#ifndef ACTIONS_HPP
#define ACTIONS_HPP

#include "Player.hpp"
#include "Tile.hpp"
#include <iostream>
#include <string>
#include "House.hpp"

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

// Flexible MoveAction for navigation
class MoveAction : public Action {
public:
    void perform(PlayerEntity& player, Tile&) override {
        std::cout << "Executing Move Action\n";
    }
    std::string getActionName() const override { return "Move"; }
};

// TradeAction for trading with other NPCs or the market
class TradeAction : public Action {
public:
    void perform(PlayerEntity& player, Tile&) override {
        std::cout << "Executing Trade Action\n";
    }
    std::string getActionName() const override { return "Trade"; }
};

class RegenerateEnergyAction : public Action {
public:
    void perform(PlayerEntity& player, Tile& tile) override {
        if (auto house = dynamic_cast<House*>(tile.getObject())) {
            house->regenerateEnergy(player);
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
                player.setMoney(playerMoney); // Update player's money after upgrade
            }
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
                std::cout << "No items in inventory to store.\n";
                return;
            }

            // Iterate through player's inventory and store items
            for (const auto& [item, quantity] : inventory) {
                if (house->storeItem(item, quantity)) {
                    player.addToInventory(item, -quantity); // Remove stored items from player inventory
                    std::cout << "Stored " << quantity << " " << item << " in the house.\n";
                } else {
                    std::cout << "House storage is full! Could not store all items.\n";
                }
            }
        } else {
            std::cout << "No house present on this tile.\n";
        }
    }

    std::string getActionName() const override {
        return "Store Items in House";
    }
};


#endif 
