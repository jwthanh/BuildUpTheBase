#include "Item.h"
#include "Util.h"


Item::Item(std::string type_name,  RarityType rarity, res_count_t level)
    : type_name(type_name), rarity(rarity), level(level), Nameable("Unnamed"), scavenge_weight(0.0)
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
