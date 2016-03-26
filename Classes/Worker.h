#pragma once
#ifndef WORKER_H
#define WORKER_H

#include "HouseBuilding.h"

template<typename Rtype>
class ResourceCondition : public Nameable
{
    public:
        Resource::ResourceType type_choice;
        Rtype choice;

        int quantity;

        ResourceCondition(Resource::ResourceType type_choice, Rtype chosen_resource, int quantity, std::string name) : Nameable(name) {
            this->type_choice;
            this->choice = chosen_resource;
            this->quantity = quantity;
        };

        bool is_satisfied(spBuilding building) {
            if (this->type_choice == Resource::Ingredient) {
                return (int)building->ingredients.size() >= this->quantity;
            }
            else if (this->type_choice == Resource::Product) {
                return (int)building->products.size() >= this->quantity;
            }
            else if (this->type_choice == Resource::Ingredient) {
                return (int)building->wastes.size() >= this->quantity;
            }
            else {
                unsigned int size = building->ingredients.size() +
                    building->products.size() +
                    building->wastes.size();
                return (int)size >= this->quantity;
            };
        };
};

typedef ResourceCondition<Ingredient::IngredientType> IngredientCondition;
#define INGREDIENT_CONDITION(ingredient_enum, quantity, name) IngredientCondition(Resource::Ingredient, Ingredient::ingredient_enum, quantity, name)

typedef ResourceCondition<Product::ProductType> ProductCondition;
#define PRODUCT_CONDITION(product_enum, quantity, name) ProductCondition(Resource::Product, Product::product_enum, quantity, name)

typedef ResourceCondition<Waste::WasteType> WasteCondition;
#define WASTE_CONDITION(waste_enum, quantity, name) WasteCondition(Resource::Waste, Waste::waste_enum, quantity, name)

//if this is satisfied, the action can happen
class Condition : public Nameable
{

};

//
class Action : public Nameable
{

};

//
//attaches to a building and does something once certain conditions are hit
//
class Worker : public Nameable
{
    public:
        spBuilding building;

        Worker(spBuilding building, std::string name);
};

#endif
