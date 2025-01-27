#include "Actions.hpp"

// TreeAction
void TreeAction::perform(NPCEntity& player, Tile& tile, const std::vector<std::vector<std::unique_ptr<Tile>>>& tileMap) {
    if (tile.hasObject() && player.addToInventory("wood", 1)) {
        tile.removeObject();
        player.consumeEnergy(5.0f); // Zużywa 5 energii
        player.receiveFeedback(10.0f, tileMap); // Nagroda za zebranie drewna
        getDebugConsole().log("Action", "Tree chopped! Wood added to inventory.");
    } else if (!tile.hasObject()) {
        player.receiveFeedback(-5.0f, tileMap); // Kara za nieprawidłową akcję
        getDebugConsole().logOnce("Action", "No tree to chop on this tile.");
    } else {
        player.receiveFeedback(-2.0f, tileMap); // Kara za pełny ekwipunek
        getDebugConsole().logOnce("Action", "Failed to chop tree. Inventory is full.");
    }
}

// StoneAction
void StoneAction::perform(NPCEntity& player, Tile& tile, const std::vector<std::vector<std::unique_ptr<Tile>>>& tileMap) {
    if (tile.hasObject() && tile.getObject()->getType() == ObjectType::Rock && player.addToInventory("stone", 1)) {
        tile.removeObject();
        player.consumeEnergy(5.0f);
        player.receiveFeedback(10.0f, tileMap);
        getDebugConsole().log("Action", "Rock mined! Stone added to inventory.");
    } else if (!tile.hasObject()) {
        player.receiveFeedback(-5.0f, tileMap);
        getDebugConsole().logOnce("Action", "No rock to mine on this tile.");
    } else {
        player.receiveFeedback(-2.0f, tileMap);
        getDebugConsole().logOnce("Action", "Failed to mine rock. Inventory is full.");
    }
}

// BushAction
void BushAction::perform(NPCEntity& player, Tile& tile, const std::vector<std::vector<std::unique_ptr<Tile>>>& tileMap) {
    if (tile.hasObject() && tile.getObject()->getType() == ObjectType::Bush && player.addToInventory("food", 1)) {
        tile.removeObject();
        player.consumeEnergy(5.0f);
        player.receiveFeedback(10.0f, tileMap);
        getDebugConsole().log("Action", "Food gathered from bush!");
    } else if (!tile.hasObject()) {
        player.receiveFeedback(-5.0f, tileMap);
        getDebugConsole().logOnce("Action", "No bush to gather food from on this tile.");
    } else {
        player.receiveFeedback(-2.0f, tileMap);
        getDebugConsole().logOnce("Action", "Failed to gather food. Inventory is full.");
    }
}

// MoveAction
void MoveAction::perform(NPCEntity& player, Tile&, const std::vector<std::vector<std::unique_ptr<Tile>>>& tileMap) {
    player.consumeEnergy(1.0f); // Zużycie energii za ruch
    player.receiveFeedback(1.0f, tileMap); // Nagroda za ruch (przykład)
    getDebugConsole().log("Action", "Player moved.");
}

// TradeAction
void TradeAction::perform(NPCEntity& player, Tile&, const std::vector<std::vector<std::unique_ptr<Tile>>>& tileMap) {
    player.receiveFeedback(5.0f, tileMap); // Przykładowa nagroda za handel
    getDebugConsole().log("Action", "Trade action performed.");
}

// RegenerateEnergyAction
void RegenerateEnergyAction::perform(NPCEntity& player, Tile& tile, const std::vector<std::vector<std::unique_ptr<Tile>>>& tileMap) {
    if (auto house = dynamic_cast<House*>(tile.getObject())) {
        house->regenerateEnergy(player); // Regeneracja energii w domu
        player.receiveFeedback(5.0f, tileMap); // Nagroda za regenerację energii
        getDebugConsole().log("Action", "Energy regenerated at house.");
    } else {
        player.receiveFeedback(-1.0f, tileMap); // Kara za nieprawidłową akcję
        getDebugConsole().logOnce("Action", "No house found to regenerate energy.");
    }
}


// UpgradeHouseAction
void UpgradeHouseAction::perform(NPCEntity& player, Tile& tile, const std::vector<std::vector<std::unique_ptr<Tile>>>& tileMap) {
    if (auto house = dynamic_cast<House*>(tile.getObject())) {
        float& npcMoney = player.getMoney(); // Create a reference to NPC money
        if (npcMoney >= house->getUpgradeCost()) {
            if (house->upgrade(npcMoney, player)) {
                player.receiveFeedback(20.0f, tileMap); // Reward for successful upgrade
                getDebugConsole().log("Action", "House upgraded successfully.");
            } else {
                player.receiveFeedback(-10.0f, tileMap); // Penalty for failed upgrade
                getDebugConsole().logOnce("Action", "Upgrade failed due to insufficient resources.");
            }
        } else {
            player.receiveFeedback(-10.0f, tileMap); // Penalty for insufficient money
            getDebugConsole().logOnce("Action", "Not enough money to upgrade the house.");
        }
    } else {
        player.receiveFeedback(-5.0f, tileMap); // Penalty for no house
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
            player.receiveFeedback(-5.0f, tileMap); // Kara za pusty ekwipunek
            getDebugConsole().logOnce("Action", "No items in inventory to store.");
            return;
        }

        if (inventory.count(item) > 0 && inventory.at(item) >= quantity) {
            if (house->storeItem(item, quantity)) {
                player.removeFromInventory(item, quantity); // Usuwamy przedmiot z ekwipunku
                player.receiveFeedback(5.0f, tileMap); // Nagroda za przechowanie
                getDebugConsole().log("Action", "Stored " + std::to_string(quantity) + " " + item + " in the house.");
            } else {
                player.receiveFeedback(-5.0f, tileMap); // Kara za pełny magazyn
                getDebugConsole().logOnce("Action", "House storage is full! Could not store all items.");
            }
        } else {
            player.receiveFeedback(-5.0f, tileMap); // Kara za brak wystarczającej ilości
            getDebugConsole().logOnce("Action", "Insufficient " + item + " in inventory.");
        }
    } else {
        player.receiveFeedback(-5.0f, tileMap); // Kara za brak domu
        getDebugConsole().logOnce("Action", "No house present on this tile.");
    }
}

std::string StoreItemAction::getActionName() const {
    return "Store Items in House";
}

// TakeOutItemsAction
TakeOutItemsAction::TakeOutItemsAction(const std::string& item, int quantity)
    : item(item), quantity(quantity) {}

void TakeOutItemsAction::perform(NPCEntity& player, Tile& tile, const std::vector<std::vector<std::unique_ptr<Tile>>>& tileMap) {
    if (auto house = dynamic_cast<House*>(tile.getObject())) {
        if (house->takeFromStorage(item, quantity, player)) {
            player.receiveFeedback(10.0f, tileMap); // Nagroda za pobranie przedmiotów
            getDebugConsole().log("Action", "Took " + std::to_string(quantity) + " " + item + " from the house.");
        } else {
            player.receiveFeedback(-5.0f, tileMap); // Kara za brak przedmiotów w magazynie
            getDebugConsole().logOnce("Action", "Failed to take items. Not enough in storage.");
        }
    } else {
        player.receiveFeedback(-5.0f, tileMap); // Kara za brak domu
        getDebugConsole().logOnce("Action", "No house present on this tile.");
    }
}


std::string TakeOutItemsAction::getActionName() const {
    return "Take Out Items from House";
}

// BuyItemAction
BuyItemAction::BuyItemAction(const std::string& item, int quantity)
    : item(item), quantity(quantity) {}

void BuyItemAction::perform(NPCEntity& player, Tile& tile, const std::vector<std::vector<std::unique_ptr<Tile>>>& tileMap) {
    if (auto market = dynamic_cast<Market*>(tile.getObject())) {
        if (market->buyItem(player, item, quantity)) {
            player.receiveFeedback(10.0f, tileMap); // Nagroda za zakup
            getDebugConsole().log("Action", "Bought " + std::to_string(quantity) + " " + item + " from the market.");
        } else {
            player.receiveFeedback(-5.0f, tileMap); // Kara za nieudany zakup
            getDebugConsole().logOnce("Action", "Failed to buy items. Not enough money or stock.");
        }
    } else {
        player.receiveFeedback(-5.0f, tileMap); // Kara za brak rynku
        getDebugConsole().logOnce("Action", "No market present on this tile.");
    }
}


std::string BuyItemAction::getActionName() const {
    return "Buy Items from Market";
}

void SellItemAction::perform(NPCEntity& player, Tile& tile, const std::vector<std::vector<std::unique_ptr<Tile>>>& tileMap) {
    if (auto market = dynamic_cast<Market*>(tile.getObject())) {
        if (market->sellItem(player, item, quantity)) {
            player.receiveFeedback(10.0f, tileMap); // Nagroda za sprzedaż
            getDebugConsole().log("Action", "Sold " + std::to_string(quantity) + " " + item + " to the market.");
        } else {
            player.receiveFeedback(-5.0f, tileMap); // Kara za nieudaną sprzedaż
            getDebugConsole().logOnce("Action", "Failed to sell items. Not enough in inventory.");
        }
    } else {
        player.receiveFeedback(-5.0f, tileMap); // Kara za brak rynku
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
    player.receiveFeedback(5.0f, tileMap); // Użycie `tileMap` dla spójności z NPCEntity
    getDebugConsole().log("Action", "NPC explored the map.");
}

// BuildAction
void BuildAction::perform(NPCEntity& player, Tile& tile, const std::vector<std::vector<std::unique_ptr<Tile>>>& tileMap) {
    if (tile.hasObject()) {
        player.receiveFeedback(-10.0f, tileMap); // Kara za próbę budowy na zajętym kafelku
        getDebugConsole().logOnce("Action", "Tile is occupied. Cannot build here.");
        return;
    }

    if (player.getInventoryItemCount("wood") >= 5 && player.getInventoryItemCount("stone") >= 3) {
        player.removeFromInventory("wood", 5);
        player.removeFromInventory("stone", 3);

        auto& textureManager = TextureManager::getInstance();
        const sf::Texture& texture = textureManager.getTexture("house1", "../assets/objects/house1.png");

        auto newHouse = std::make_unique<House>(texture, 1); // Przekazanie tekstury i poziomu
        tile.placeObject(std::move(newHouse));

        player.receiveFeedback(20.0f, tileMap); // Nagroda za budowę
        getDebugConsole().log("Action", "NPC built a structure on the tile.");
    } else {
        player.receiveFeedback(-10.0f, tileMap); // Kara za brak zasobów
        getDebugConsole().logOnce("Action", "Not enough resources to build.");
    }
}

// PrioritizeAction
void PrioritizeAction::perform(NPCEntity& player, Tile&, const std::vector<std::vector<std::unique_ptr<Tile>>>& tileMap) {
    // AI logic to prioritize actions dynamically
    if (player.getEnergy() < 20.0f) {
        player.receiveFeedback(-10.0f, tileMap); // Kara za niski poziom energii
        getDebugConsole().log("Action", "NPC has low energy and needs to regenerate.");
    } else if (player.getInventorySize() >= player.getMaxInventorySize()) {
        player.receiveFeedback(-5.0f, tileMap); // Kara za pełny ekwipunek
        getDebugConsole().log("Action", "NPC inventory is full; needs to store items.");
    } else {
        player.receiveFeedback(10.0f, tileMap); // Nagroda za efektywne priorytetyzowanie
        getDebugConsole().log("Action", "NPC prioritized its actions successfully.");
    }
}

// IdleAction
void IdleAction::perform(NPCEntity& player, Tile&, const std::vector<std::vector<std::unique_ptr<Tile>>>& tileMap) {
    player.receiveFeedback(-20.0f, tileMap); // Duża kara za bezczynność
    getDebugConsole().log("Action", "NPC idled and lost rewards.");
}

// SpecialAction
SpecialAction::SpecialAction(const std::string& description, float reward, float penalty)
    : description(description), reward(reward), penalty(penalty) {}

void SpecialAction::perform(NPCEntity& player, Tile&, const std::vector<std::vector<std::unique_ptr<Tile>>>& tileMap) {
    if (reward > penalty) {
        player.receiveFeedback(reward, tileMap); // Nagroda za sukces akcji specjalnej
        getDebugConsole().log("SpecialAction", description + " succeeded with reward: " + std::to_string(reward));
    } else {
        player.receiveFeedback(-penalty, tileMap); // Kara za porażkę
        getDebugConsole().log("SpecialAction", description + " failed with penalty: " + std::to_string(penalty));
    }
}

// RestAction
void RestAction::perform(NPCEntity& player, Tile&, const std::vector<std::vector<std::unique_ptr<Tile>>>& tileMap) {
    if (player.getEnergy() < player.getMaxEnergy()) {
        player.setEnergy(player.getMaxEnergy()); // Pełne przywrócenie energii
        player.receiveFeedback(5.0f, tileMap); // Nagroda za odpoczynek
        getDebugConsole().log("Action", "NPC rested and restored energy.");
    } else {
        player.receiveFeedback(-1.0f, tileMap); // Kara za próbę odpoczynku przy pełnej energii
        getDebugConsole().logOnce("Action", "Energy is already full. No need to rest.");
    }
}

// EvaluateStateAction
void EvaluateStateAction::perform(NPCEntity& player, Tile&, const std::vector<std::vector<std::unique_ptr<Tile>>>& tileMap) {
    if (player.getHunger() < 50.0f) {
        player.receiveFeedback(-2.0f, tileMap); // Kara za niski poziom głodu
        getDebugConsole().log("Action", "NPC is hungry and should gather food.");
    } else if (player.getEnergy() < 20.0f) {
        player.receiveFeedback(-2.0f, tileMap); // Kara za niski poziom energii
        getDebugConsole().log("Action", "NPC is tired and needs to regenerate energy.");
    } else {
        player.receiveFeedback(2.0f, tileMap); // Nagroda za zrównoważony stan
        getDebugConsole().log("Action", "NPC is in a balanced state.");
    }
}
