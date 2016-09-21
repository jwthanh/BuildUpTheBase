#include "Item.h"

Item::Item(std::string name, std::string description, res_count_t base_cost, RarityType rarity, res_count_t level)
    : Nameable(name), rarity(rarity), level(level), _base_cost(base_cost), description(description)
{
};

const std::map<RarityType, float> ITEM_RARITY_MODIFIER = {
    { RarityType::Poor, 0.85f },
    { RarityType::Normal, 1.0f },
    { RarityType::Rare, 1.15f }
};

res_count_t Item::get_effective_cost()
{
    res_count_t total_cost = this->_base_cost * std::pow(2, this->level);

    total_cost *= ITEM_RARITY_MODIFIER.at(this->rarity);

    return total_cost;
};
