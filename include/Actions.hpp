#ifndef ACTIONS_HPP
#define ACTIONS_HPP

#include "NPCEntity.hpp"
#include "Tile.hpp"
#include "House.hpp"
#include "Market.hpp"
#include "debug.hpp"
#include <string>
#include <memory> // For std::make_unique
#include "ActionType.hpp"

class NPCEntity;

// Base class for all actions, implementing polymorphism and abstraction
class Action {
public:
    virtual ~Action() = default;

    // Declares an abstract method, forcing derived classes to implement it
    virtual void perform(NPCEntity& player, Tile& tile, const std::vector<std::vector<std::unique_ptr<Tile>>>& tileMap) = 0;

    // Return the name of the action
    virtual std::string getActionName() const = 0;

    // Return the reward for performing this action
    virtual float getReward() const { return 0.0f; }

    // Return the penalty for performing this action, if applicable
    virtual float getPenalty() const { return 0.0f; }
};


// Action for chopping down trees
class TreeAction : public Action { 
public:
    // Polymorphism 
    void perform(NPCEntity& player, Tile& tile, const std::vector<std::vector<std::unique_ptr<Tile>>>& tileMap) override;
    std::string getActionName() const override { return "Chop Tree"; }
    float getReward() const override { return 10.0f; }
};


// Action for mining rocks
class StoneAction : public Action {
public:
    void perform(NPCEntity& player, Tile& tile, const std::vector<std::vector<std::unique_ptr<Tile>>>& tileMap) override;
    std::string getActionName() const override { return "Mine Rock"; }
    float getReward() const override { return 8.0f; }
};


// Action for gathering bushes
class BushAction : public Action {
public:
    void perform(NPCEntity& player, Tile& tile, const std::vector<std::vector<std::unique_ptr<Tile>>>& tileMap) override;
    std::string getActionName() const override { return "Gather Bush"; }
    float getReward() const override { return 5.0f; }
};


// Action for moving across the map
class MoveAction : public Action {
public:
    void perform(NPCEntity& player, Tile&, const std::vector<std::vector<std::unique_ptr<Tile>>>& tileMap) override;
    std::string getActionName() const override { return "Move"; }
    float getPenalty() const override { return -1.0f; }
};


// Action for regenerating energy
class RegenerateEnergyAction : public Action {
public:
    void perform(NPCEntity& player, Tile& tile, const std::vector<std::vector<std::unique_ptr<Tile>>>& tileMap) override;
    std::string getActionName() const override { return "Regenerate Energy"; }
    float getReward() const override { return 5.0f; }
};


// Action for upgrading a house
class UpgradeHouseAction : public Action {
public:
    void perform(NPCEntity& player, Tile& tile, const std::vector<std::vector<std::unique_ptr<Tile>>>& tileMap) override;
    std::string getActionName() const override { return "Upgrade House"; }
    float getReward() const override { return 20.0f; }
};

// Action for storing items in a house
class StoreItemAction : public Action {
private:
    std::string item;
    int quantity;

public:
    StoreItemAction(const std::string& item, int quantity);
    void perform(NPCEntity& player, Tile& tile, const std::vector<std::vector<std::unique_ptr<Tile>>>& tileMap) override;
    std::string getActionName() const override;
    float getReward() const override { return 5.0f; }
};

// class TakeOutItemsAction : public Action {
// private:
//     std::string item;
//     int quantity;

// public:
//     TakeOutItemsAction(const std::string& item, int quantity);
//     void perform(NPCEntity& player, Tile& tile, const std::vector<std::vector<std::unique_ptr<Tile>>>& tileMap) override;
//     std::string getActionName() const override;
//     float getPenalty() const override { return -2.0f; }
// };


// Action for buying items from the market
class BuyItemAction : public Action {
private:
    std::string item;
    int quantity;

public:
    BuyItemAction(const std::string& item, int quantity); // Encapsulation (hiding item and quantity variables), access only through controlled methods
    void perform(NPCEntity& player, Tile& tile, const std::vector<std::vector<std::unique_ptr<Tile>>>& tileMap) override;
    std::string getActionName() const override;
    float getReward() const override { return 10.0f; }
};


// Action for selling items at the market
class SellItemAction : public Action {
private:
    std::string item;
    int quantity;

public:
    SellItemAction(const std::string& item, int quantity);
    void perform(NPCEntity& player, Tile& tile, const std::vector<std::vector<std::unique_ptr<Tile>>>& tileMap) override;
    std::string getActionName() const override;
    float getReward() const override { return 12.0f; }
};


// Action for resting
class RestAction : public Action {
public:
    void perform(NPCEntity& player, Tile&, const std::vector<std::vector<std::unique_ptr<Tile>>>& tileMap) override;
    std::string getActionName() const override { return "Rest"; }
    float getPenalty() const override { return -5.0f; }
};


// Action for exploring new areas
class ExploreAction : public Action {
public:
    void perform(NPCEntity& player, Tile&, const std::vector<std::vector<std::unique_ptr<Tile>>>& tileMap) override;
    std::string getActionName() const override { return "Explore"; }
    float getReward() const override { return 5.0f; }
};

// Idle
class IdleAction : public Action {
public:
    void perform(NPCEntity& player, Tile&, const std::vector<std::vector<std::unique_ptr<Tile>>>& tileMap) override;
    std::string getActionName() const override { return "Idle"; }
    float getPenalty() const override { return -20.0f; }
};

// PrioritizeAction
class PrioritizeAction : public Action {
public:
    void perform(NPCEntity& player, Tile&, const std::vector<std::vector<std::unique_ptr<Tile>>>& tileMap) override;
    std::string getActionName() const override { return "Prioritize"; }
    float getReward() const override { return 10.0f; }
    float getPenalty() const override { return -10.0f; }
};

#endif
