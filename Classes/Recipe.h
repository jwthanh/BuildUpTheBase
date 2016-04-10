#pragma once
#ifndef RECIPE_H
#define RECIPE_H

#include <map>

#include "HouseBuilding.h"
#include "constants.h"

typedef std::map<Ingredient::IngredientType, int> ComponentMap;
typedef std::map<Ingredient::IngredientType, int> OutputMap;
typedef std::map<Product::ProductType, int> ProductMap;

class Recipe : public Nameable
{
    public:
        ComponentMap components;
        OutputMap outputs;

        VoidFunc _callback;
        void callback();

        std::string description;

        Recipe(std::string name="Unnamed", std::string description="");

        //if there's enough ingredients in the input vector to make recipe
        bool is_satisfied(vsIngredient input);
        //remove ING from input
        void consume(vsIngredient& input);

};

#endif
