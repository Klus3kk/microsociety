#include "Actions.hpp"
#include "NPCEntity.hpp"
#include "House.hpp"
#include "Market.hpp"

// Helper functions to reduce code duplication

// Apply feedback to NPC if entity is an NPC
inline void applyNPCFeedback(Entity& entity, float feedback, const std::vector<std::vector<std::unique_ptr<Tile>>>& tileMap) {
    if (auto* npc = dynamic_cast<NPCEntity*>(&entity)) {
        npc->receiveFeedback(feedback, tileMap);
    }
}

// Check if tile has a market and return pointer
inline Market* getMarketFromTile(Tile& tile) {
    if (tile.hasObject() && tile.getObject()->getType() == ObjectType::Market) {
        return dynamic_cast<Market*>(tile.getObject());
    }
    return nullptr;
}

// Check if tile has a house and return pointer
inline House* getHouseFromTile(Tile& tile) {
    if (tile.hasObject()) {
        return dynamic_cast<House*>(tile.getObject());
    }
    return nullptr;
}

// Common implementation for resource gathering actions
void ResourceGatherAction::performGather(Entity& entity, Tile& tile, const std::vector<std::vector<std::unique_ptr<Tile>>>& tileMap) {
    auto* npc = dynamic_cast<NPCEntity*>(&entity);

    // Check if tile has the expected object type
    bool hasCorrectObject = tile.hasObject() && 
                           (expectedObjectType == ObjectType::Tree || 
                            tile.getObject()->getType() == expectedObjectType);
    
    if (!hasCorrectObject) {
        if (npc) {
            float penalty = (expectedObjectType == ObjectType::Tree) ? -2.0f : -5.0f;
            npc->receiveFeedback(penalty, tileMap);
        }
        if (tile.hasObject() || expectedObjectType != ObjectType::Tree) {
            getDebugConsole().logOnce(actionName, "No " + resourceName + " to gather on this tile.");
        }
        return;
    }

    if (!npc) return;

    // Try to add resource to inventory
    if (npc->addToInventory(resourceName, 1)) {
        tile.removeObject();
        npc->consumeEnergy(energyCost);
        npc->receiveFeedback(rewardValue, tileMap);
        
        // Track items gathered
        npc->incrementItemsGathered(resourceName, 1);
        
        getDebugConsole().log(actionName, npc->getName() + " gathered " + resourceName + "! Total gathered: " + 
                            std::to_string(npc->getTotalItemsGathered()));
    } else {
        npc->receiveFeedback(-2.0f, tileMap);
        getDebugConsole().log(actionName, npc->getName() + " inventory full. Cannot gather " + resourceName + ".");
    }
}

// MoveAction
void MoveAction::perform(Entity& entity, Tile&, const std::vector<std::vector<std::unique_ptr<Tile>>>& tileMap) {
    if (auto* npc = dynamic_cast<NPCEntity*>(&entity)) {
        npc->consumeEnergy(1.0f);
        npc->receiveFeedback(1.0f, tileMap);
    }
    getDebugConsole().log("Action", "Entity moved.");
}

// RegenerateEnergyAction
void RegenerateEnergyAction::perform(Entity& entity, Tile& tile, const std::vector<std::vector<std::unique_ptr<Tile>>>& tileMap) {
    auto* house = getHouseFromTile(tile);
    
    if (!house) {
        applyNPCFeedback(entity, -1.0f, tileMap);
        getDebugConsole().logOnce("Action", tile.hasObject() ? "No house found to regenerate energy." : "No object found on this tile.");
        return;
    }

    house->regenerateEnergy(entity);
    applyNPCFeedback(entity, 5.0f, tileMap);
    getDebugConsole().log("Action", "Energy regenerated at house.");
}

// UpgradeHouseAction
void UpgradeHouseAction::perform(Entity& entity, Tile& tile, const std::vector<std::vector<std::unique_ptr<Tile>>>& tileMap) {
    auto* house = getHouseFromTile(tile);
    
    if (!house) {
        applyNPCFeedback(entity, -5.0f, tileMap);
        getDebugConsole().logOnce("Action", "No house found to upgrade.");
        return;
    }

    float entityMoney = entity.getMoney();
    
    if (entityMoney < house->getUpgradeCost()) {
        applyNPCFeedback(entity, -10.0f, tileMap);
        getDebugConsole().logOnce("Action", "Not enough money to upgrade the house.");
        return;
    }

    if (house->upgrade(entityMoney, entity)) {
        entity.setMoney(entityMoney);
        applyNPCFeedback(entity, 20.0f, tileMap);
        getDebugConsole().log("Action", "House upgraded successfully.");
    } else {
        applyNPCFeedback(entity, -10.0f, tileMap);
        getDebugConsole().logOnce("Action", "Upgrade failed due to insufficient resources.");
    }
}

// StoreItemAction
StoreItemAction::StoreItemAction(const std::string& item, int quantity)
    : item(item), quantity(quantity) {}

void StoreItemAction::perform(Entity& entity, Tile& tile, const std::vector<std::vector<std::unique_ptr<Tile>>>& tileMap) {
    auto* house = getHouseFromTile(tile);
    auto* npc = dynamic_cast<NPCEntity*>(&entity);
    
    if (!house) {
        applyNPCFeedback(entity, -5.0f, tileMap);
        getDebugConsole().logOnce("Action", "No house present on this tile.");
        return;
    }

    if (!npc) return;

    const auto& inventory = npc->getInventory();
    if (inventory.empty()) {
        npc->receiveFeedback(-5.0f, tileMap);
        getDebugConsole().logOnce("Action", "No items in inventory to store.");
        return;
    }

    if (inventory.count(item) > 0 && inventory.at(item) >= quantity) {
        if (house->storeItem(item, quantity)) {
            npc->removeFromInventory(item, quantity);
            npc->receiveFeedback(5.0f, tileMap);
            getDebugConsole().log("Action", "Stored " + std::to_string(quantity) + " " + item + " in the house.");
        } else {
            npc->receiveFeedback(-5.0f, tileMap);
            getDebugConsole().logOnce("Action", "House storage is full! Could not store all items.");
        }
    } else {
        npc->receiveFeedback(-5.0f, tileMap);
        getDebugConsole().logOnce("Action", "Insufficient " + item + " in inventory.");
    }
}

std::string StoreItemAction::getActionName() const {
    return "Store Items in House";
}

// BuyItemAction
BuyItemAction::BuyItemAction(const std::string& item, int quantity)
    : item(item), quantity(quantity) {}

void BuyItemAction::perform(Entity& entity, Tile& tile, const std::vector<std::vector<std::unique_ptr<Tile>>>& tileMap) {
    auto* market = getMarketFromTile(tile);
    
    if (!market) {
        applyNPCFeedback(entity, -5.0f, tileMap);
        getDebugConsole().logOnce("Action", "No market present on this tile.");
        return;
    }

    if (market->buyItem(entity, item, quantity)) {
        applyNPCFeedback(entity, 10.0f, tileMap);
        getDebugConsole().log("Action", "Bought " + std::to_string(quantity) + " " + item + " from the market.");
    } else {
        applyNPCFeedback(entity, -5.0f, tileMap);
        getDebugConsole().logOnce("Action", "Failed to buy items. Not enough money or stock.");
    }
}

std::string BuyItemAction::getActionName() const {
    return "Buy Items from Market";
}

void SellItemAction::perform(Entity& entity, Tile& tile, const std::vector<std::vector<std::unique_ptr<Tile>>>& tileMap) {
    auto* market = getMarketFromTile(tile);
    
    if (!market) {
        applyNPCFeedback(entity, -5.0f, tileMap);
        getDebugConsole().logOnce("Action", "No market present on this tile.");
        return;
    }

    if (market->sellItem(entity, item, quantity)) {
        applyNPCFeedback(entity, 10.0f, tileMap);
        getDebugConsole().log("Action", "Sold " + std::to_string(quantity) + " " + item + " to the market.");
    } else {
        applyNPCFeedback(entity, -5.0f, tileMap);
        getDebugConsole().logOnce("Action", "Failed to sell items. Not enough in inventory.");
    }
}

SellItemAction::SellItemAction(const std::string& item, int quantity)
    : item(item), quantity(quantity) {}

std::string SellItemAction::getActionName() const {
    return "Sell Items to Market";
}

// ExploreAction
void ExploreAction::perform(Entity& entity, Tile&, const std::vector<std::vector<std::unique_ptr<Tile>>>& tileMap) {
    if (auto* npc = dynamic_cast<NPCEntity*>(&entity)) {
        npc->consumeEnergy(2.0f);
        npc->receiveFeedback(5.0f, tileMap);
    }
    getDebugConsole().log("Action", "Entity explored the map.");
}

// PrioritizeAction
void PrioritizeAction::perform(Entity& entity, Tile&, const std::vector<std::vector<std::unique_ptr<Tile>>>& tileMap) {
    // AI-based logic to prioritize actions dynamically
    if (auto* npc = dynamic_cast<NPCEntity*>(&entity)) {
        if (npc->getEnergy() < 20.0f) {
            npc->receiveFeedback(-10.0f, tileMap); // Penalty for low energy
            getDebugConsole().log("Action", "NPC has low energy and needs to regenerate.");
        } else if (npc->getInventorySize() >= npc->getMaxInventorySize()) {
            npc->receiveFeedback(-5.0f, tileMap); // Penalty for full inventory
            getDebugConsole().log("Action", "NPC inventory is full; needs to store items.");
        } else {
            npc->receiveFeedback(10.0f, tileMap); // Reward for efficient prioritization
            getDebugConsole().log("Action", "NPC prioritized its actions successfully.");
        }
    }
}

// IdleAction
void IdleAction::perform(Entity& entity, Tile&, const std::vector<std::vector<std::unique_ptr<Tile>>>& tileMap) {
    applyNPCFeedback(entity, -20.0f, tileMap);
    getDebugConsole().log("Action", "Entity idled and lost rewards.");
}

// RestAction
void RestAction::perform(Entity& entity, Tile&, const std::vector<std::vector<std::unique_ptr<Tile>>>& tileMap) {
    if (entity.getEnergy() < GameConfig::MAX_ENERGY) {
        entity.setEnergy(GameConfig::MAX_ENERGY);
        applyNPCFeedback(entity, 5.0f, tileMap);
        getDebugConsole().log("Action", "Entity rested and restored energy.");
    } else {
        applyNPCFeedback(entity, -1.0f, tileMap);
        getDebugConsole().logOnce("Action", "Energy is already full. No need to rest.");
    }
}
