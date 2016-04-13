#pragma once
#ifndef RECIPE_H
#define RECIPE_H

#include <map>

#include "HouseBuilding.h"
#include "constants.h"

typedef std::map<Ingredient::SubType, int> ComponentMap;
typedef std::map<Ingredient::SubType, int> OutputMap;
typedef std::map<Product::SubType, int> ProductMap;

class Recipe : public Nameable
{
    public:
        int current_clicks;
        int clicks_required;

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
