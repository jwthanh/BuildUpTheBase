#include "Recipe.h"
#include "GameLogic.h"
#include "HouseBuilding.h"
#include "Buildup.h"


void Recipe::callback()
{
    if (_callback != NULL)
    {
        this->_callback();
    }
}

Recipe::Recipe(std::string name, std::string description) : Nameable(name), description(description)
{
    this->current_clicks = 0;
    this->clicks_required = 15;

    this->components = ComponentMap();
    this->outputs = OutputMap();

    // this->_callback = [this](Beatup* bu) {
    //     this-> //TODO figure out how to add this output to building
    // };

    //example recipe
    // this->components[Ingredient::SubType::Grain] = 2;
    // this->components[Ingredient::SubType::Iron] = 1;
};

bool Recipe::is_satisfied(mistIngredient input)
{
    ComponentMap input_components = ComponentMap();

    for (auto& ingredient : input)
    {
        //if ingredient type's not in matches map, add it regardless of whether
        //its relevant or not
        Ingredient::SubType i_type = ingredient.first;
        input_components[i_type] = input[i_type];
    };

    for (auto& pair : input_components)
    {
        auto it = this->components.find(pair.first);
        //if the type isnt in the component map, ignore it
        if (it == this->components.end()) continue;

        //if there's not enough input, fail early
        Ingredient::SubType i_type = it->first;
        if (input_components[i_type] < this->components[i_type])
        {
            return false;
        };

    };

    for (auto& pair : this->components)
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

void Recipe::consume()
{
    ComponentMap temp_map = ComponentMap(this->components);
    auto& all_ingredients = BUILDUP->get_all_ingredients();
    if (this->is_satisfied(all_ingredients))
    {
        for (auto component_pair : temp_map)
        {
            BUILDUP->remove_shared_ingredients_from_all(component_pair.first, component_pair.second);
        }

        this->callback();
    }
    else
    {

    };
};
