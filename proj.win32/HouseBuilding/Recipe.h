#pragma once
#ifndef RECIPE_H
#define RECIPE_H

#include <map>

#include "HouseBuilding.h"
typedef std::map<Ingredient::Type, int> ComponentMap;

class Recipe : public Nameable
{
    public:
        ComponentMap components;

        //if there's enough ingredients in the input vector to make recipe
        bool is_satisfied(vsIngredient input);

        Recipe(std::string name="Unnamed");
};

#endif
