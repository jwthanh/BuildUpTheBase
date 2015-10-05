#include "Recipe.h"


Recipe::Recipe()
{
    this->components = ComponentMap();
    this->components[Ingredient::Type::Grain] = 2;
    this->components[Ingredient::Type::Iron] = 1;
};

bool Recipe::is_satisfied(vsIngredient input)
{
    ComponentMap input_components = ComponentMap();

    for (auto ingredient : input)
    {
        //if ingredient type's not in matches map, add it regardless of whether
        //its relevant or not
        Ingredient::Type i_type = ingredient->ingredient_type; 
        auto it = input_components.find(i_type);
        if (it == input_components.end())
        {
            input_components[i_type] = 0;
        };

        //increment the match for the given i_type
        input_components[i_type] += 1;
    };

    for (auto pair : input_components)
    {
        auto it = this->components.find(pair.first);
        //if the type isnt in the component map, ignore it
        if (it == this->components.end()) continue;

        //if there's not enough input, fail early
        Ingredient::Type i_type = it->first;
        if (input_components[i_type] < this->components[i_type])
        {
            return false;
        };

    };
    return true;
};
