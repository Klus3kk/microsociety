#include "Actions.hpp"

// FIXED: Include complete type definitions needed for dynamic_cast
#include "NPCEntity.hpp"
#include "PlayerEntity.hpp"
#include "House.hpp"
#include "Market.hpp"

// FIXED: Changed all method signatures from NPCEntity& to Entity&

// Base class method is overridden by each action type
void TreeAction::perform(Entity& entity, Tile& tile, const std::vector<std::vector<std::unique_ptr<Tile>>>& tileMap) {
    // Check if the tile has an object (tree)
    if (tile.hasObject()) {
        // For NPCEntity, we can access specific methods
        if (auto* npc = dynamic_cast<NPCEntity*>(&entity)) {
            // Attempt to add wood to the NPC's inventory
            if (npc->addToInventory("wood", 1)) {
                tile.removeObject(); // Remove tree from the map
                npc->consumeEnergy(5.0f); // Reduce NPC's energy
                npc->receiveFeedback(10.0f, tileMap); // Reward for success
                getDebugConsole().log("TreeAction", "Tree chopped! Wood added to inventory.");
            } else {
                npc->receiveFeedback(-2.0f, tileMap); // Penalty if inventory is full
                getDebugConsole().logOnce("TreeAction", "Inventory full. Cannot chop tree.");
            }
        }
        // For PlayerEntity, we can access inventory methods differently
        else if (auto* player = dynamic_cast<PlayerEntity*>(&entity)) {
            if (player->addToInventory("wood", 1)) {
                tile.removeObject(); // Remove tree from the map
                player->consumeEnergy(5.0f); // Reduce player's energy
                getDebugConsole().log("TreeAction", "Tree chopped! Wood added to inventory.");
            } else {
                getDebugConsole().logOnce("TreeAction", "Inventory full. Cannot chop tree.");
            }
        }
    } else {
        // For NPCs, we can provide feedback
        if (auto* npc = dynamic_cast<NPCEntity*>(&entity)) {
            npc->receiveFeedback(-5.0f, tileMap); // Penalty if no tree is found
        }
        getDebugConsole().logOnce("TreeAction", "No tree to chop on this tile.");
    }
}

// StoneAction
void StoneAction::perform(Entity& entity, Tile& tile, const std::vector<std::vector<std::unique_ptr<Tile>>>& tileMap) {
    // Check if the tile has a rock object
    if (tile.hasObject() && tile.getObject()->getType() == ObjectType::Rock) {
        if (auto* npc = dynamic_cast<NPCEntity*>(&entity)) {
            // Attempt to add stone to inventory
            if (npc->addToInventory("stone", 1)) {
                tile.removeObject(); // Remove rock from the tile
                npc->consumeEnergy(5.0f);
                npc->receiveFeedback(10.0f, tileMap); // Reward for success
                getDebugConsole().log("StoneAction", "Rock mined! Stone added to inventory.");
            } else {
                npc->receiveFeedback(-2.0f, tileMap); // Penalty if inventory is full
                getDebugConsole().logOnce("StoneAction", "Inventory full. Cannot mine rock.");
            }
        } else if (auto* player = dynamic_cast<PlayerEntity*>(&entity)) {
            if (player->addToInventory("stone", 1)) {
                tile.removeObject(); // Remove rock from the tile
                player->consumeEnergy(5.0f);
                getDebugConsole().log("StoneAction", "Rock mined! Stone added to inventory.");
            } else {
                getDebugConsole().logOnce("StoneAction", "Inventory full. Cannot mine rock.");
            }
        }
    } else {
        if (auto* npc = dynamic_cast<NPCEntity*>(&entity)) {
            npc->receiveFeedback(-5.0f, tileMap); // Penalty if no rock found
        }
        getDebugConsole().logOnce("StoneAction", "No rock to mine on this tile.");
    }
}

// BushAction
void BushAction::perform(Entity& entity, Tile& tile, const std::vector<std::vector<std::unique_ptr<Tile>>>& tileMap) {
    // Check if the tile has a bush object
    if (tile.hasObject() && tile.getObject()->getType() == ObjectType::Bush) {
        if (auto* npc = dynamic_cast<NPCEntity*>(&entity)) {
            // Attempt to gather bush resource
            if (npc->addToInventory("bush", 1)) {
                tile.removeObject();
                npc->consumeEnergy(5.0f);
                npc->receiveFeedback(10.0f, tileMap);
                getDebugConsole().log("BushAction", "Bush gathered from tile!");
            } else {
                npc->receiveFeedback(-2.0f, tileMap);
                getDebugConsole().logOnce("BushAction", "Inventory full. Cannot gather bush.");
            }
        } else if (auto* player = dynamic_cast<PlayerEntity*>(&entity)) {
            if (player->addToInventory("bush", 1)) {
                tile.removeObject();
                player->consumeEnergy(5.0f);
                getDebugConsole().log("BushAction", "Bush gathered from tile!");
            } else {
                getDebugConsole().logOnce("BushAction", "Inventory full. Cannot gather bush.");
            }
        }
    } else {
        if (auto* npc = dynamic_cast<NPCEntity*>(&entity)) {
            npc->receiveFeedback(-5.0f, tileMap);
        }
        getDebugConsole().logOnce("BushAction", "No bush to gather on this tile.");
    }
}

// MoveAction
void MoveAction::perform(Entity& entity, Tile&, const std::vector<std::vector<std::unique_ptr<Tile>>>& tileMap) {
    if (auto* npc = dynamic_cast<NPCEntity*>(&entity)) {
        npc->consumeEnergy(1.0f); // Reduce energy per move
        npc->receiveFeedback(1.0f, tileMap); // Reward for movement
    } else if (auto* player = dynamic_cast<PlayerEntity*>(&entity)) {
        player->consumeEnergy(1.0f); // Reduce energy per move
    }
    getDebugConsole().log("Action", "Entity moved.");
}

// RegenerateEnergyAction
void RegenerateEnergyAction::perform(Entity& entity, Tile& tile, const std::vector<std::vector<std::unique_ptr<Tile>>>& tileMap) {
    // Check if a house exists on the tile
    if (tile.hasObject()) {
        if (auto house = dynamic_cast<House*>(tile.getObject())) {
            house->regenerateEnergy(entity); // Restore entity's energy
            if (auto* npc = dynamic_cast<NPCEntity*>(&entity)) {
                npc->receiveFeedback(5.0f, tileMap); // Reward for regeneration
            }
            getDebugConsole().log("Action", "Energy regenerated at house.");
        } else {
            if (auto* npc = dynamic_cast<NPCEntity*>(&entity)) {
                npc->receiveFeedback(-1.0f, tileMap); // Penalty if no house found
            }
            getDebugConsole().logOnce("Action", "No house found to regenerate energy.");
        }
    } else {
        if (auto* npc = dynamic_cast<NPCEntity*>(&entity)) {
            npc->receiveFeedback(-1.0f, tileMap);
        }
        getDebugConsole().logOnce("Action", "No object found on this tile.");
    }
}

// UpgradeHouseAction
void UpgradeHouseAction::perform(Entity& entity, Tile& tile, const std::vector<std::vector<std::unique_ptr<Tile>>>& tileMap) {
    // Ensure the tile has a house
    if (auto house = dynamic_cast<House*>(tile.getObject())) {
        // FIXED: Use proper money handling with temporary variable
        float entityMoney = entity.getMoney(); // Get current money value
        
        if (entityMoney >= house->getUpgradeCost()) {
            // Attempt to upgrade the house
            if (house->upgrade(entityMoney, entity)) {
                entity.setMoney(entityMoney); // Update the entity's money after upgrade
                if (auto* npc = dynamic_cast<NPCEntity*>(&entity)) {
                    npc->receiveFeedback(20.0f, tileMap); // Reward for success
                }
                getDebugConsole().log("Action", "House upgraded successfully.");
            } else {
                if (auto* npc = dynamic_cast<NPCEntity*>(&entity)) {
                    npc->receiveFeedback(-10.0f, tileMap); // Penalty for failure
                }
                getDebugConsole().logOnce("Action", "Upgrade failed due to insufficient resources.");
            }
        } else {
            if (auto* npc = dynamic_cast<NPCEntity*>(&entity)) {
                npc->receiveFeedback(-10.0f, tileMap); // Penalty for insufficient money
            }
            getDebugConsole().logOnce("Action", "Not enough money to upgrade the house.");
        }
    } else {
        if (auto* npc = dynamic_cast<NPCEntity*>(&entity)) {
            npc->receiveFeedback(-5.0f, tileMap);
        }
        getDebugConsole().logOnce("Action", "No house found to upgrade.");
    }
}

// StoreItemAction
StoreItemAction::StoreItemAction(const std::string& item, int quantity)
    : item(item), quantity(quantity) {}

void StoreItemAction::perform(Entity& entity, Tile& tile, const std::vector<std::vector<std::unique_ptr<Tile>>>& tileMap) {
    if (auto house = dynamic_cast<House*>(tile.getObject())) {
        // For NPCs, check their inventory
        if (auto* npc = dynamic_cast<NPCEntity*>(&entity)) {
            const auto& inventory = npc->getInventory();
            if (inventory.empty()) {
                npc->receiveFeedback(-5.0f, tileMap); // Penalty for empty inventory
                getDebugConsole().logOnce("Action", "No items in inventory to store.");
                return;
            }

            if (inventory.count(item) > 0 && inventory.at(item) >= quantity) {
                // Attempt to store the item in the house
                if (house->storeItem(item, quantity)) {
                    npc->removeFromInventory(item, quantity);
                    npc->receiveFeedback(5.0f, tileMap); // Reward for storing
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
        // For players, we could implement similar logic
        else if (auto* player = dynamic_cast<PlayerEntity*>(&entity)) {
            const auto& inventory = player->getInventory();
            if (inventory.count(item) > 0 && inventory.at(item) >= quantity) {
                if (house->storeItem(item, quantity)) {
                    player->removeFromInventory(item, quantity);
                    getDebugConsole().log("Action", "Stored " + std::to_string(quantity) + " " + item + " in the house.");
                } else {
                    getDebugConsole().logOnce("Action", "House storage is full! Could not store all items.");
                }
            } else {
                getDebugConsole().logOnce("Action", "Insufficient " + item + " in inventory.");
            }
        }
    } else {
        if (auto* npc = dynamic_cast<NPCEntity*>(&entity)) {
            npc->receiveFeedback(-5.0f, tileMap);
        }
        getDebugConsole().logOnce("Action", "No house present on this tile.");
    }
}

std::string StoreItemAction::getActionName() const {
    return "Store Items in House";
}

// BuyItemAction
BuyItemAction::BuyItemAction(const std::string& item, int quantity)
    : item(item), quantity(quantity) {}

void BuyItemAction::perform(Entity& entity, Tile& tile, const std::vector<std::vector<std::unique_ptr<Tile>>>& tileMap) {
    // Ensure a market exists on the tile
    if (auto market = dynamic_cast<Market*>(tile.getObject())) {
        if (market->buyItem(entity, item, quantity)) {
            if (auto* npc = dynamic_cast<NPCEntity*>(&entity)) {
                npc->receiveFeedback(10.0f, tileMap);
            }
            getDebugConsole().log("Action", "Bought " + std::to_string(quantity) + " " + item + " from the market.");
        } else {
            if (auto* npc = dynamic_cast<NPCEntity*>(&entity)) {
                npc->receiveFeedback(-5.0f, tileMap);
            }
            getDebugConsole().logOnce("Action", "Failed to buy items. Not enough money or stock.");
        }
    } else {
        if (auto* npc = dynamic_cast<NPCEntity*>(&entity)) {
            npc->receiveFeedback(-5.0f, tileMap);
        }
        getDebugConsole().logOnce("Action", "No market present on this tile.");
    }
}

std::string BuyItemAction::getActionName() const {
    return "Buy Items from Market";
}

void SellItemAction::perform(Entity& entity, Tile& tile, const std::vector<std::vector<std::unique_ptr<Tile>>>& tileMap) {
    // Ensure a market exists on the tile
    if (auto market = dynamic_cast<Market*>(tile.getObject())) {
        // Attempt to sell items
        if (market->sellItem(entity, item, quantity)) {
            if (auto* npc = dynamic_cast<NPCEntity*>(&entity)) {
                npc->receiveFeedback(10.0f, tileMap); // Reward for successful sale
            }
            getDebugConsole().log("Action", "Sold " + std::to_string(quantity) + " " + item + " to the market.");
        } else {
            if (auto* npc = dynamic_cast<NPCEntity*>(&entity)) {
                npc->receiveFeedback(-5.0f, tileMap); // Penalty if not enough items in inventory
            }
            getDebugConsole().logOnce("Action", "Failed to sell items. Not enough in inventory.");
        }
    } else {
        if (auto* npc = dynamic_cast<NPCEntity*>(&entity)) {
            npc->receiveFeedback(-5.0f, tileMap); // Penalty if no market exists
        }
        getDebugConsole().logOnce("Action", "No market present on this tile.");
    }
}

SellItemAction::SellItemAction(const std::string& item, int quantity)
    : item(item), quantity(quantity) {}

std::string SellItemAction::getActionName() const {
    return "Sell Items to Market";
}

// ExploreAction
void ExploreAction::perform(Entity& entity, Tile&, const std::vector<std::vector<std::unique_ptr<Tile>>>& tileMap) {
    // Exploration consumes some energy but provides small rewards
    if (auto* npc = dynamic_cast<NPCEntity*>(&entity)) {
        npc->consumeEnergy(2.0f);
        npc->receiveFeedback(5.0f, tileMap); // Reward for exploration
    } else if (auto* player = dynamic_cast<PlayerEntity*>(&entity)) {
        player->consumeEnergy(2.0f);
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
    if (auto* npc = dynamic_cast<NPCEntity*>(&entity)) {
        npc->receiveFeedback(-20.0f, tileMap); // Large penalty for idling
    }
    getDebugConsole().log("Action", "Entity idled and lost rewards.");
}

// RestAction
void RestAction::perform(Entity& entity, Tile&, const std::vector<std::vector<std::unique_ptr<Tile>>>& tileMap) {
    // Ensure entity is not already at full energy
    if (entity.getEnergy() < GameConfig::MAX_ENERGY) {
        entity.setEnergy(GameConfig::MAX_ENERGY); // Fully restore energy
        if (auto* npc = dynamic_cast<NPCEntity*>(&entity)) {
            npc->receiveFeedback(5.0f, tileMap); // Reward for resting
        }
        getDebugConsole().log("Action", "Entity rested and restored energy.");
    } else {
        if (auto* npc = dynamic_cast<NPCEntity*>(&entity)) {
            npc->receiveFeedback(-1.0f, tileMap); // Penalty for unnecessary rest
        }
        getDebugConsole().logOnce("Action", "Energy is already full. No need to rest.");
    }
}