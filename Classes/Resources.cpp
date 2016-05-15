#include "Resources.h"

#include <algorithm>
#include <cassert>

#include "constants.h"

#include "cocos2d.h"

const std::map<Ingredient::SubType, std::string> Ingredient::type_map = {
    {Ingredient::SubType::Grain, "grain"},
    {Ingredient::SubType::PileOfGrain, "pileofgrain"},
    {Ingredient::SubType::Bread, "bread"},
    {Ingredient::SubType::Loaf, "loaf"},
    {Ingredient::SubType::Seed, "seed"},
    {Ingredient::SubType::Wood, "wood"},
    {Ingredient::SubType::Iron, "iron"},
    {Ingredient::SubType::Copper, "copper"},
    {Ingredient::SubType::Wood, "wood"},
    {Ingredient::SubType::Fly, "fly"},
    {Ingredient::SubType::Sand, "sand"},
    {Ingredient::SubType::Flesh, "flesh"},
    {Ingredient::SubType::Berry, "berry"},
    {Ingredient::SubType::Soul, "soul"},
    {Ingredient::SubType::Blood, "blood"}
};

const std::map<Product::SubType, std::string> Product::type_map = {
    {Product::SubType::Veggies, "veggies"},
    {Product::SubType::Meat, "meat"},
    {Product::SubType::Dairy, "dairy"},
    {Product::SubType::Cereals, "cereals"},
    {Product::SubType::Sword, "sword"},
    {Product::SubType::Shield, "shield"},
    {Product::SubType::Meat, "meat"},
};

const std::map<Waste::SubType, std::string> Waste::type_map = {
    {Waste::SubType::Corpse, "corpse"},
    {Waste::SubType::Wasted_Iron, "wasted_iron"},
};

#define type_stuff(ResType)std::string ResType::type_to_string(ResType::SubType type) \
{ \
    std::string result = "none"; \
    for (auto pair : ResType::type_map) \
        { \
        if (type == pair.first) \
                { \
            return pair.second; \
                } \
        } \
 \
    return result; \
}; \
 \
ResType::SubType ResType::string_to_type(std::string string_type) \
{ \
    ResType::SubType result = ResType::SubType::None; \
    std::transform(string_type.begin(), string_type.end(), string_type.begin(), ::tolower); \
    for (auto pair : ResType::type_map) \
        { \
        if (pair.second == string_type) \
                { \
            return pair.first; \
                } \
        } \
    printj("type id " << string_type); \
    assert(false && "unknown type"); \
 \
    return result; \
};

type_stuff(Ingredient);
type_stuff(Product);
type_stuff(Waste);

std::string Resource::type_str(ResourceType type)
{
    auto mapping = std::map<ResourceType, std::string>();
    mapping[Ingredient] = "Ingredient";
    mapping[ResourceType::Product] = "Product";
    mapping[ResourceType::Waste] = "Waste";

    return mapping[type];
}
