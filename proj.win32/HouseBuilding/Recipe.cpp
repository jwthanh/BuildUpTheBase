#include "Recipe.h"


Recipe::Recipe()
{
    this->components = std::map<Ingredient::Type, int>();
};

bool Recipe::is_satisfied(vsIngredient input)
{

    return false;
};
