#ifndef ACTION_TYPE_HPP
#define ACTION_TYPE_HPP

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
    Rest,
    EvaluateState,
    Explore,
    Build,
    SpecialAction,
    Idle
};

#endif // ACTION_TYPE_HPP
