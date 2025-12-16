#ifndef ACTIONS_HPP
#define ACTIONS_HPP

#include "Entity.hpp"  
#include "Tile.hpp"
#include "debug.hpp"
#include <string>
#include <memory> 
#include "ActionType.hpp"

// Forward declarations only - complete type definitions will be in .cpp file
class NPCEntity;
class House;
class Market;

// Base class for all actions, implementing polymorphism and abstraction
class Action {
public:
    virtual ~Action() = default;

    // FIXED: Declares an abstract method using Entity base class
    virtual void perform(Entity& entity, Tile& tile, const std::vector<std::vector<std::unique_ptr<Tile>>>& tileMap) = 0;

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
    void perform(Entity& entity, Tile& tile, const std::vector<std::vector<std::unique_ptr<Tile>>>& tileMap) override;
    std::string getActionName() const override { return "Chop Tree"; }
    float getReward() const override { return 10.0f; }
};

// Action for mining rocks
class StoneAction : public Action {
public:
    void perform(Entity& entity, Tile& tile, const std::vector<std::vector<std::unique_ptr<Tile>>>& tileMap) override;
    std::string getActionName() const override { return "Mine Rock"; }
    float getReward() const override { return 8.0f; }
};

// Action for gathering bushes
class BushAction : public Action {
public:
    void perform(Entity& entity, Tile& tile, const std::vector<std::vector<std::unique_ptr<Tile>>>& tileMap) override;
    std::string getActionName() const override { return "Gather Bush"; }
    float getReward() const override { return 5.0f; }
};

// Action for moving across the map
class MoveAction : public Action {
public:
    void perform(Entity& entity, Tile&, const std::vector<std::vector<std::unique_ptr<Tile>>>& tileMap) override;
    std::string getActionName() const override { return "Move"; }
    float getPenalty() const override { return -1.0f; }
};

// Action for regenerating energy
class RegenerateEnergyAction : public Action {
public:
    void perform(Entity& entity, Tile& tile, const std::vector<std::vector<std::unique_ptr<Tile>>>& tileMap) override;
    std::string getActionName() const override { return "Regenerate Energy"; }
    float getReward() const override { return 5.0f; }
};

// Action for upgrading a house
class UpgradeHouseAction : public Action {
public:
    void perform(Entity& entity, Tile& tile, const std::vector<std::vector<std::unique_ptr<Tile>>>& tileMap) override;
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
    void perform(Entity& entity, Tile& tile, const std::vector<std::vector<std::unique_ptr<Tile>>>& tileMap) override;
    std::string getActionName() const override;
    float getReward() const override { return 5.0f; }
};

// Action for buying items from the market
class BuyItemAction : public Action {
private:
    std::string item;
    int quantity;

public:
    BuyItemAction(const std::string& item, int quantity); 
    void perform(Entity& entity, Tile& tile, const std::vector<std::vector<std::unique_ptr<Tile>>>& tileMap) override;
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
    void perform(Entity& entity, Tile& tile, const std::vector<std::vector<std::unique_ptr<Tile>>>& tileMap) override;
    std::string getActionName() const override;
    float getReward() const override { return 12.0f; }
};

// Action for resting
class RestAction : public Action {
public:
    void perform(Entity& entity, Tile&, const std::vector<std::vector<std::unique_ptr<Tile>>>& tileMap) override;
    std::string getActionName() const override { return "Rest"; }
    float getPenalty() const override { return -5.0f; }
};

// Action for exploring new areas
class ExploreAction : public Action {
public:
    void perform(Entity& entity, Tile&, const std::vector<std::vector<std::unique_ptr<Tile>>>& tileMap) override;
    std::string getActionName() const override { return "Explore"; }
    float getReward() const override { return 5.0f; }
};

// Idle
class IdleAction : public Action {
public:
    void perform(Entity& entity, Tile&, const std::vector<std::vector<std::unique_ptr<Tile>>>& tileMap) override;
    std::string getActionName() const override { return "Idle"; }
    float getPenalty() const override { return -20.0f; }
};

// PrioritizeAction
class PrioritizeAction : public Action {
public:
    void perform(Entity& entity, Tile&, const std::vector<std::vector<std::unique_ptr<Tile>>>& tileMap) override;
    std::string getActionName() const override { return "Prioritize"; }
    float getReward() const override { return 10.0f; }
    float getPenalty() const override { return -10.0f; }
};

#endif
