#include "Resources.h"

#include <algorithm>
#include <cassert>
#include <sstream>

#include "constants.h"

const std::map<Ingredient::SubType, std::string> Ingredient::type_map = {
    {Ingredient::SubType::Grain, "grain"},
    {Ingredient::SubType::PileOfGrain, "pileofgrain"},
    {Ingredient::SubType::Bread, "bread"},
    {Ingredient::SubType::Loaf, "loaf"},
    {Ingredient::SubType::Seed, "seed"},
    {Ingredient::SubType::Wood, "wood"},
    {Ingredient::SubType::Iron, "iron"},
    {Ingredient::SubType::Copper, "copper"},
    {Ingredient::SubType::Fly, "fly"},
    {Ingredient::SubType::Sand, "sand"},
    {Ingredient::SubType::Flesh, "flesh"},
    {Ingredient::SubType::Berry, "berry"},
    {Ingredient::SubType::Soul, "soul"},
    {Ingredient::SubType::Blood, "blood"},
    {Ingredient::SubType::Paper, "paper"},
    {Ingredient::SubType::Undead, "undead"},
    {Ingredient::SubType::Minecart, "minecart"},
    {Ingredient::SubType::MineRails, "minerails"}
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
    for (auto& pair : ResType::type_map) \
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

std::string Ingredient::type_to_string(Ingredient::SubType type)
{
    std::string result = "none";
    for (auto pair : Ingredient::type_map)
    {
        if (type == pair.first) { return pair.second; }
    }
    return result;
};

Ingredient::SubType Ingredient::string_to_type(std::string string_type)
{
    Ingredient::SubType result = Ingredient::SubType::None;
    std::transform(string_type.begin(), string_type.end(), string_type.begin(), ::tolower);
    for (auto& pair : Ingredient::type_map)
    {
        if (pair.second == string_type)
        {
            return pair.first;
        }
    }

    assert(false && "unknown type"); return result;
};

std::string Resource::type_str(ResourceType type)
{
    auto mapping = std::map<ResourceType, std::string>();
    mapping[Ingredient] = "Ingredient";

    return mapping[type];
}
