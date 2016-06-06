#include "Worker.h"

#include "Clock.h"

#include "HouseBuilding.h"
#include "Util.h"
#include "GameLogic.h"
#include "Beatup.h"
#include "Fighter.h"
#include "attribute_container.h"
#include "attribute.h"

Worker::Worker(spBuilding building, std::string name, SubType sub_type)
    : Nameable(name), Updateable(), sub_type(sub_type) {
    this->building = building;

    this->update_clock->set_threshold(1.0f); //unused

    ResourceCondition* ingredient_condition = ResourceCondition::create_ingredient_condition(Ingredient::SubType::Berry, 1, "berry_name_huh");
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

Harvester::Harvester(spBuilding building, std::string name, Ingredient::SubType ing_type, SubType sub_type)
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

res_count_t Harvester::get_base_shop_cost(SubType harv_type)
{
    res_count_t base_cost = 25;
    
    if (harv_type == SubType::One) { base_cost = 25; }
    else if (harv_type == SubType::Two) { base_cost = 200; }
    else if (harv_type == SubType::Three) { base_cost = 1000; }
    else if (harv_type == SubType::Four) { base_cost = 12000; }
    else if (harv_type == SubType::Five) { base_cost = 100000; }
    else if (harv_type == SubType::Six) { base_cost = 500000; }
    else { base_cost = 9999;}

    return base_cost;
};

res_count_t Harvester::get_to_harvest_count(SubType harv_type)
{
    res_count_t harvested_count = 1;
    
    if (harv_type == SubType::One) { harvested_count = 0.1; }
    else if (harv_type == SubType::Two) { harvested_count = 1; }
    else if (harv_type == SubType::Three) { harvested_count = 8; }
    else if (harv_type == SubType::Four) { harvested_count = 25; }
    else if (harv_type == SubType::Five) { harvested_count = 100; }
    else if (harv_type == SubType::Six) { harvested_count = 350; }
    else { harvested_count = 9999;}

    return harvested_count;
}

Salesman::Salesman(spBuilding building, std::string name, Ingredient::SubType ing_type, SubType sub_type)
    : Harvester(building, name, ing_type, sub_type)
{
}

res_count_t Salesman::get_base_shop_cost(SubType sub_type)
{
    mistWorkerSubType map = {
        {SubType::One, 10}
    };

    return map_get(map, sub_type, 9999);
}

res_count_t Salesman::get_to_sell_count(SubType sub_type)
{
    mistWorkerSubType map = {
        {SubType::One, 1}
    };

    return map_get(map, sub_type, 9999);
};

///NOTE this only gets called once per building->update_clock, not once a frame
void Salesman::on_update(float dt)
{
    res_count_t base_sell_count = Salesman::get_to_sell_count(this->sub_type) * building->building_level;
    res_count_t active_sell_count = base_sell_count*this->active_count;

    if (active_sell_count > 0)
    {
        res_count_t max_can_sell = map_get(this->building->ingredients, ing_type, 0);
        if (max_can_sell != 0)
        {
            res_count_t to_sell = std::min(max_can_sell, active_sell_count);
            res_count_t coins_gained = 3; //TODO use actual resource price instead of hardcoded FIXME

            if (to_sell >= active_sell_count)
            {
                std::string string_type = Ingredient::type_to_string(ing_type);

                this->building->ingredients[ing_type] -= to_sell;
                BEATUP->add_total_coin((double)(to_sell*coins_gained));
            }
            else
            {
            }
        }
    }
};

ConsumerHarvester::ConsumerHarvester(spBuilding building, std::string name, Ingredient::SubType ing_type, SubType sub_type)
    : Harvester(building, name, ing_type, sub_type)
{
    
}

void ConsumerHarvester::on_update(float dt)
{
    res_count_t blood_cost = 5.0;
    res_count_t active_cost = blood_cost * this->active_count;
    if (this->building->count_ingredients(Ingredient::SubType::Blood) >= active_cost) {
        auto health = BUILDUP->fighter->attrs->health;
        if (health->is_full() == false)
        {
            res_count_t healing = 5 * this->active_count;
            CCLOG("this building has enough blood, healing %L", healing);

            health->add_to_current_val((int)healing);
            this->building->ingredients[Ingredient::SubType::Blood] -= active_cost;
        }

    };
}
