#pragma once
#ifndef RESOURCES_H
#define RESOURCES_H
#include "Nameable.h"

#include <string>
#include <map>

class Resource : public Nameable
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

        Resource(std::string name) :Nameable(name){};
};

#define TYPE_MAP_CONVERSION(Rtype, Rlowertype) \
    static const std::map<Rtype##Type, std::string> type_map; \
Rtype##Type Rlowertype##_type; \
static std::string type_to_string(Rtype::Rtype##Type type); \
static Rtype::Rtype##Type string_to_type(std::string string_type); 

class Ingredient : public Resource
{
    public:
        static const ResourceType resource_type = Resource::ResourceType::Ingredient;

        enum IngredientType {
            None,
            Grain,
            PileOfGrain,
            Bread,
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

        Ingredient(std::string name) : Resource(name) {
            this->ingredient_type = Ingredient::string_to_type(name);
        };

        Ingredient(const Ingredient& other) : Resource(other) {
            this->ingredient_type = other.ingredient_type;
        };

};

class Product : public Resource
{
    public:
        static const ResourceType resource_type = Resource::ResourceType::Product;

        enum ProductType {
            None,
            Veggies,
            Meat,
            Dairy,
            Cereals,
            Sword,
            Shield
        };

        TYPE_MAP_CONVERSION(Product, product);

        Product(std::string name) : Resource(name) {
            this->product_type = Product::string_to_type(name);
        };

        Product(const Product& other) : Resource(other) {
            this->product_type = other.product_type;
        };
};

class Waste : public Resource
{
    public:
        static const ResourceType resource_type = Resource::ResourceType::Waste;

        enum WasteType {
            None,
            Corpse,
            Wasted_Iron
        };

        TYPE_MAP_CONVERSION(Waste, waste);


        Waste(std::string name) : Resource(name) {
            this->waste_type = Waste::string_to_type(name);
        };

        Waste(const Waste& other) : Resource(other) {
            this->waste_type = other.waste_type;
        };
};

#endif
