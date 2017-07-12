#include "Constructable.h"
#include <sstream>
#include "NuItem.h"

ConstructableManager* ConstructableManager::_instance = nullptr;

void Constructable::update(float dt)
{
    if (this->_has_celebrated == false && this->passed_threshold())
    {
        this->try_to_celebrate();
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

std::string HarvesterShopNuItemBlueprint::build_map_id()
{
    //generate map_id for the city, building, nuitem type (worker type, sublevel)
    std::stringstream constructable_map_id_ss;
    const std::string divider;
    constructable_map_id_ss << "city" << divider;
    constructable_map_id_ss << this->nuitem->building->name << divider;
    constructable_map_id_ss << "HarvesterShopNuItem" << divider;
    constructable_map_id_ss << "harv_type_" << (int)this->nuitem->harv_type << divider;
    constructable_map_id_ss << "ing_type" << (int)this->nuitem->ing_type;
    std::string constructable_map_id = constructable_map_id_ss.str();
    return constructable_map_id;
};
