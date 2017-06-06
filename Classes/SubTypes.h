#pragma once
#ifndef SUBTYPES_H
#define SUBTYPES_H

using res_count_t = long double;

enum class IngredientSubType {
    None, Grain, PileOfGrain, Bread, Loaf, Seed, Tree, Iron, Copper,
    Wood, Fly, Sand, Flesh, Berry, Soul, Blood, Paper, Undead, Minecart,
    MineRails
};

enum class ProductSubType {
    None,
    Veggies,
    Meat,
    Dairy,
    Cereals,
    Sword,
    Shield
};

enum class WasteSubType {
    None,
    Corpse,
    Wasted_Iron
};

enum class TechSubType
{
    None = -1,
    ClickDoublePower = 0,
    CombatDamage = 1,
    CombatArmor = 2,
    CombatCritChance = 3,
    CombatCritFactor = 4,
    SalesmenBaseBoost = 5,
    CombatWeakenEnemy = 6,
    RaiseWoodFind = 7,
    RaiseWalletCap = 8
};

enum class WorkerSubType
{
    ZERO = 0, //ZERO is unset
    One = 1, Two, Three, Four, Five, Six, Seven, Eight, Nine, Ten,
    Eleven, Twelve, Thirteen, Fourteen, Fifteen, Sixteen, Seventeen, Eighteen, Nineteen, Twenty
};

#endif