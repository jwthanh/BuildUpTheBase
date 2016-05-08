#include "Worker.h"

#include "Clock.h"

#include "HouseBuilding.h"
#include "Util.h"
#include "GameLogic.h"
#include "Beatup.h"

Worker::Worker(spBuilding building, std::string name, WorkerSubType sub_type)
    : Nameable(name), Updateable(), sub_type(sub_type) {
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
    this->on_update(dt);
    this->update_clock->reset();
};

void Worker::on_update(float dt)
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

Harvester::Harvester(spBuilding building, std::string name, Ingredient::SubType ing_type, WorkerSubType sub_type)
    : Worker(building, name, sub_type), ing_type(ing_type)
{
};

void Harvester::on_update(float dt)
{
    auto harvested_count = Harvester::get_to_harvest_count(this->sub_type);
    res_count_t to_create = harvested_count*this->active_count;
    if (to_create > 0)
    {
        this->building->create_ingredients(this->ing_type, to_create*dt);
    }
};

res_count_t Harvester::get_base_shop_cost(WorkerSubType harv_type)
{
    res_count_t base_cost = 25;
    
    if (harv_type == WorkerSubType::One) { base_cost = 25; }
    else if (harv_type == WorkerSubType::Two) { base_cost = 200; }
    else if (harv_type == WorkerSubType::Three) { base_cost = 1000; }
    else if (harv_type == WorkerSubType::Four) { base_cost = 12000; }
    else if (harv_type == WorkerSubType::Five) { base_cost = 100000; }
    else if (harv_type == WorkerSubType::Six) { base_cost = 500000; }
    else { base_cost = 9999;}

    return base_cost;
};

res_count_t Harvester::get_to_harvest_count(WorkerSubType harv_type)
{
    res_count_t harvested_count = 1;
    
    if (harv_type == WorkerSubType::One) { harvested_count = 0.1; }
    else if (harv_type == WorkerSubType::Two) { harvested_count = 1; }
    else if (harv_type == WorkerSubType::Three) { harvested_count = 8; }
    else if (harv_type == WorkerSubType::Four) { harvested_count = 25; }
    else if (harv_type == WorkerSubType::Five) { harvested_count = 100; }
    else if (harv_type == WorkerSubType::Six) { harvested_count = 350; }
    else { harvested_count = 9999;}

    return harvested_count;
}

Salesman::Salesman(spBuilding building, std::string name, Ingredient::SubType ing_type, WorkerSubType sub_type)
    : Harvester(building, name, ing_type, sub_type)
{
}

res_count_t Salesman::get_base_shop_cost(WorkerSubType sub_type)
{
    mistWorkerSubType map = {
        {WorkerSubType::One, 10}
    };

    return map_get(map, sub_type, 9999);
}

res_count_t Salesman::get_to_sell_count(WorkerSubType sub_type)
{
    mistWorkerSubType map = {
        {WorkerSubType::One, 1}
    };

    return map_get(map, sub_type, 9999);
};

///NOTE this only gets called once per building->update_clock, not once a frame
void Salesman::on_update(float dt)
{
    res_count_t base_sell_count = Salesman::get_to_sell_count(this->sub_type);
    res_count_t active_sell_count = base_sell_count*this->active_count;

    if (active_sell_count > 0)
    {
        res_count_t max_can_sell = map_get(this->building->ingredients, ing_type, 0);
        if (max_can_sell != 0)
        {
            res_count_t to_sell = std::min(max_can_sell, active_sell_count);
            res_count_t coins_gained = 10;

            if (to_sell >= 1.0)
            {
                std::string string_type = Ingredient::type_to_string(ing_type);
                CCLOG("auto selling %f of %s", to_sell, string_type.c_str());

                this->building->ingredients[ing_type] -= to_sell;
                BEATUP->add_total_coin((double)(to_sell*coins_gained));
                CCLOG("auto SELLING STUFF");
            }
            else
            {
                CCLOG("waiting for more, cant sell only %f", to_sell);
            }
        }
    }
};

