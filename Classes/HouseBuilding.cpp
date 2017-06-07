#include <memory>
#include <vector>
#include <assert.h>

#include <json/stringbuffer.h>

#include "HouseBuilding.h"
#include "Recipe.h"
#include "Worker.h"
#include "Util.h"
#include "StaticData.h"
#include "Fighter.h"
#include "Technology.h"
#include "GameLogic.h"
#include "Clock.h"
#include "Ingredients.h"


#include "Buildup.h"

USING_NS_CC;


void City::update(float dt)
{
    Updateable::update(dt);

    this->update_buildings(dt);
};


void City::update_buildings(float dt)
{
    for (std::shared_ptr<Building>& building : this->buildings)
    {
        building->update(dt);
    };

};

spBuilding City::building_by_name(std::string name)
{
    for (auto bldg : this->buildings)
    {
        if (bldg->name == name)
            return bldg;
    };

    return NULL;
};

Building::Building(City* city, std::string name, std::string id_key) :
             Nameable(name), Buyable(id_key), Updateable(), city(city)
{
    building_level = 1;

    update_clock.set_threshold(1.0f);

    workers = vsWorker();

    this->harvesters = mistHarvester();
    this->_harvester_cache = std::make_shared<HarvesterCache>();

    this->salesmen = mistHarvester();
    this->_salesmen_cache = std::make_shared<SalesmenCache>();

    this->consumers = mistHarvester();
    this->_consumer_cache = std::make_shared<ConsumerCache>();

    this->scavengers = mistHarvester({
        { { WorkerSubType::One, IngredientSubType::Undead }, 1 }
    });
    this->_scavenger_cache = std::make_shared<ScavengerCache>();

    this->data = std::make_shared<BuildingData>(name);
    this->_shop_cost = atoi(this->data->get_base_CI_cost().c_str());
    this->short_name = this->data->get_short_name();

    //TODO TODO
    // work out how to unlock buildings properly
    //TODO TODO
    // this->set_been_bought(true);

    this->techtree = std::make_shared<TechTree>();
};

res_count_t Building::get_total_harvester_output()
{
    res_count_t total = 0;

    for (auto h_mist : this->harvesters)
    {
        WorkerSubType harv_type = h_mist.first.first;
        Ingredient::SubType ing_type = h_mist.first.second;
        res_count_t active_count = h_mist.second;
        total += Harvester::get_to_harvest_count(harv_type, ing_type)*active_count;
    };

    return total;
};

res_count_t Building::get_total_salesmen_output()
{
    res_count_t total = 0;

    for (auto h_mist : this->salesmen)
    {
        WorkerSubType harv_type = h_mist.first.first;
        Ingredient::SubType ing_type = h_mist.first.second;
        res_count_t active_count = h_mist.second;
        total += Salesman::get_to_sell_count(harv_type)*active_count*this->building_level;
    };

    return total;
};

template<typename T>
std::shared_ptr<T> create_one(typename T::SubType sub_type)
{
    return std::make_shared<T>(sub_type);
};

template<typename mistT>
void create(mistT& mist, res_count_t quantity, typename mistT::key_type sub_type)
{
    res_count_t def = 0;
    auto exisiting_count = map_get(mist, sub_type, def);
    mist[sub_type] = exisiting_count + quantity;
};

void Building::create_ingredients(Ingredient::SubType sub_type, res_count_t to_create)
{
    //NOTE this compares this buildings limit against all the ingredients in the city. this might get weird
    auto counted_ingredients = BUILDUP->count_ingredients(sub_type);
    auto storage_space = this->get_storage_space();

    //if its more than storage can fit, only create what can fit
    if ((counted_ingredients + to_create) > storage_space)
    {
        to_create = storage_space - counted_ingredients;
    };

    if (to_create > 0)
    {
        mistIngredient& all_ingredients = BUILDUP->get_all_ingredients();
        create<mistIngredient>(all_ingredients, to_create, sub_type);
    }
};

void Building::consume_recipe(Recipe* recipe)
{
    auto& all_ingredients = BUILDUP->get_all_ingredients();
    if (recipe->is_satisfied(all_ingredients))
    {
        recipe->consume();
    }
};


template<typename CacheT>
typename CacheT::element_type::mapped_type get_or_create_from_cache(spBuilding& building, CacheT& cache, std::pair<WorkerSubType, Ingredient::SubType> key)
{
    typename CacheT::element_type::mapped_type temp_harvester;
    if (cache->find(key) != cache->end())
    {
        //pull it out of cache
        temp_harvester = (*cache)[key];
    }
    else
    {
        //create and add to cache
        temp_harvester = std::make_shared<typename CacheT::element_type::mapped_type::element_type>(building, "test worker", key.second, key.first);
        (*cache)[key] = temp_harvester;
    };

    return temp_harvester;
};

void Building::_update_consumers(float dt)
{
    spBuilding shared_this = this->shared_from_this();
    for (auto& mist : this->consumers) {
        const std::pair<WorkerSubType, Ingredient::SubType>& sub_type = mist.first;
        const WorkerSubType& harv_type = sub_type.first;
        const Ingredient::SubType& ing_type = sub_type.second;
        res_count_t& count = mist.second;

        std::pair<WorkerSubType, IngredientSubType> key = std::make_pair(harv_type, ing_type);
        std::shared_ptr<ConsumerHarvester> consumer = get_or_create_from_cache(shared_this, this->_consumer_cache, key);
        consumer->active_count = count;
        consumer->update(dt);
    };
}

void Building::_update_salesmen(float dt)
{
    spBuilding shared_this = this->shared_from_this();
    for (auto& mist : this->salesmen) {
        const std::pair<WorkerSubType, Ingredient::SubType>& sub_type = mist.first;
        const WorkerSubType& harv_type = sub_type.first;
        const Ingredient::SubType& ing_type = sub_type.second;
        const res_count_t& count = mist.second;

        std::pair<WorkerSubType, IngredientSubType> key = std::make_pair(harv_type, ing_type);
        std::shared_ptr<Salesman> salesman = get_or_create_from_cache(shared_this, this->_salesmen_cache, key);
        salesman->active_count = count;
        salesman->update(dt);
    };
}

void Building::_update_scavengers(float dt)
{
    spBuilding shared_this = this->shared_from_this();
    for (auto& mist : this->scavengers) {
        if (this->name != "The Dump") { break; }; //TODO remove this after debug

        const std::pair<WorkerSubType, Ingredient::SubType>& sub_type = mist.first;
        const WorkerSubType& harv_type = sub_type.first;
        const Ingredient::SubType& ing_type = sub_type.second;
        const res_count_t& count = mist.second;

        //find cache worker, otherwise create and add it to cache
        std::shared_ptr<ScavengerHarvester> scavenger;
        std::pair<WorkerSubType, IngredientSubType> key = std::make_pair(harv_type, ing_type);
        scavenger = get_or_create_from_cache(shared_this, this->_scavenger_cache, key);

        scavenger->active_count = count;
        scavenger->update(dt);
    };
}

void Building::_update_harvesters(float dt)
{
    spBuilding shared_this = this->shared_from_this();
    for (auto& mist : this->harvesters) {
        const std::pair<WorkerSubType, Ingredient::SubType>& sub_type = mist.first;
        const WorkerSubType& harv_type = sub_type.first;
        const Ingredient::SubType& ing_type = sub_type.second;
        const res_count_t& count = mist.second;

        //find cache worker, otherwise create and add it to cache
        std::shared_ptr<Harvester> harvester;
        std::pair<WorkerSubType, IngredientSubType> key = std::make_pair(harv_type, ing_type);
        harvester = get_or_create_from_cache(shared_this, this->_harvester_cache, key);

        harvester->active_count = count;
        harvester->update(dt);
    };
}

void Building::update(float dt)
{
    Updateable::update(dt);

    spBuilding shared_this = this->shared_from_this();

    this->_update_harvesters(dt);

    if (update_clock.passed_threshold())
    {
        update_clock.reset();

        //consumers update before salesmen so that they get first dibs on resources
        this->_update_consumers(dt);
        this->_update_salesmen(dt);
        this->_update_scavengers(dt);

    }
};

res_count_t Building::get_storage_space()
{
    res_count_t _def = 9999999;
    return map_get(BUILDING_LEVEL_STORAGE_LIMIT, this->building_level, _def);
}

bool Building::is_storage_full_of_ingredients(Ingredient::SubType sub_type)
{
    return !this->can_fit_more_ingredients(sub_type, 1);
};

bool Building::can_fit_more_ingredients(Ingredient::SubType sub_type, res_count_t quantity /*= 1*/)
{
    //NOTE uses building storage but global ingredients, might get weird
    return BUILDUP->count_ingredients(sub_type) + quantity <= this->get_storage_space();
};

