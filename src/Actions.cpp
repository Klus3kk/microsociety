#include "Actions.hpp"

// TreeAction
void TreeAction::perform(NPCEntity& player, Tile& tile) {
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
void StoneAction::perform(NPCEntity& player, Tile& tile) {
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
void BushAction::perform(NPCEntity& player, Tile& tile) {
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
void MoveAction::perform(NPCEntity& player, Tile&) {
    getDebugConsole().log("Action", "Player moved.");
}

// TradeAction
void TradeAction::perform(NPCEntity& player, Tile&) {
    getDebugConsole().log("Action", "Trade action performed.");
}

// RegenerateEnergyAction
void RegenerateEnergyAction::perform(NPCEntity& player, Tile& tile) {
    if (auto house = dynamic_cast<House*>(tile.getObject())) {
        house->regenerateEnergy(player);
        getDebugConsole().log("Action", "Player energy regenerated.");
    } else {
        getDebugConsole().logOnce("Action", "No house found to regenerate energy.");
    }
}

// UpgradeHouseAction
void UpgradeHouseAction::perform(NPCEntity& player, Tile& tile) {
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

void StoreItemAction::perform(NPCEntity& player, Tile& tile) {
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

void TakeOutItemsAction::perform(NPCEntity& player, Tile& tile) {
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

void BuyItemAction::perform(NPCEntity& player, Tile& tile) {
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

void SellItemAction::perform(NPCEntity& player, Tile& tile) {
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

// ExploreAction
void ExploreAction::perform(NPCEntity& player, Tile&) {
    // Exploration consumes some energy but provides small rewards
    player.consumeEnergy(2.0f);
    player.addReward(5.0f); // Small reward for exploration
    getDebugConsole().log("Action", "NPC explored the map.");
}

// BuildAction
void BuildAction::perform(NPCEntity& player, Tile& tile) {
    if (tile.hasObject()) {
        player.addPenalty(-10.0f); // Penalty for trying to build on an occupied tile
        getDebugConsole().logOnce("Action", "Tile is occupied. Cannot build here.");
        return;
    }

    if (player.getInventoryItemCount("wood") >= 5 && player.getInventoryItemCount("stone") >= 3) {
        player.removeFromInventory("wood", 5);
        player.removeFromInventory("stone", 3);

        auto& textureManager = TextureManager::getInstance();
        const sf::Texture& texture = textureManager.getTexture("house1", "../assets/objects/house1.png");

        auto newHouse = std::make_unique<House>(texture, 1); // Pass texture and level
        tile.placeObject(std::move(newHouse));

        player.addReward(getReward());
        getDebugConsole().log("Action", "NPC built a structure on the tile.");
    } else {
        player.addPenalty(-10.0f); // Penalty for insufficient resources
        getDebugConsole().logOnce("Action", "Not enough resources to build.");
    }
}



// PrioritizeAction
void PrioritizeAction::perform(NPCEntity& player, Tile&) {
    // AI logic to prioritize actions dynamically
    if (player.getEnergy() < 20.0f) {
        player.addPenalty(-10.0f);
        getDebugConsole().log("Action", "NPC has low energy and needs to regenerate.");
    } else if (player.getInventorySize() >= player.getMaxInventorySize()) {
        player.addPenalty(-5.0f);
        getDebugConsole().log("Action", "NPC inventory is full; needs to store items.");
    } else {
        player.addReward(10.0f);
        getDebugConsole().log("Action", "NPC prioritized its actions successfully.");
    }
}

// IdleAction
void IdleAction::perform(NPCEntity& player, Tile&) {
    player.addPenalty(-20.0f); // Heavy penalty for idling
    getDebugConsole().log("Action", "NPC idled and lost rewards.");
}

// SpecialAction
SpecialAction::SpecialAction(const std::string& description, float reward, float penalty)
    : description(description), reward(reward), penalty(penalty) {}

void SpecialAction::perform(NPCEntity& player, Tile&) {
    if (reward > penalty) {
        player.addReward(reward);
        getDebugConsole().log("SpecialAction", description + " succeeded with reward: " + std::to_string(reward));
    } else {
        player.addPenalty(penalty);
        getDebugConsole().log("SpecialAction", description + " failed with penalty: " + std::to_string(penalty));
    }
}

// RestAction
void RestAction::perform(NPCEntity& player, Tile&) {
    if (player.getEnergy() < player.getMaxEnergy()) {
        player.setEnergy(player.getMaxEnergy()); // Fully restore energy
        getDebugConsole().log("Action", "NPC rested and restored energy.");
    } else {
        getDebugConsole().logOnce("Action", "Energy is already full. No need to rest.");
    }
}

// EvaluateStateAction
void EvaluateStateAction::perform(NPCEntity& player, Tile&) {
    if (player.getHunger() < 50.0f) {
        getDebugConsole().log("Action", "NPC is hungry and should gather food.");
    } else if (player.getEnergy() < 20.0f) {
        getDebugConsole().log("Action", "NPC is tired and needs to regenerate energy.");
    } else {
        getDebugConsole().log("Action", "NPC is in a balanced state.");
    }
}