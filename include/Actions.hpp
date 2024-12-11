#ifndef ACTIONS_HPP
#define ACTIONS_HPP

#include "NPCEntity.hpp"
#include "Tile.hpp"
#include "House.hpp"
#include "Market.hpp"
#include "debug.hpp"
#include <string>

enum class ActionType {
    None,
    Move,
    ChopTree,
    MineRock,
    GatherBush,
    StoreItem,
    UpgradeHouse,
    RegenerateEnergy,
    TakeOutItems,
    BuyItem,
    SellItem,
    Rest,               // New action for idle/rest behavior
    EvaluateState       // New action for AI evaluation and decision-making
};

// Base Action class
class Action {
public:
    virtual ~Action() = default;

    // Perform action
    virtual void perform(PlayerEntity& player, Tile& tile) = 0; 

    // Name of the action
    virtual std::string getActionName() const = 0;

    // Reward for performing this action
    virtual float getReward() const { return 0.0f; } 

    // Penalty for this action if applicable
    virtual float getPenalty() const { return 0.0f; }
};

// TreeAction
class TreeAction : public Action {
public:
    void perform(PlayerEntity& player, Tile& tile) override;
    std::string getActionName() const override { return "Chop Tree"; }
    float getReward() const override { return 10.0f; } // Reward for gathering resources
};

// StoneAction
class StoneAction : public Action {
public:
    void perform(PlayerEntity& player, Tile& tile) override;
    std::string getActionName() const override { return "Mine Rock"; }
    float getReward() const override { return 8.0f; } // Slightly lower reward than chopping trees
};

// BushAction
class BushAction : public Action {
public:
    void perform(PlayerEntity& player, Tile& tile) override;
    std::string getActionName() const override { return "Gather Bush"; }
    float getReward() const override { return 5.0f; } // Smaller reward
};

// MoveAction
class MoveAction : public Action {
public:
    void perform(PlayerEntity& player, Tile&) override;
    std::string getActionName() const override { return "Move"; }
    float getPenalty() const override { return -1.0f; } // Small energy cost for moving
};

// TradeAction
class TradeAction : public Action {
public:
    void perform(PlayerEntity& player, Tile&) override;
    std::string getActionName() const override { return "Trade"; }
    float getReward() const override { return 15.0f; } // Reward for trading successfully
};

// RegenerateEnergyAction
class RegenerateEnergyAction : public Action {
public:
    void perform(PlayerEntity& player, Tile& tile) override;
    std::string getActionName() const override { return "Regenerate Energy"; }
    float getReward() const override { return 5.0f; } // Positive reward for staying alive
};

// UpgradeHouseAction
class UpgradeHouseAction : public Action {
public:
    void perform(PlayerEntity& player, Tile& tile) override;
    std::string getActionName() const override { return "Upgrade House"; }
    float getReward() const override { return 20.0f; } // Large reward for upgrades
};

// StoreItemAction
class StoreItemAction : public Action {
private:
    std::string item;
    int quantity;

public:
    StoreItemAction(const std::string& item, int quantity);
    void perform(PlayerEntity& player, Tile& tile) override;
    std::string getActionName() const override;
    float getReward() const override { return 5.0f; } // Reward for managing resources
};

// TakeOutItemsAction
class TakeOutItemsAction : public Action {
private:
    std::string item;
    int quantity;

public:
    TakeOutItemsAction(const std::string& item, int quantity);
    void perform(PlayerEntity& player, Tile& tile) override;
    std::string getActionName() const override;
    float getPenalty() const override { return -2.0f; } // Slight penalty for taking resources
};

// BuyItemAction
class BuyItemAction : public Action {
private:
    std::string item;
    int quantity;

public:
    BuyItemAction(const std::string& item, int quantity);
    void perform(PlayerEntity& player, Tile& tile) override;
    std::string getActionName() const override;
    float getReward() const override { return 10.0f; } // Reward for acquiring needed items
};

// SellItemAction
class SellItemAction : public Action {
private:
    std::string item;
    int quantity;

public:
    SellItemAction(const std::string& item, int quantity);
    void perform(PlayerEntity& player, Tile& tile) override;
    std::string getActionName() const override;
    float getReward() const override { return 12.0f; } // Reward for earning money
};

// RestAction for idling or skipping a turn
class RestAction : public Action {
public:
    void perform(PlayerEntity& player, Tile&) override;
    std::string getActionName() const override { return "Rest"; }
    float getPenalty() const override { return -5.0f; } // Penalty for wasting time
};

// EvaluateStateAction for AI evaluation and planning
class EvaluateStateAction : public Action {
public:
    void perform(PlayerEntity& player, Tile&) override;
    std::string getActionName() const override { return "Evaluate State"; }
    float getReward() const override { return 0.0f; } // Neutral reward
};

#endif
