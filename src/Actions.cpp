#include "Actions.hpp"

// Base class method is overridden by each action type
void TreeAction::perform(NPCEntity& player, Tile& tile, const std::vector<std::vector<std::unique_ptr<Tile>>>& tileMap) {
    // Check if the tile has an object (tree)
    if (tile.hasObject()) {
        // Attempt to add wood to the player's inventory
        if (player.addToInventory("wood", 1)) {
            tile.removeObject(); // Remove tree from the map
            player.consumeEnergy(5.0f); // Reduce player's energy
            player.receiveFeedback(10.0f, tileMap); // Reward for success
            getDebugConsole().log("TreeAction", "Tree chopped! Wood added to inventory.");
        } else {
            player.receiveFeedback(-2.0f, tileMap); // Penalty if inventory is full
            getDebugConsole().logOnce("TreeAction", "Inventory full. Cannot chop tree.");
        }
    } else {
        player.receiveFeedback(-5.0f, tileMap); // Penalty if no tree is found
        getDebugConsole().logOnce("TreeAction", "No tree to chop on this tile.");
    }
}

// StoneAction
void StoneAction::perform(NPCEntity& player, Tile& tile, const std::vector<std::vector<std::unique_ptr<Tile>>>& tileMap) {
    // Check if the tile has a rock object
    if (tile.hasObject() && tile.getObject()->getType() == ObjectType::Rock) {
        // Attempt to add stone to inventory
        if (player.addToInventory("stone", 1)) {
            tile.removeObject(); // Remove rock from the tile
            player.consumeEnergy(5.0f);
            player.receiveFeedback(10.0f, tileMap); // Reward for success
            getDebugConsole().log("StoneAction", "Rock mined! Stone added to inventory.");
        } else {
            player.receiveFeedback(-2.0f, tileMap); // Penalty if inventory is full
            getDebugConsole().logOnce("StoneAction", "Inventory full. Cannot mine rock.");
        }
    } else {
        player.receiveFeedback(-5.0f, tileMap); // Penalty if no rock found
        getDebugConsole().logOnce("StoneAction", "No rock to mine on this tile.");
    }
}

// BushAction
void BushAction::perform(NPCEntity& player, Tile& tile, const std::vector<std::vector<std::unique_ptr<Tile>>>& tileMap) {
    // Check if the tile has a bush object
    if (tile.hasObject() && tile.getObject()->getType() == ObjectType::Bush) {
        // Attempt to gather bush resource
        if (player.addToInventory("bush", 1)) {
            tile.removeObject();
            player.consumeEnergy(5.0f);
            player.receiveFeedback(10.0f, tileMap);
            getDebugConsole().log("BushAction", "Bush gathered from tile!");
        } else {
            player.receiveFeedback(-2.0f, tileMap);
            getDebugConsole().logOnce("BushAction", "Inventory full. Cannot gather bush.");
        }
    } else {
        player.receiveFeedback(-5.0f, tileMap);
        getDebugConsole().logOnce("BushAction", "No bush to gather on this tile.");
    }
}

// MoveAction
void MoveAction::perform(NPCEntity& player, Tile&, const std::vector<std::vector<std::unique_ptr<Tile>>>& tileMap) {
    player.consumeEnergy(1.0f); // Reduce energy per move
    player.receiveFeedback(1.0f, tileMap); // Reward for movement
    getDebugConsole().log("Action", "Player moved.");
}

// RegenerateEnergyAction
void RegenerateEnergyAction::perform(NPCEntity& player, Tile& tile, const std::vector<std::vector<std::unique_ptr<Tile>>>& tileMap) {
    // Check if a house exists on the tile
    if (tile.hasObject()) {
        if (auto house = dynamic_cast<House*>(tile.getObject())) {
            house->regenerateEnergy(player); // Restore player's energy
            player.receiveFeedback(5.0f, tileMap); // Reward for regeneration
            getDebugConsole().log("Action", "Energy regenerated at house.");
        } else {
            player.receiveFeedback(-1.0f, tileMap); // Penalty if no house found
            getDebugConsole().logOnce("Action", "No house found to regenerate energy.");
        }
    } else {
        player.receiveFeedback(-1.0f, tileMap);
        getDebugConsole().logOnce("Action", "No object found on this tile.");
    }
}

// UpgradeHouseAction
void UpgradeHouseAction::perform(NPCEntity& player, Tile& tile, const std::vector<std::vector<std::unique_ptr<Tile>>>& tileMap) {
    // Ensure the tile has a house
    if (auto house = dynamic_cast<House*>(tile.getObject())) {
        // FIXED: Use proper money handling with temporary variable
        float npcMoney = player.getMoney(); // Get current money value
        
        if (npcMoney >= house->getUpgradeCost()) {
            // Attempt to upgrade the house
            if (house->upgrade(npcMoney, player)) {
                player.setMoney(npcMoney); // Update the NPC's money after upgrade
                player.receiveFeedback(20.0f, tileMap); // Reward for success
                getDebugConsole().log("Action", "House upgraded successfully.");
            } else {
                player.receiveFeedback(-10.0f, tileMap); // Penalty for failure
                getDebugConsole().logOnce("Action", "Upgrade failed due to insufficient resources.");
            }
        } else {
            player.receiveFeedback(-10.0f, tileMap); // Penalty for insufficient money
            getDebugConsole().logOnce("Action", "Not enough money to upgrade the house.");
        }
    } else {
        player.receiveFeedback(-5.0f, tileMap);
        getDebugConsole().logOnce("Action", "No house found to upgrade.");
    }
}

// StoreItemAction
StoreItemAction::StoreItemAction(const std::string& item, int quantity)
    : item(item), quantity(quantity) {}

void StoreItemAction::perform(NPCEntity& player, Tile& tile, const std::vector<std::vector<std::unique_ptr<Tile>>>& tileMap) {
    if (auto house = dynamic_cast<House*>(tile.getObject())) {
        const auto& inventory = player.getInventory();
        if (inventory.empty()) {
            player.receiveFeedback(-5.0f, tileMap); // Penalty for empty inventory
            getDebugConsole().logOnce("Action", "No items in inventory to store.");
            return;
        }

        if (inventory.count(item) > 0 && inventory.at(item) >= quantity) {
            // Attempt to store the item in the house
            if (house->storeItem(item, quantity)) {
                player.removeFromInventory(item, quantity);
                player.receiveFeedback(5.0f, tileMap); // Reward for storing
                getDebugConsole().log("Action", "Stored " + std::to_string(quantity) + " " + item + " in the house.");
            } else {
                player.receiveFeedback(-5.0f, tileMap);
                getDebugConsole().logOnce("Action", "House storage is full! Could not store all items.");
            }
        } else {
            player.receiveFeedback(-5.0f, tileMap);
            getDebugConsole().logOnce("Action", "Insufficient " + item + " in inventory.");
        }
    } else {
        player.receiveFeedback(-5.0f, tileMap);
        getDebugConsole().logOnce("Action", "No house present on this tile.");
    }
}

std::string StoreItemAction::getActionName() const {
    return "Store Items in House";
}

// BuyItemAction
BuyItemAction::BuyItemAction(const std::string& item, int quantity)
    : item(item), quantity(quantity) {}

void BuyItemAction::perform(NPCEntity& player, Tile& tile, const std::vector<std::vector<std::unique_ptr<Tile>>>& tileMap) {
    // Ensure a market exists on the tile
    if (auto market = dynamic_cast<Market*>(tile.getObject())) {
        if (market->buyItem(player, item, quantity)) {
            player.receiveFeedback(10.0f, tileMap);
            getDebugConsole().log("Action", "Bought " + std::to_string(quantity) + " " + item + " from the market.");
        } else {
            player.receiveFeedback(-5.0f, tileMap);
            getDebugConsole().logOnce("Action", "Failed to buy items. Not enough money or stock.");
        }
    } else {
        player.receiveFeedback(-5.0f, tileMap);
        getDebugConsole().logOnce("Action", "No market present on this tile.");
    }
}

std::string BuyItemAction::getActionName() const {
    return "Buy Items from Market";
}

void SellItemAction::perform(NPCEntity& player, Tile& tile, const std::vector<std::vector<std::unique_ptr<Tile>>>& tileMap) {
    // Ensure a market exists on the tile
    if (auto market = dynamic_cast<Market*>(tile.getObject())) {
        // Attempt to sell items
        if (market->sellItem(player, item, quantity)) {
            player.receiveFeedback(10.0f, tileMap); // Reward for successful sale
            getDebugConsole().log("Action", "Sold " + std::to_string(quantity) + " " + item + " to the market.");
        } else {
            player.receiveFeedback(-5.0f, tileMap); // Penalty if not enough items in inventory
            getDebugConsole().logOnce("Action", "Failed to sell items. Not enough in inventory.");
        }
    } else {
        player.receiveFeedback(-5.0f, tileMap); // Penalty if no market exists
        getDebugConsole().logOnce("Action", "No market present on this tile.");
    }
}

std::string SellItemAction::getActionName() const {
    return "Sell Items to Market";
}

// ExploreAction
void ExploreAction::perform(NPCEntity& player, Tile&, const std::vector<std::vector<std::unique_ptr<Tile>>>& tileMap) {
    // Exploration consumes some energy but provides small rewards
    player.consumeEnergy(2.0f);
    player.receiveFeedback(5.0f, tileMap); // Reward for exploration
    getDebugConsole().log("Action", "NPC explored the map.");
}

// PrioritizeAction
void PrioritizeAction::perform(NPCEntity& player, Tile&, const std::vector<std::vector<std::unique_ptr<Tile>>>& tileMap) {
    // AI-based logic to prioritize actions dynamically
    if (player.getEnergy() < 20.0f) {
        player.receiveFeedback(-10.0f, tileMap); // Penalty for low energy
        getDebugConsole().log("Action", "NPC has low energy and needs to regenerate.");
    } else if (player.getInventorySize() >= player.getMaxInventorySize()) {
        player.receiveFeedback(-5.0f, tileMap); // Penalty for full inventory
        getDebugConsole().log("Action", "NPC inventory is full; needs to store items.");
    } else {
        player.receiveFeedback(10.0f, tileMap); // Reward for efficient prioritization
        getDebugConsole().log("Action", "NPC prioritized its actions successfully.");
    }
}

// IdleAction
void IdleAction::perform(NPCEntity& player, Tile&, const std::vector<std::vector<std::unique_ptr<Tile>>>& tileMap) {
    player.receiveFeedback(-20.0f, tileMap); // Large penalty for idling
    getDebugConsole().log("Action", "NPC idled and lost rewards.");
}

// RestAction
void RestAction::perform(NPCEntity& player, Tile&, const std::vector<std::vector<std::unique_ptr<Tile>>>& tileMap) {
    // Ensure NPC is not already at full energy
    if (player.getEnergy() < player.getMaxEnergy()) {
        player.setEnergy(player.getMaxEnergy()); // Fully restore energy
        player.receiveFeedback(5.0f, tileMap); // Reward for resting
        getDebugConsole().log("Action", "NPC rested and restored energy.");
    } else {
        player.receiveFeedback(-1.0f, tileMap); // Penalty for unnecessary rest
        getDebugConsole().logOnce("Action", "Energy is already full. No need to rest.");
    }
}