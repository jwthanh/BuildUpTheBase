#include "Item.h"
#include "Util.h"

const std::map<RarityType, float> ITEM_RARITY_MODIFIER = {
    { RarityType::Poor, 0.85f },
    { RarityType::Normal, 1.0f },
    { RarityType::Rare, 1.15f }
};

const std::map<RarityType, std::string> ITEM_RARITY_STRINGS = {
    { RarityType::Poor, "Poor" },
    { RarityType::Normal, "Norm" },
    { RarityType::Rare, "Rare" }
};

Item::Item(std::string name, std::string summary, std::string description, res_count_t base_cost, RarityType rarity, res_count_t level)
    : Nameable(name), rarity(rarity), level(level), _base_cost(base_cost), summary(summary), description(description)
{
};

res_count_t Item::get_effective_cost()
{
    res_count_t total_cost = this->_base_cost * std::pow(2, this->level);

    total_cost *= ITEM_RARITY_MODIFIER.at(this->rarity);

    return total_cost;
};

std::string Item::get_name()
{
    std::stringstream name;
    //Normal
    if (this->rarity != RarityType::Normal)
        name << ITEM_RARITY_STRINGS.at(this->rarity) << " ";
    //Dagger
    name << this->name << " ";
    //(LV 1)
    name << "LV" << beautify_double(this->level);
    return name.str();
};
