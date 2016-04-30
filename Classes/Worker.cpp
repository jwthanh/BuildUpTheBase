#include "Worker.h"

#include "Clock.h"

#include "HouseBuilding.h"

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

bool ResourceCondition::is_satisfied(spBuilding building){
    if (this->type_choice == Resource::Ingredient) {
        return (int)building->ingredients.size() >= this->quantity;
    }
    else if (this->type_choice == Resource::Product) {
        return (int)building->products.size() >= this->quantity;
    }
    else if (this->type_choice == Resource::Ingredient) {
        return (int)building->wastes.size() >= this->quantity;
    }
    else {
        unsigned int size = building->ingredients.size() +
            building->products.size() +
            building->wastes.size();
        return (int)size >= this->quantity;
    };
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

Harvester::Harvester(spBuilding building, std::string name, Ingredient::SubType ing_type, SubType sub_type)
    : Worker(building, name), ing_type(ing_type), sub_type(sub_type)
{
};

void Harvester::on_update()
{
    auto harvested_count = Harvester::get_harvested_count(this->sub_type);
    //TODO change create_ingredients and active_count to res_count_t type, i dont want to recompile again
    this->building->create_ingredients(this->ing_type, (int)harvested_count*(res_count_t)this->active_count);
};

res_count_t Harvester::get_base_shop_cost(Harvester::SubType harv_type)
{
    res_count_t base_cost = 25;
    
    if (harv_type == Harvester::SubType::One) { base_cost = 25; }
    else if (harv_type == Harvester::SubType::Two) { base_cost = 200; }
    else if (harv_type == Harvester::SubType::Three) { base_cost = 1000; }
    else if (harv_type == Harvester::SubType::Four) { base_cost = 12000; }
    else if (harv_type == Harvester::SubType::Five) { base_cost = 100000; }
    else { base_cost = 9999;}

    return base_cost;
};

res_count_t Harvester::get_harvested_count(Harvester::SubType harv_type)
{
    res_count_t harvested_count = 1;
    
    if (harv_type == Harvester::SubType::One) { harvested_count = 1; }
    else if (harv_type == Harvester::SubType::Two) { harvested_count = 3; }
    else if (harv_type == Harvester::SubType::Three) { harvested_count = 8; }
    else if (harv_type == Harvester::SubType::Four) { harvested_count = 25; }
    else if (harv_type == Harvester::SubType::Five) { harvested_count = 100; }
    else { harvested_count = 9999;}

    return harvested_count;
};
