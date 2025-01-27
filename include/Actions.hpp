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

// Base Action class
class Action {
public:
    virtual ~Action() = default;

    // Perform action
    virtual void perform(NPCEntity& player, Tile& tile, const std::vector<std::vector<std::unique_ptr<Tile>>>& tileMap) = 0;

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
    void perform(NPCEntity& player, Tile& tile, const std::vector<std::vector<std::unique_ptr<Tile>>>& tileMap) override;
    std::string getActionName() const override { return "Chop Tree"; }
    float getReward() const override { return 10.0f; }
};

// StoneAction
class StoneAction : public Action {
public:
    void perform(NPCEntity& player, Tile& tile, const std::vector<std::vector<std::unique_ptr<Tile>>>& tileMap) override;
    std::string getActionName() const override { return "Mine Rock"; }
    float getReward() const override { return 8.0f; }
};

// BushAction
class BushAction : public Action {
public:
    void perform(NPCEntity& player, Tile& tile, const std::vector<std::vector<std::unique_ptr<Tile>>>& tileMap) override;
    std::string getActionName() const override { return "Gather Bush"; }
    float getReward() const override { return 5.0f; }
};

// MoveAction
class MoveAction : public Action {
public:
    void perform(NPCEntity& player, Tile&, const std::vector<std::vector<std::unique_ptr<Tile>>>& tileMap) override;
    std::string getActionName() const override { return "Move"; }
    float getPenalty() const override { return -1.0f; }
};

// TradeAction
class TradeAction : public Action {
public:
    void perform(NPCEntity& player, Tile&, const std::vector<std::vector<std::unique_ptr<Tile>>>& tileMap) override;
    std::string getActionName() const override { return "Trade"; }
    float getReward() const override { return 15.0f; }
};

// RegenerateEnergyAction
class RegenerateEnergyAction : public Action {
public:
    void perform(NPCEntity& player, Tile& tile, const std::vector<std::vector<std::unique_ptr<Tile>>>& tileMap) override;
    std::string getActionName() const override { return "Regenerate Energy"; }
    float getReward() const override { return 5.0f; }
};

// UpgradeHouseAction
class UpgradeHouseAction : public Action {
public:
    void perform(NPCEntity& player, Tile& tile, const std::vector<std::vector<std::unique_ptr<Tile>>>& tileMap) override;
    std::string getActionName() const override { return "Upgrade House"; }
    float getReward() const override { return 20.0f; }
};

// StoreItemAction
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

// TakeOutItemsAction
class TakeOutItemsAction : public Action {
private:
    std::string item;
    int quantity;

public:
    TakeOutItemsAction(const std::string& item, int quantity);
    void perform(NPCEntity& player, Tile& tile, const std::vector<std::vector<std::unique_ptr<Tile>>>& tileMap) override;
    std::string getActionName() const override;
    float getPenalty() const override { return -2.0f; }
};

// BuyItemAction
class BuyItemAction : public Action {
private:
    std::string item;
    int quantity;

public:
    BuyItemAction(const std::string& item, int quantity);
    void perform(NPCEntity& player, Tile& tile, const std::vector<std::vector<std::unique_ptr<Tile>>>& tileMap) override;
    std::string getActionName() const override;
    float getReward() const override { return 10.0f; }
};

// SellItemAction
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

// RestAction
class RestAction : public Action {
public:
    void perform(NPCEntity& player, Tile&, const std::vector<std::vector<std::unique_ptr<Tile>>>& tileMap) override;
    std::string getActionName() const override { return "Rest"; }
    float getPenalty() const override { return -5.0f; }
};

// EvaluateStateAction
class EvaluateStateAction : public Action {
public:
    void perform(NPCEntity& player, Tile&, const std::vector<std::vector<std::unique_ptr<Tile>>>& tileMap) override;
    std::string getActionName() const override { return "Evaluate State"; }
    float getReward() const override { return 0.0f; }
};

// ExploreAction
class ExploreAction : public Action {
public:
    void perform(NPCEntity& player, Tile&, const std::vector<std::vector<std::unique_ptr<Tile>>>& tileMap) override;
    std::string getActionName() const override { return "Explore"; }
    float getReward() const override { return 5.0f; }
};

// BuildAction
class BuildAction : public Action {
public:
    void perform(NPCEntity& player, Tile& tile, const std::vector<std::vector<std::unique_ptr<Tile>>>& tileMap) override;
    std::string getActionName() const override { return "Build"; }
    float getReward() const override { return 20.0f; }
};

// SpecialAction
class SpecialAction : public Action {
private:
    std::string description;
    float reward;
    float penalty;

public:
    SpecialAction(const std::string& description, float reward, float penalty);
    void perform(NPCEntity& player, Tile&, const std::vector<std::vector<std::unique_ptr<Tile>>>& tileMap) override;
    std::string getActionName() const override { return "Special Action"; }
    float getReward() const override { return reward; }
    float getPenalty() const override { return penalty; }
};

// IdleAction
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
