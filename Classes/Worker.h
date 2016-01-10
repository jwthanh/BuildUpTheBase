#pragma once
#ifndef WORKER_H
#define WORKER_H

#include "HouseBuilding.h"

template<typename Rtype>
class ResourceCondition : public Nameable
{
    public:
        enum TypeChoices {
            Any = 0,
            Ingredient,
            Product, 
            Waste
        };
        TypeChoices type_choice;
        Rtype choice;

        int quantity;

        ResourceCondition(TypeChoices type_choice, Rtype choice, int quantity, std::string name) : Nameable(name) {
            this->type_choice;
            this->choice = choice;
            this->quantity = quantity;
        };

        bool is_satisfied(spBuilding building) {
            if (this->type_choice == Ingredient)
            {
                return (int)building->ingredients.size() >= this->quantity;
            }
            else if (this->type_choice == Product)
            {
                return (int)building->products.size() >= this->quantity;
            }
            else if (this->type_choice == Ingredient)
            {
                return (int)building->wastes.size() >= this->quantity;
            }
            else
            {
                unsigned int size = building->ingredients.size() +
                    building->products.size() +
                    building->wastes.size();
                return (int)size >= this->quantity;
            };
        };
};

typedef ResourceCondition<Ingredient::IngredientType> IngredientCondition;
#define IngredientConditionEx(ingredient_enum, quantity, name) IngredientCondition(IngredientCondition::TypeChoices::Ingredient, Ingredient::ingredient_enum, quantity, name)
typedef ResourceCondition<Product::ProductType> ProductCondition;
#define ProductConditionEx(product_enum, quantity, name) ProductCondition(ProductCondition::TypeChoices::Product, Product::product_enum, quantity, name)
typedef ResourceCondition<Waste::WasteType> WasteCondition;
#define WasteConditionEx(waste_enum, quantity, name) WasteCondition(WasteCondition::TypeChoices::Waste, Waste::waste_enum, quantity, name)

class Action : public Nameable
{

};

//
//attaches to a building and builds recipes once certain conditions are hit
//
class Worker : public Nameable
{
    public:
        spBuilding building;

        Worker(spBuilding building, std::string name) : Nameable(name) {
            this->building = building;
        };
};

#endif
