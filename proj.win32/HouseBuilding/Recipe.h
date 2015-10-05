#pragma once
#ifndef RECIPE_H
#define RECIPE_H

#include "HouseBuilding.h"
#include <map>

class Recipe
{
    public:

        // enum Component {
        //     Grain,
        //     Iron
        // };

        std::map<Ingredient::Type, int> components;

        //if there's enough ingredients in the input vector to make recipe
        bool is_satisfied(vsIngredient input);

        Recipe();
};

#endif
