#include "Resources.h"

#include <algorithm>
#include <cassert>

#include "constants.h"

#include "cocos2d.h"

const std::map<Ingredient::IngredientType, std::string> Ingredient::type_map = {
    {Ingredient::Grain, "grain"},
    {Ingredient::PileOfGrain, "pileofgrain"},
    {Ingredient::Seed, "seed"},
    {Ingredient::Wood, "wood"},
    {Ingredient::Iron, "iron"},
    {Ingredient::Copper, "copper"},
    {Ingredient::Wood, "wood"},
    {Ingredient::Fly, "fly"},
    {Ingredient::Sand, "sand"},
    {Ingredient::Flesh, "flesh"},
    {Ingredient::Berry, "berry"}
};

const std::map<Product::ProductType, std::string> Product::type_map = {
    {Product::Veggies, "veggies"},
    {Product::Meat, "meat"},
    {Product::Dairy, "dairy"},
    {Product::Cereals, "cereals"},
    {Product::Sword, "sword"},
    {Product::Shield, "shield"},
    {Product::Meat, "meat"},
};

const std::map<Waste::WasteType, std::string> Waste::type_map = {
    {Waste::Corpse, "corpse"},
    {Waste::Wasted_Iron, "wasted_iron"},
};

#define type_stuff(ResType)std::string ResType::type_to_string(ResType::ResType##Type type) \
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
ResType::ResType##Type ResType::string_to_type(std::string string_type) \
{ \
    ResType::ResType##Type result = ResType::ResType##Type::None; \
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
