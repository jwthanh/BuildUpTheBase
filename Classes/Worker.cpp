#include "Worker.h"


Worker::Worker(spBuilding building, std::string name)
    : Nameable(name), Updateable() {
    this->building = building;

    this->update_clock->set_threshold(1.0f); //unused

    ResourceCondition* ingredient_condition = ResourceCondition::create_ingredient_condition(Ingredient::Berry, 1, "berry_name_huh");
    ingredient_condition->is_satisfied(building);

};

void Worker::update(float dt)
{
    //make sure there's at least one going before doing the work
    if (this->active_count < 1){
        return;
    };

    //used to use update_timer, but this only gets called when a building gets
    //updated so it doesnt need to be regulated anyway
    this->on_update();
    this->update_clock->reset();
};

void Worker::on_update()
{

};

ResourceCondition* ResourceCondition::create_ingredient_condition(Ingredient::SubType ing_type, int quantity, std::string condition_name)
{
    auto ingredient_condition = new ResourceCondition(Resource::Ingredient, quantity, condition_name);
    ingredient_condition->ing_type = ing_type;

    return ingredient_condition;
};

ResourceCondition* ResourceCondition::create_product_condition(Product::SubType pro_type, int quantity, std::string condition_name)
{
    auto product_condition = new ResourceCondition(Resource::Product, quantity, condition_name);
    product_condition->pro_type = pro_type;

    return product_condition;
};

ResourceCondition* ResourceCondition::create_waste_condition(Waste::SubType wst_type, int quantity, std::string condition_name)
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
    this->building->create_resources(Resource::ResourceType::Ingredient, 1, this->building->punched_ingredient_type);
    CCLOG("harvester %i update %s", this, this->name.c_str());
};
