#include "Worker.h"


Worker::Worker(spBuilding building, std::string name) : Nameable(name) {
    this->building = building;


    //std::shared_ptr<ResourceCondition> sp_ingredient_condition = std::make_shared<ResourceCondition>(Resource::Ingredient, Ingredient::Berry, 1, "asda");

    ResourceCondition* ingredient_condition = ResourceCondition::create_ingredient_condition(Ingredient::Berry, 1, "berry_name_huh");
    ingredient_condition->is_satisfied(building);

};

void Worker::update(float dt)
{
    //make sure there's at least one going before doing the work
    if (this->active_count < 1){
        return;
    };

    Updateable::update(dt);

    if (this->update_clock->passed_threshold())
    {
        this->on_update();
        this->update_clock->reset();
    };
};

void Worker::on_update()
{

};

ResourceCondition* ResourceCondition::create_ingredient_condition(Ingredient::IngredientType ing_type, int quantity, std::string condition_name)
{
    auto ingredient_condition = new ResourceCondition(Resource::Ingredient, quantity, condition_name);
    ingredient_condition->ing_type = ing_type;

    return ingredient_condition;
};

ResourceCondition* ResourceCondition::create_product_condition(Product::ProductType pro_type, int quantity, std::string condition_name)
{
    auto product_condition = new ResourceCondition(Resource::Product, quantity, condition_name);
    product_condition->pro_type = pro_type;

    return product_condition;
};

ResourceCondition* ResourceCondition::create_waste_condition(Waste::WasteType wst_type, int quantity, std::string condition_name)
{
    auto waste_condition = new ResourceCondition(Resource::Waste, quantity, condition_name);
    waste_condition->wst_type = wst_type;

    return waste_condition;
};

Harvester::Harvester(spBuilding building, std::string name) :
    Worker(building, name)
{

};

void Harvester::on_update()
{
    CCLOG("harvester update %s", this->name.c_str());
};
