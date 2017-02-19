#include "Worker.h"

#include "Clock.h"

#include "HouseBuilding.h"
#include "Util.h"
#include "GameLogic.h"
#include "Beatup.h"
#include "Fighter.h"
#include "attribute_container.h"
#include "attribute.h"
#include "MiscUI.h"
#include "Technology.h"
#include "base/CCDirector.h"
#include "HarvestScene.h"
#include "goals/Achievement.h"

Worker::Worker(spBuilding building, std::string name, SubType sub_type)
    : Nameable(name), Updateable(), sub_type(sub_type) {
    this->building = building;

    this->update_clock.set_threshold(1.0f); //unused
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
    this->update_clock.reset();
};

void Worker::on_update(float dt)
{

};

bool ResourceCondition::is_satisfied(spBuilding building){
    //if (this->type_choice == Resource::Ingredient) {
    //    return (int)building->ingredients.size() >= this->quantity;
    //}
    //else if (this->type_choice == Resource::Product) {
    //    return (int)building->products.size() >= this->quantity;
    //}
    //else if (this->type_choice == Resource::Ingredient) {
    //    return (int)building->wastes.size() >= this->quantity;
    //}
    //else {
    //    unsigned int size = building->ingredients.size() +
    //        building->products.size() +
    //        building->wastes.size();
    //    return (int)size >= this->quantity;
    //};
    CCLOG("does this happen, resource condition check");
    return false;
};

ResourceCondition* ResourceCondition::create_ingredient_condition(Ingredient::SubType ing_type, int quantity, std::string condition_name)
{
    auto ingredient_condition = new ResourceCondition(Resource::Ingredient, quantity, condition_name);
    ingredient_condition->ing_type = ing_type;

    return ingredient_condition;
};

Harvester::Harvester(spBuilding building, std::string name, Ingredient::SubType ing_type, SubType sub_type)
    : Worker(building, name, sub_type), ing_type(ing_type)
{
};

void Harvester::on_update(float dt)
{
    auto harvested_count = Harvester::get_to_harvest_count(this->sub_type, this->ing_type);
    res_count_t to_create = harvested_count*this->active_count;
    if (to_create > 0)
    {
        this->building->create_ingredients(this->ing_type, to_create*dt);
    }
};

res_count_t Harvester::get_base_shop_cost(SubType harv_type)
{
    res_count_t base_cost = 25;

    if (harv_type == SubType::One)        { base_cost = 25; }
    else if (harv_type == SubType::Two)   { base_cost = 200; }
    else if (harv_type == SubType::Three) { base_cost = 1000; }
    else if (harv_type == SubType::Four)  { base_cost = 12000; }
    else if (harv_type == SubType::Five)  { base_cost = 100000; }
    else if (harv_type == SubType::Six)   { base_cost = 500000; }
    else if (harv_type == SubType::Seven) { base_cost = 1000000; }
    else if (harv_type == SubType::Eight) { base_cost = 2500000; }
    else if (harv_type == SubType::Nine)  { base_cost = 4500000; }
    else if (harv_type == SubType::Ten)   { base_cost = 15000000; }
    else if (harv_type == SubType::Eleven){ base_cost = 550000000; }
    else { base_cost = 9999;}

    return base_cost;
};

res_count_t Harvester::get_to_harvest_count(WorkerSubType harv_type, IngredientSubType ing_type)
{
    res_count_t harvested_count = 1;

    if (harv_type == SubType::One)         { harvested_count = 0.1; }
    else if (harv_type == SubType::Two)    { harvested_count = 1; }
    else if (harv_type == SubType::Three)  { harvested_count = 8; }
    else if (harv_type == SubType::Four)   { harvested_count = 15; }
    else if (harv_type == SubType::Five)   { harvested_count = 46; }
    else if (harv_type == SubType::Six)    { harvested_count = 37; }
    else if (harv_type == SubType::Seven)  { harvested_count = 50; }
    else if (harv_type == SubType::Eight)  { harvested_count = 175; }
    else if (harv_type == SubType::Nine)   { harvested_count = 519; }
    else if (harv_type == SubType::Ten)    { harvested_count = 2750; }
    else if (harv_type == SubType::Eleven) { harvested_count = 5240; }
    else { harvested_count = 9999;}

    return harvested_count * Ingredient::type_to_harvest_ratio.at(ing_type);
}

Salesman::Salesman(spBuilding building, std::string name, Ingredient::SubType ing_type, SubType sub_type)
    : Harvester(building, name, ing_type, sub_type)
{
}

mistWorkerSubType base_salesman_cost_map = {
    { Salesman::SubType::One, 10 }
};

res_count_t Salesman::get_base_shop_cost(SubType sub_type)
{
    res_count_t default_shop_cost = 9999.0;
    return map_get(base_salesman_cost_map, sub_type, default_shop_cost);
}

mistWorkerSubType base_salesman_to_sell_count = {
    {Salesman::SubType::One, 1}
};

res_count_t Salesman::get_to_sell_count(SubType sub_type)
{
    res_count_t default_to_sell_count = 9999.0;

    //base count for salesmen
    res_count_t base_count = map_get(base_salesman_to_sell_count, sub_type, default_to_sell_count);

    //adjust based on how many upgrades were crafted at the Workshop

    auto workshop = BUILDUP->city->building_by_name("The Workshop");
    auto tech_map = workshop->techtree->get_tech_map();

    auto tech_type = Technology::SubType::SalesmenBaseBoost;
    res_count_t _def = 0L;
    res_count_t times_increased = map_get(tech_map, tech_type, _def);


    return base_count * (times_increased + 1);
};

///NOTE this only gets called once per building->update_clock, not once a frame
void Salesman::on_update(float dt)
{
    res_count_t base_sell_count = Salesman::get_to_sell_count(this->sub_type) * building->building_level;
    res_count_t active_sell_count = base_sell_count*this->active_count;

    mistIngredient& all_ingredients = BUILDUP->get_all_ingredients();

    if (active_sell_count > 0)
    {
        res_count_t def = 0.0;
        res_count_t max_can_sell = map_get(all_ingredients, ing_type, def);

        if (max_can_sell != 0)
        {
            res_count_t num_to_sell = std::min(max_can_sell, active_sell_count);
            res_count_t coins_gained_per = Ingredient::type_to_value.at(this->ing_type);

            //make sure enough coin space for sale, otherwise limit num_to_sell until
            // it's below the space left
            res_count_t coin_storage_left = BEATUP->get_coin_storage_left();


            //max you can sell without going over
            res_count_t max_to_sell = coin_storage_left/coins_gained_per;
            res_count_t final_to_sell = std::min(num_to_sell, max_to_sell);
            res_count_t actual_value = final_to_sell * coins_gained_per;


            //remove the ingredients
            BUILDUP->remove_shared_ingredients_from_all(ing_type, final_to_sell);

            //add the value of amount sold
            BEATUP->add_total_coin(actual_value);

            //create floating label for the amount sold over the correct ing panel
            cocos2d::Scene* root_scene = cocos2d::Director::getInstance()->getRunningScene();
            auto harvest_scene = dynamic_cast<HarvestScene*>(root_scene->getChildByName("HarvestScene"));
            if (harvest_scene)
            {
                std::string message = "+$" + beautify_double(actual_value);
                harvest_scene->spawn_floating_label_for_ing_type(ing_type, message);
            }
            else
            {
                CCLOG("warn no harvest scene found, probably due to this being on startup, so we're not going to make any floating labels");
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
    mistIngredient& all_ingredients = BUILDUP->get_all_ingredients();
    if (BUILDUP->count_ingredients(Ingredient::SubType::Blood) >= active_cost) {
        auto health = BUILDUP->fighter->attrs->health;
        if (health->is_full() == false)
        {
            res_count_t healing = 5 * this->active_count;
            CCLOG("this building has enough blood, healing %f", healing);

            health->add_to_current_val((int)healing);
            all_ingredients[Ingredient::SubType::Blood] -= active_cost;

            auto achievement_manager = AchievementManager::getInstance();
            std::shared_ptr<BaseAchievement> achievement = achievement_manager->getAchievement(AchievementType::TotalHealthRegenerated);
            achievement->increment_by_n(healing);
        }

    };
}
