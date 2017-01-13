#pragma once
#ifndef RESOURCES_H
#define RESOURCES_H
#include <map>
#include <unordered_map>

#include "constants.h"

#include "SubTypes.h"

class Resource
{
    public:
        enum ResourceType {
            ALL = -1,
            Ingredient = 0,
            Product = 1,
            Waste = 2
        };

        // convert type to string
        static std::string type_str(ResourceType type);

        static const ResourceType resource_type;
};

#define TYPE_MAP_CONVERSION(Rtype, Rlowertype) \
    static const std::map<SubType, std::string> type_map; \
SubType sub_type; \
static std::string type_to_string(Rtype::SubType type); \
static Rtype::SubType string_to_type(std::string string_type);

class Ingredient : public Resource
{
    public:
        static const ResourceType resource_type = Resource::ResourceType::Ingredient;

        using SubType = IngredientSubType;

        static const std::map<SubType, res_count_t> type_to_value;
        static const std::map<Ingredient::SubType, res_count_t> type_to_harvest_ratio;

        static const std::vector<SubType> basic_ingredients;
        static const std::vector<SubType> advanced_ingredients;


        TYPE_MAP_CONVERSION(Ingredient, ingredient);

        Ingredient(Ingredient::SubType sub_type) : Resource() {
            this->sub_type = sub_type;
        };

        Ingredient(const Ingredient& other) : Resource(other) {
            this->sub_type = other.sub_type;
        };

};

#define MAKE_MI_SUBTYPE(Cls)typedef std::map<Cls::SubType, res_count_t> mist##Cls
MAKE_MI_SUBTYPE(Ingredient);
#undef  MAKE_MI_SUBTYPE

#endif
