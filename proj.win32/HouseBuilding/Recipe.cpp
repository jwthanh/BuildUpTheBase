#include "Recipe.h"


Recipe::Recipe()
{
    this->components = ComponentMap();
    this->components[Ingredient::Type::Grain] = 2;
    this->components[Ingredient::Type::Iron] = 1;
};

bool Recipe::is_satisfied(vsIngredient input)
{
    ComponentMap matches = ComponentMap();

    for (auto i : input)
    {
        //if ingredient type's not in matches map, add it
        Ingredient::Type i_type = i->ingredient_type; 
        auto it = matches.find(i_type);
        if (it == matches.end())
        {
            matches[i_type] = 0;
        };

        //increment the match for the given i_type
        matches[i_type] += 1;
    };

    return false;
};
