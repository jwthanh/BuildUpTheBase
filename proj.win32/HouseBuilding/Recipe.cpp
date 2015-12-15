#include "Recipe.h"
#include <algorithm>


void Recipe::callback(Beatup* beatup)
{
    if (_callback != NULL)
    {
        this->_callback(beatup);
    }
}

Recipe::Recipe(std::string name) : Nameable(name)
{
    this->components = ComponentMap();

    //example recipe
    // this->components[Ingredient::IngredientType::Grain] = 2;
    // this->components[Ingredient::IngredientType::Iron] = 1;
};

bool Recipe::is_satisfied(vsIngredient input)
{
    ComponentMap input_components = ComponentMap();

    for (auto ingredient : input)
    {
        //if ingredient type's not in matches map, add it regardless of whether
        //its relevant or not
        Ingredient::IngredientType i_type = ingredient->ingredient_type; 
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
        Ingredient::IngredientType i_type = it->first;
        if (input_components[i_type] < this->components[i_type])
        {
            return false;
        };

    };

    for (auto pair : this->components)
    {
        auto it = input_components.find(pair.first);
        //if the type isnt in the inputs map, fail
        if (it == input_components.end())
        {
            return false;
        };
    }
    return true;
};

void Recipe::consume(vsIngredient& input)
{
    ComponentMap temp_map = ComponentMap(this->components);
    if (this->is_satisfied(input))
    {
        auto removal_fun = [&temp_map](spIngredient element) -> bool {
            try {
                int& val = temp_map.at(element->ingredient_type);

                if (val > 0) {
                    val -= 1;
                    return true;
                } else {
                    return false;
                }

            }
            catch (const std::out_of_range&) {
                return false;
            }
        };

        input.erase(
            std::remove_if(input.begin(), input.end(), removal_fun),
            input.end()
        );
        print("consumed " << this->name);
    }
    else
    {

        print("can't make recipe " << this->name);
    };
};
