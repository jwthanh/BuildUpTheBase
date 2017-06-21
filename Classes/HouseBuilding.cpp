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
    for (const auto& pair : this->buildings)
    {
        spBuilding building = pair.second;
        building->update(dt);
    };

};

spBuilding City::building_by_type(BuildingTypes type)
{
    return this->buildings.at(type);
};

Building::Building(City* city, BuildingTypes type, std::string id_key) :
     Nameable(BuildingTypes_to_name.at(type)), Updateable(), Buyable(id_key), type(type), city(city)
{
    this->set_building_level(1);

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

    for (const auto& h_mist : this->harvesters)
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

    for (auto& h_mist : this->salesmen)
    {
        WorkerSubType harv_type = h_mist.first.first;
        res_count_t active_count = h_mist.second;
        total += Salesman::get_to_sell_count(harv_type)*active_count*this->_building_level;
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
    const res_count_t exisiting_count = map_get(mist, sub_type, 0.0L);
    mist[sub_type] = exisiting_count + quantity;
};

void Building::create_ingredients(const Ingredient::SubType& sub_type, res_count_t to_create)
{
    //NOTE this compares this buildings limit against all the ingredients in the city. this might get weird
    res_count_t&& counted_ingredients = BUILDUP->count_ingredients(sub_type);
    const res_count_t& storage_space = this->get_storage_space();

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



template<typename spCacheT>
typename spCacheT::element_type::mapped_type& get_or_create_from_cache(spBuilding& building, spCacheT& cache, std::pair<WorkerSubType, Ingredient::SubType>& key)
{
    typename spCacheT::element_type::iterator&& it = cache->find(key);
    if (it == cache->end()) {
        //create a cache for the key type on the building
        (*cache)[key] = std::make_shared<typename spCacheT::element_type::mapped_type::element_type>(
            building, "cached worker", key.second, key.first
        );
    } 

    return (*cache)[key];
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
        std::shared_ptr<ConsumerHarvester>& consumer = get_or_create_from_cache(shared_this, this->_consumer_cache, key);
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
        std::shared_ptr<Salesman>& salesman = get_or_create_from_cache(shared_this, this->_salesmen_cache, key);

        salesman->active_count = count;
        salesman->update(dt);
    };
}

void Building::_update_scavengers(float dt)
{
    spBuilding shared_this = this->shared_from_this();
    for (auto& mist : this->scavengers) {
        if (this->type != BuildingTypes::TheDump) { break; }; //TODO remove this after debug

        const std::pair<WorkerSubType, Ingredient::SubType>& sub_type = mist.first;
        const WorkerSubType& harv_type = sub_type.first;
        const Ingredient::SubType& ing_type = sub_type.second;
        const res_count_t& count = mist.second;

        std::pair<WorkerSubType, IngredientSubType> key = std::make_pair(harv_type, ing_type);
        std::shared_ptr<ScavengerHarvester> scavenger = get_or_create_from_cache(shared_this, this->_scavenger_cache, key);

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

        std::pair<WorkerSubType, IngredientSubType> key = std::make_pair(harv_type, ing_type);
        spHarvester& harvester = get_or_create_from_cache(shared_this, this->_harvester_cache, key);

        harvester->active_count = count;
        harvester->update(dt);
    };
}

void Building::update(float dt)
{
    Updateable::update(dt);

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

void Building::update_storage_space()
{
    this->_storage_space = BUILDING_LEVEL_STORAGE_LIMIT.at(this->_building_level)*BuildingTypes_to_base_upgrade_cost.at(this->type);
}

const res_count_t& Building::get_storage_space() const
{
    return this->_storage_space;
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

void Building::set_building_level(int new_level)
{
    this->_building_level = new_level;
    this->update_storage_space();
}

const int& Building::get_building_level() const
{
    return this->_building_level;
}
