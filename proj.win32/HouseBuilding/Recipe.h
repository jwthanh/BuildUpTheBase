#pragma once
#ifndef RECIPE_H
#define RECIPE_H

#include <map>

#include "HouseBuilding.h"
typedef std::map<Ingredient::IngredientType, int> ComponentMap;

class Recipe : public Nameable
{
    public:
        ComponentMap components;

        Recipe(std::string name="Unnamed");

        //if there's enough ingredients in the input vector to make recipe
        bool is_satisfied(vsIngredient input);
        //remove ING from input
        void consume(vsIngredient input);

};

#endif
