#ifndef ACTIONS_HPP
#define ACTIONS_HPP

#include <memory> 
#include <string>

#include "Entity.hpp"  
#include "Tile.hpp"
#include "debug.hpp"
#include "ActionType.hpp"

class NPCEntity;
class House;
class Market;

class Action {
public:
    virtual ~Action() = default; // for cleanup of actions 

    // perform the action on the given entity and tile
    virtual void perform(Entity& entity, Tile& tile, const std::vector<std::vector<std::unique_ptr<Tile>>>& tileMap) = 0;

    // getters for action name, reward, and penalty
    virtual std::string getActionName() const = 0;
    virtual float getReward() const { return 0.0f; }
    virtual float getPenalty() const { return 0.0f; }
};

// base class for resource gathering actions (trees, rocks, bushes)
class ResourceGatherAction : public Action {
protected:
    std::string resourceName;
    std::string actionName;
    ObjectType expectedObjectType;
    float energyCost;
    float rewardValue;
    float healthCost;

    // template method pattern - subclasses configure parameters
    virtual void performGather(Entity& entity, Tile& tile, const std::vector<std::vector<std::unique_ptr<Tile>>>& tileMap);

public:
    ResourceGatherAction(const std::string& resource, const std::string& action, 
                        ObjectType objectType, float energy, float reward, float health = 0.0f)
        : resourceName(resource), actionName(action), expectedObjectType(objectType),
          energyCost(energy), rewardValue(reward), healthCost(health) {}

    void perform(Entity& entity, Tile& tile, const std::vector<std::vector<std::unique_ptr<Tile>>>& tileMap) override {
        performGather(entity, tile, tileMap);
    }

    std::string getActionName() const override { return actionName; }
    float getReward() const override { return rewardValue; }
};

// action for chopping trees
class TreeAction : public ResourceGatherAction { 
public:
    TreeAction() : ResourceGatherAction("wood", "Chop Tree", ObjectType::Tree, 1.0f, 10.0f) {}
};

// action for mining rocks
class StoneAction : public ResourceGatherAction {
public:
    StoneAction() : ResourceGatherAction("stone", "Mine Rock", ObjectType::Rock, 5.0f, 10.0f) {}
};

// action for gathering bushes
class BushAction : public ResourceGatherAction {
public:
    BushAction() : ResourceGatherAction("bush", "Gather Bush", ObjectType::Bush, 5.0f, 10.0f) {}
};

// action for moving across the map
class MoveAction : public Action {
public:
    void perform(Entity& entity, Tile&, const std::vector<std::vector<std::unique_ptr<Tile>>>& tileMap) override;

    std::string getActionName() const override { return "Move"; }
    float getPenalty() const override { return -1.0f; }
};

// action for regenerating energy
class RegenerateEnergyAction : public Action {
public:
    void perform(Entity& entity, Tile& tile, const std::vector<std::vector<std::unique_ptr<Tile>>>& tileMap) override;

    std::string getActionName() const override { return "Regenerate Energy"; }
    float getReward() const override { return 5.0f; }
};

// action for upgrading a house
class UpgradeHouseAction : public Action {
public:
    void perform(Entity& entity, Tile& tile, const std::vector<std::vector<std::unique_ptr<Tile>>>& tileMap) override;

    std::string getActionName() const override { return "Upgrade House"; }
    float getReward() const override { return 20.0f; }
};

// action for storing items in a house
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

// action for buying items from the market
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

// action for selling items at the market
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

// action for resting
class RestAction : public Action {
public:
    void perform(Entity& entity, Tile&, const std::vector<std::vector<std::unique_ptr<Tile>>>& tileMap) override;

    std::string getActionName() const override { return "Rest"; }
    float getPenalty() const override { return -5.0f; }
};

// action for exploring new areas
class ExploreAction : public Action {
public:
    void perform(Entity& entity, Tile&, const std::vector<std::vector<std::unique_ptr<Tile>>>& tileMap) override;

    std::string getActionName() const override { return "Explore"; }
    float getReward() const override { return 5.0f; }
};

// action for idling
class IdleAction : public Action {
public:
    void perform(Entity& entity, Tile&, const std::vector<std::vector<std::unique_ptr<Tile>>>& tileMap) override;

    std::string getActionName() const override { return "Idle"; }
    float getPenalty() const override { return -20.0f; }
};

// action for prioritizing
class PrioritizeAction : public Action {
public:
    void perform(Entity& entity, Tile&, const std::vector<std::vector<std::unique_ptr<Tile>>>& tileMap) override;

    std::string getActionName() const override { return "Prioritize"; }
    float getReward() const override { return 10.0f; }
    float getPenalty() const override { return -10.0f; }
};

// action for producing goods
class ProduceAction : public Action {
public:
    void perform(Entity& entity, Tile& tile, const std::vector<std::vector<std::unique_ptr<Tile>>>& tileMap) override;
    std::string getActionName() const override { return "Produce Goods"; }
    float getReward() const override { return 15.0f; }
};

// action for producing water
class ProduceWaterActions: public ProduceAction {
public:
    std::string getActionName() const override { return "Produce Water"; }
    float getReward() const override { return 12.0f; }
};

// action for producing food
class ProduceFoodActions: public ProduceAction {
public:
    std::string getActionName() const override { return "Produce Food"; }
    float getReward() const override { return 18.0f; }
};

// action for breead with NPC
class BreedAction : public Action {
public:
    std::string getActionName() const override { return "Breed with NPC"; }
    float getReward() const override { return 20.0f; }
};

// action for consuming goods
class ConsumeAction : public Action {
public:
    void perform(Entity& entity, Tile& tile, const std::vector<std::vector<std::unique_ptr<Tile>>>& tileMap) override;
    std::string getActionName() const override { return "Consume Goods"; }
    float getReward() const override { return 10.0f; }
};

// action for upgrading 
class UpgradeAction : public Action {
public:
    void perform(Entity& entity, Tile& tile, const std::vector<std::vector<std::unique_ptr<Tile>>>& tileMap) override;
    std::string getActionName() const override { return "Upgrade Entity"; }
    float getReward() const override { return 25.0f; }
};

// action for investing money
class InvestAction : public Action {
public:
    void perform(Entity& entity, Tile& tile, const std::vector<std::vector<std::unique_ptr<Tile>>>& tileMap) override;
    std::string getActionName() const override { return "Invest Money"; }
    float getReward() const override { return 30.0f; }
};


class TalkAction : public Action {
public:
    void perform(Entity& entity, Tile& tile, const std::vector<std::vector<std::unique_ptr<Tile>>>& tileMap) override;
    std::string getActionName() const override { return "Talk to NPC"; }
    float getReward() const override { return 15.0f; }
};



#endif
