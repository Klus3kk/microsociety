#ifndef ACTIONS_HPP
#define ACTIONS_HPP

#include "Player.hpp"
#include "Tile.hpp"
#include <string>
#include "House.hpp"
#include "Market.hpp"
#include "debug.hpp"

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
    SellItem
};

// Base Action class
class Action {
public:
    virtual ~Action() = default;
    virtual void perform(PlayerEntity& player, Tile& tile) = 0; // Pure virtual function
    virtual std::string getActionName() const = 0;              // Name of the action
};

// TreeAction
class TreeAction : public Action {
public:
    void perform(PlayerEntity& player, Tile& tile) override;
    std::string getActionName() const override { return "Chop Tree"; }
};

// StoneAction
class StoneAction : public Action {
public:
    void perform(PlayerEntity& player, Tile& tile) override;
    std::string getActionName() const override { return "Mine Rock"; }
};

// BushAction
class BushAction : public Action {
public:
    void perform(PlayerEntity& player, Tile& tile) override;
    std::string getActionName() const override { return "Gather Bush"; }
};

// MoveAction
class MoveAction : public Action {
public:
    void perform(PlayerEntity& player, Tile&) override;
    std::string getActionName() const override { return "Move"; }
};

// TradeAction
class TradeAction : public Action {
public:
    void perform(PlayerEntity& player, Tile&) override;
    std::string getActionName() const override { return "Trade"; }
};

// RegenerateEnergyAction
class RegenerateEnergyAction : public Action {
public:
    void perform(PlayerEntity& player, Tile& tile) override;
    std::string getActionName() const override { return "Regenerate Energy"; }
};

// UpgradeHouseAction
class UpgradeHouseAction : public Action {
public:
    void perform(PlayerEntity& player, Tile& tile) override;
    std::string getActionName() const override { return "Upgrade House"; }
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
};

class TakeOutItemsAction : public Action {
private:
    std::string item;
    int quantity;

public:
    TakeOutItemsAction(const std::string& item, int quantity);
    void perform(PlayerEntity& player, Tile& tile) override;
    std::string getActionName() const override;
};

class BuyItemAction : public Action {
private:
    std::string item;
    int quantity;

public:
    BuyItemAction(const std::string& item, int quantity);
    void perform(PlayerEntity& player, Tile& tile) override;
    std::string getActionName() const override;
};

class SellItemAction : public Action {
private:
    std::string item;
    int quantity;

public:
    SellItemAction(const std::string& item, int quantity);
    void perform(PlayerEntity& player, Tile& tile) override;
    std::string getActionName() const override;
};

#endif

