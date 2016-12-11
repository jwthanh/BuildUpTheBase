#include "Item.h"
#include "Util.h"
#include <sstream>


Item::Item(std::string type_name,  ItemRarityType rarity, res_count_t level)
    : type_name(type_name), rarity(rarity), slot_type(ItemSlotType::Unset), level(level), Nameable("Unnamed"), scavenge_weight(0.0)
{
};

res_count_t Item::get_effective_cost()
{
    res_count_t total_cost = scale_number_slow(this->_base_cost, this->level, 3.3L);

    total_cost *= ITEM_RARITY_MODIFIER.at(this->rarity);

    return total_cost;
};

std::string Item::get_name()
{
    std::stringstream name;
    //Normal
    if (this->rarity != ItemRarityType::Normal)
        name << ITEM_RARITY_STRINGS.at(this->rarity) << " ";
    //Dagger
    name << this->name << " ";
    //(LV 1)
    name << "LV" << beautify_double(this->level);
    return name.str();
};
