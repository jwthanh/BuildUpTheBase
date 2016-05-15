#pragma once
#ifndef RECIPE_H
#define RECIPE_H

#include <map>

#include "constants.h"
#include "Resources.h"

typedef mistIngredient ComponentMap;
typedef mistIngredient OutputMap;
typedef mistProduct ProductMap;

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
        bool is_satisfied(mistIngredient input);
        //remove ING from input
        void consume(mistIngredient& input);

};

#endif
