#pragma once
#ifndef ITEM_H
#define ITEM_H

#include "constants.h"
#include "Nameable.h"

enum class ItemRarityType
{
    Unset = -1,
    Poor = 0,
    Normal = 1,
    Rare = 2
};

class Item : public Nameable
{
    private:
        res_count_t _base_cost;

    public:
        ItemRarityType rarity; //variation on item's base price
        res_count_t level; //affects base price
        double scavenge_weight; //how often it'll drop in the dump

        std::string type_name; //a Dagger is dagger, Ashen Mirror is ashen_mirror. used to pull data from static and save file

        std::string summary; //goes into nuitem
        std::string description; //goes into full body text

        std::string img_path; //full relative image path

        Item(std::string type_name, ItemRarityType rarity, res_count_t level); //builds static data off type_name in ItemData

        //base_cost doubled per level, multiplied by rarity, (cost * (2**level) * rarity)
        res_count_t get_effective_cost();

        //name, including rarity and level
        std::string get_name();

        friend class ItemData;

};

const std::map<ItemRarityType, float> ITEM_RARITY_MODIFIER = {
    { ItemRarityType::Poor, 0.85f },
    { ItemRarityType::Normal, 1.0f },
    { ItemRarityType::Rare, 1.15f }
};

const std::map<ItemRarityType, std::string> ITEM_RARITY_STRINGS = {
    { ItemRarityType::Poor, "Poor" },
    { ItemRarityType::Normal, "Norm" },
    { ItemRarityType::Rare, "Rare" }
};

#endif
