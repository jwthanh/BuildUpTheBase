#pragma once
#ifndef RESOURCES_H
#define RESOURCES_H
#include <map>

#include "constants.h"

#include "SubTypes.h"

struct Ingredient
{
    using SubType = IngredientSubType;

    static const std::map<SubType, res_count_t> type_to_value;
    static const std::map<Ingredient::SubType, res_count_t> type_to_harvest_ratio;

    static const std::vector<SubType> basic_ingredients;
    static const std::vector<SubType> advanced_ingredients;

    static const std::map<SubType, std::string> type_map;
    static std::string type_to_string(Ingredient::SubType type);
    static Ingredient::SubType string_to_type(std::string string_type);

};

#endif
