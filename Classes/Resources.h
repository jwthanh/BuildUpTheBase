#pragma once
#ifndef RESOURCES_H
#define RESOURCES_H
#include "Nameable.h"
#include <map>

#include <string>

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

        enum SubType {
            None,
            Grain,
            PileOfGrain,
            Bread,
            Loaf,
            Seed,
            Tree,
            Iron,
            Copper,
            Wood,
            Fly,
            Sand,
            Flesh,
            Berry
        };


        TYPE_MAP_CONVERSION(Ingredient, ingredient);

        Ingredient(Ingredient::SubType sub_type) : Resource() {
            this->sub_type = sub_type;
        };

        Ingredient(const Ingredient& other) : Resource(other) {
            this->sub_type = other.sub_type;
        };

};

class Product : public Resource
{
    public:
        static const ResourceType resource_type = Resource::ResourceType::Product;

        enum SubType {
            None,
            Veggies,
            Meat,
            Dairy,
            Cereals,
            Sword,
            Shield
        };

        TYPE_MAP_CONVERSION(Product, product);

        Product(Product::SubType sub_type) : Resource() {
            this->sub_type = sub_type;
        };

        Product(const Product& other) : Resource(other) {
            this->sub_type = other.sub_type;
        };
};

class Waste : public Resource
{
    public:
        static const ResourceType resource_type = Resource::ResourceType::Waste;

        enum SubType {
            None,
            Corpse,
            Wasted_Iron
        };

        TYPE_MAP_CONVERSION(Waste, waste);


        Waste(Waste::SubType sub_type) : Resource() {
            this->sub_type = sub_type;
        };

        Waste(const Waste& other) : Resource(other) {
            this->sub_type = other.sub_type;
        };
};

typedef unsigned long long int res_count_t;

#define MAKE_MI_SUBTYPE(Cls)typedef std::map<Cls::SubType, res_count_t> mist##Cls
MAKE_MI_SUBTYPE(Ingredient);
MAKE_MI_SUBTYPE(Product);
MAKE_MI_SUBTYPE(Waste);
#undef  MAKE_MI_SUBTYPE

#endif
