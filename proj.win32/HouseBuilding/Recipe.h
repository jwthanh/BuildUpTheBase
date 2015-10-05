#pragma once
#ifndef RECIPE_H
#define RECIPE_H

#include "HouseBuilding.h"
#include <map>
typedef std::map<Ingredient::Type, int> ComponentMap;
class Recipe
{
    public:

        // enum Component {
        //     Grain,
        //     Iron
        // };

        ComponentMap components;

        //if there's enough ingredients in the input vector to make recipe
        bool is_satisfied(vsIngredient input);

        Recipe();
};

#endif
