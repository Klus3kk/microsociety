#include "Actions.hpp"

// TreeAction
void TreeAction::perform(PlayerEntity& player, Tile& tile) {
    if (tile.hasObject() && player.addToInventory("wood", 1)) {
        tile.removeObject();
        getDebugConsole().log("Action", "Tree chopped! Wood added to inventory.");
    } else if (!tile.hasObject()) {
        getDebugConsole().logOnce("Action", "No tree to chop on this tile.");
    } else {
        getDebugConsole().logOnce("Action", "Failed to chop tree. Inventory is full.");
    }
}

// StoneAction
void StoneAction::perform(PlayerEntity& player, Tile& tile) {
    if (tile.hasObject() && player.addToInventory("stone", 1)) {
        tile.removeObject();
        getDebugConsole().log("Action", "Rock mined! Stone added to inventory.");
    } else if (!tile.hasObject()) {
        getDebugConsole().logOnce("Action", "No rock to mine on this tile.");
    } else {
        getDebugConsole().logOnce("Action", "Failed to mine rock. Inventory is full.");
    }
}

// BushAction
void BushAction::perform(PlayerEntity& player, Tile& tile) {
    if (tile.hasObject() && player.addToInventory("food", 1)) {
        tile.removeObject();
        getDebugConsole().log("Action", "Food gathered from bush!");
    } else if (!tile.hasObject()) {
        getDebugConsole().logOnce("Action", "No bush to gather food from on this tile.");
    } else {
        getDebugConsole().logOnce("Action", "Failed to gather food. Inventory is full.");
    }
}

// MoveAction
void MoveAction::perform(PlayerEntity& player, Tile&) {
    getDebugConsole().log("Action", "Player moved.");
}

// TradeAction
void TradeAction::perform(PlayerEntity& player, Tile&) {
    getDebugConsole().log("Action", "Trade action performed.");
}

// RegenerateEnergyAction
void RegenerateEnergyAction::perform(PlayerEntity& player, Tile& tile) {
    if (auto house = dynamic_cast<House*>(tile.getObject())) {
        house->regenerateEnergy(player);
        getDebugConsole().log("Action", "Player energy regenerated.");
    } else {
        getDebugConsole().logOnce("Action", "No house found to regenerate energy.");
    }
}

// UpgradeHouseAction
void UpgradeHouseAction::perform(PlayerEntity& player, Tile& tile) {
    if (auto house = dynamic_cast<House*>(tile.getObject())) {
        float playerMoney = player.getMoney();
        if (house->upgrade(playerMoney, player)) {
            player.setMoney(playerMoney);
            getDebugConsole().log("Action", "House upgraded successfully.");
        } else {
            getDebugConsole().logOnce("Action", "Not enough money to upgrade the house.");
        }
    } else {
        getDebugConsole().logOnce("Action", "No house found to upgrade.");
    }
}

// StoreItemAction
StoreItemAction::StoreItemAction(const std::string& item, int quantity)
    : item(item), quantity(quantity) {}

void StoreItemAction::perform(PlayerEntity& player, Tile& tile) {
    if (auto house = dynamic_cast<House*>(tile.getObject())) {
        const auto& inventory = player.getInventory();
        if (inventory.empty()) {
            getDebugConsole().logOnce("Action", "No items in inventory to store.");
            return;
        }

        if (inventory.count(item) > 0 && inventory.at(item) >= quantity) {
            if (house->storeItem(item, quantity)) {
                player.addToInventory(item, -quantity);
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

std::string StoreItemAction::getActionName() const {
    return "Store Items in House";
}

// TakeOutItemsAction
TakeOutItemsAction::TakeOutItemsAction(const std::string& item, int quantity)
    : item(item), quantity(quantity) {}

void TakeOutItemsAction::perform(PlayerEntity& player, Tile& tile) {
    if (auto house = dynamic_cast<House*>(tile.getObject())) {
        if (house->takeFromStorage(item, quantity, player)) {
            getDebugConsole().log("Action", "Took " + std::to_string(quantity) + " " + item + " from the house.");
        } else {
            getDebugConsole().logOnce("Action", "Failed to take items. Not enough in storage.");
        }
    } else {
        getDebugConsole().logOnce("Action", "No house present on this tile.");
    }
}

std::string TakeOutItemsAction::getActionName() const {
    return "Take Out Items from House";
}

// BuyItemAction
BuyItemAction::BuyItemAction(const std::string& item, int quantity)
    : item(item), quantity(quantity) {}

void BuyItemAction::perform(PlayerEntity& player, Tile& tile) {
    if (auto market = dynamic_cast<Market*>(tile.getObject())) {
        if (market->buyItem(player, item, quantity)) {
            getDebugConsole().log("Action", "Bought " + std::to_string(quantity) + " " + item + " from the market.");
        } else {
            getDebugConsole().logOnce("Action", "Failed to buy items. Not enough money or stock.");
        }
    } else {
        getDebugConsole().logOnce("Action", "No market present on this tile.");
    }
}

std::string BuyItemAction::getActionName() const {
    return "Buy Items from Market";
}

// SellItemAction
SellItemAction::SellItemAction(const std::string& item, int quantity)
    : item(item), quantity(quantity) {}

void SellItemAction::perform(PlayerEntity& player, Tile& tile) {
    if (auto market = dynamic_cast<Market*>(tile.getObject())) {
        if (market->sellItem(player, item, quantity)) {
            getDebugConsole().log("Action", "Sold " + std::to_string(quantity) + " " + item + " to the market.");
        } else {
            getDebugConsole().logOnce("Action", "Failed to sell items. Not enough in inventory.");
        }
    } else {
        getDebugConsole().logOnce("Action", "No market present on this tile.");
    }
}

std::string SellItemAction::getActionName() const {
    return "Sell Items to Market";
}
