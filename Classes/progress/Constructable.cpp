#include "Constructable.h"
#include <sstream>
#include <iomanip>
#include "NuItem.h"

ConstructableManager* ConstructableManager::_instance = nullptr;

const bool CTRUCT_LOGGING = false;

Constructable::Constructable(VoidFunc celebrate_func, spBlueprint blueprint)
    : total_in_queue(0),
    _has_celebrated(false),
    _celebrate_func(celebrate_func),
    blueprint(blueprint)
{
    this->init_end_time();
};

void Constructable::init_end_time()
{
    //TODO dont make it always have 'now' as a starting time
    TimePoint end_time = SysClock::now() + this->blueprint->base_duration;
    this->base_end_time = end_time;
    this->current_end_time = this->base_end_time;
};

void Constructable::update(float dt)
{
    if (this->passed_threshold())
    {
        if (this->_has_celebrated == false) {
            if (CTRUCT_LOGGING) CCLOG("trying to celebrate because its false");
            this->try_to_celebrate();
            this->total_in_queue -= 1;

            if (this->total_in_queue >= 1) {
                if (CTRUCT_LOGGING) CCLOG("gte 1 total in queue");
                //update current_end_time based off the blueprint
                this->_has_celebrated = false;
                this->init_end_time();
            } else {
                if (CTRUCT_LOGGING) CCLOG("less than 1 in queue, skipping");
            };
        } else {
            if (CTRUCT_LOGGING) CCLOG("Already celebrated, skipping");
        }
    }
}

bool Constructable::passed_threshold() const
{
    TimePoint now_time_point = SysClock::now();
    Duration diff = now_time_point - this->current_end_time;

    //FIXME this seems like it should be <= instead but it seems to work
    return diff > Duration(0);
}

void Constructable::try_to_celebrate()
{
    if (this->_has_celebrated == false) {
        this->_celebrate_func();

        this->_has_celebrated = true;
    };
}

ConstructableManager* ConstructableManager::getInstance()
{
    if (ConstructableManager::_instance == NULL)
    {
        ConstructableManager::_instance = new ConstructableManager();
    }

    return ConstructableManager::_instance;
}

void ConstructableManager::update(float dt) const
{
    for (const auto& constructable : this->constructables)
    {
        constructable.second->update(dt);
    }
}

void ConstructableManager::add_blueprint_to_queue(spBlueprint blueprint, VoidFunc celebration_func)
{
    //find the blueprint id, if its not in the map add it, otherwise increment the
    // total_in_queue

    auto matched_map_id = this->constructables.find(blueprint->build_map_id());

    spConstructable constructable;
    if (matched_map_id != this->constructables.end()) {
        constructable = matched_map_id->second;
        if (CTRUCT_LOGGING) CCLOG("adding BP to queue, found existing...");
    } else {
        constructable = std::make_shared<Constructable>(celebration_func, blueprint);
        this->constructables[blueprint->build_map_id()] = constructable;
        if (CTRUCT_LOGGING) CCLOG("adding BP to queue, creating new...");
    };

    constructable->total_in_queue += 1;
    //force active in case it already celebrated and is skipping loop
    if (constructable->get_has_celebrated())
    {
        constructable->set_has_celebrated(false);
        constructable->init_end_time();
    }
    if (CTRUCT_LOGGING) CCLOG("...total in queue: %f", constructable->total_in_queue);
};

std::string HarvesterShopNuItemBlueprint::build_map_id()
{
    //generate map_id for the city, building, nuitem type (worker type, sublevel)
    std::stringstream constructable_map_id_ss;
    const std::string divider = "__";
    constructable_map_id_ss << "city" << divider;
    constructable_map_id_ss << this->nuitem->building->name << divider;
    constructable_map_id_ss << "HarvesterShopNuItem" << divider;
    constructable_map_id_ss << "harv_type_" << (int)this->nuitem->harv_type << divider;
    constructable_map_id_ss << "ing_type_" << (int)this->nuitem->ing_type;
    std::string constructable_map_id = constructable_map_id_ss.str();
    return constructable_map_id;
};
