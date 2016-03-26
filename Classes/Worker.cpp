#include "Worker.h"


Worker::Worker(spBuilding building, std::string name) : Nameable(name) {
    this->building = building;

    INGREDIENT_CONDITION(Berry, 1, "berry_name_huh");
    std::shared_ptr<IngredientCondition> sp_ingredient_condition = std::make_shared<IngredientCondition>(Resource::Ingredient, Ingredient::Berry, 1, "asda");


};
