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

float Constructable::get_current_percent() const
{
    //get duration
    Duration total_duration = this->blueprint->base_duration;
    //get current time and get end time
    TimePoint now_time_point = SysClock::now();
    Duration time_until_passed = this->current_end_time  - now_time_point;

    Duration time_progressed = total_duration - time_until_passed;
    float progress_count = static_cast<float>(time_progressed.count());
    float duration_count = static_cast<float>(total_duration.count());
    float progress = (progress_count / duration_count) * 100.0f;
    //float progress = static_cast<float>(raw_progress);
    CCLOG("progress %f", progress);
    /// if we're 5 seconds away a 20 second duration, we know we're
    /// 75% of the way through

    return progress;
};

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

spConstructable ConstructableManager::add_blueprint_to_queue(spBlueprint blueprint, VoidFunc celebration_func)
{
    //find the blueprint id, if its not in the map add it, otherwise increment the
    // total_in_queue

    spConstructable constructable = this->get_constructable_from_blueprint(blueprint, celebration_func);

    constructable->total_in_queue += 1;
    //force active in case it already celebrated and is skipping loop
    if (constructable->get_has_celebrated())
    {
        constructable->set_has_celebrated(false);
        constructable->init_end_time();
    }
    if (CTRUCT_LOGGING) CCLOG("...total in queue: %f", constructable->total_in_queue);

    return constructable;
};

spConstructable ConstructableManager::get_constructable_from_blueprint(spBlueprint blueprint, VoidFunc celebration_func)
{
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

    return constructable;
};

void Blueprint::serialize(rjDocument& document, rjAllocator& allocator)
{
    std::string map_id = this->build_map_id();
    rjValue key = RJ_STRING(map_id);

    rjValue value(rapidjson::kObjectType);
    //adds the type of the blueprint
    value.AddMember("type_id", RJ_STRING(this->get_serialized_type_id()), allocator);

    //adds the base duration of the blueprint
    Duration raw_duration = this->base_duration;
    auto duration = std::chrono::duration_cast<std::chrono::duration<float>>(raw_duration);
    value.AddMember("duration", rjValue(duration.count()), allocator);

    //adds the values to the document
    document.AddMember(key, value, allocator);
};

HarvesterShopNuItemBlueprint::HarvesterShopNuItemBlueprint(HarvesterShopNuItem* nuitem)
    : HarvesterShopNuItemBlueprint(nuitem->building->name, nuitem->harv_type, nuitem->ing_type)
{
}

HarvesterShopNuItemBlueprint::HarvesterShopNuItemBlueprint(std::string building_name, WorkerSubType worker_subtype, IngredientSubType ing_type)
    : building_name(building_name), worker_subtype(worker_subtype), ing_type(ing_type)
{
    this->base_duration = Duration(1);
}

std::string HarvesterShopNuItemBlueprint::build_map_id()
{
    //generate map_id for the city, building, nuitem type (worker type, sublevel)
    std::stringstream constructable_map_id_ss;
    const std::string divider = "__";
    constructable_map_id_ss << "city" << divider;
    constructable_map_id_ss << this->building_name << divider;
    constructable_map_id_ss << "HarvesterShopNuItem" << divider;
    constructable_map_id_ss << "harv_type_" << (int)this->worker_subtype << divider;
    constructable_map_id_ss << "ing_type_" << (int)this->ing_type;
    std::string constructable_map_id = constructable_map_id_ss.str();
    return constructable_map_id;
}

void HarvesterShopNuItemBlueprint::serialize(rjDocument& document, rjAllocator& allocator)
{
    std::string map_id = this->build_map_id();
    Blueprint::serialize(document, allocator);

    rjValue& value = document[map_id.c_str()];
    value.AddMember("worker_subtype",(int)this->worker_subtype, allocator);
    value.AddMember("ing_type",(int)this->ing_type, allocator);
}

spBlueprint HarvesterShopNuItemBlueprint::load(rjValue& container, rjAllocator& allocator)
{
    std::string building_name = container["building_name"].GetString();
    WorkerSubType worker_subtype = (WorkerSubType)container["worker_subtype"].GetInt();
    IngredientSubType ing_type = (IngredientSubType)container["ing_type"].GetInt();
    spBlueprint created_blueprint = std::make_shared<HarvesterShopNuItemBlueprint>(building_name, worker_subtype, ing_type);

    return created_blueprint;
};

SalesmanShopNuItemBlueprint::SalesmanShopNuItemBlueprint(SalesmanShopNuItem* nuitem)
    : SalesmanShopNuItemBlueprint(nuitem->building->name, nuitem->harv_type, nuitem->ing_type)
{
}

SalesmanShopNuItemBlueprint::SalesmanShopNuItemBlueprint(std::string building_name, WorkerSubType worker_subtype, IngredientSubType ing_type)
    : building_name(building_name), worker_subtype(worker_subtype), ing_type(ing_type)
{
    this->base_duration = Duration(1);
}

std::string SalesmanShopNuItemBlueprint::build_map_id()
{
    //generate map_id for the city, building, nuitem type (worker type, sublevel)
    std::stringstream constructable_map_id_ss;
    const std::string divider = "__";
    constructable_map_id_ss << "city" << divider;
    constructable_map_id_ss << this->building_name << divider;
    constructable_map_id_ss << "SalesmanShopNuItem" << divider;
    constructable_map_id_ss << "harv_type_" << (int)this->worker_subtype << divider;
    constructable_map_id_ss << "ing_type_" << (int)this->ing_type;
    std::string constructable_map_id = constructable_map_id_ss.str();
    return constructable_map_id;
}

void SalesmanShopNuItemBlueprint::serialize(rjDocument& document, rjAllocator& allocator)
{
    std::string map_id = this->build_map_id();
    Blueprint::serialize(document, allocator);

    rjValue& value = document[map_id.c_str()];
    value.AddMember("worker_subtype",(int)this->worker_subtype, allocator);
    value.AddMember("ing_type",(int)this->ing_type, allocator);
}

spBlueprint SalesmanShopNuItemBlueprint::load(rjValue& container, rjAllocator& allocator)
{
    std::string building_name = container["building_name"].GetString();
    WorkerSubType worker_subtype = (WorkerSubType)container["worker_subtype"].GetInt();
    IngredientSubType ing_type = (IngredientSubType)container["ing_type"].GetInt();
    spBlueprint created_blueprint = std::make_shared<SalesmanShopNuItemBlueprint>(building_name, worker_subtype, ing_type);

    return created_blueprint;
};

UpgradeBuildingShopNuItemBlueprint::UpgradeBuildingShopNuItemBlueprint(UpgradeBuildingShopNuItem* nuitem)
    : UpgradeBuildingShopNuItemBlueprint(nuitem->building->name, nuitem->building_level)
{

};

UpgradeBuildingShopNuItemBlueprint::UpgradeBuildingShopNuItemBlueprint(std::string building_name, res_count_t building_level)
    : building_name(building_name), building_level(building_level)
{
    this->base_duration = Duration(1);
};

std::string UpgradeBuildingShopNuItemBlueprint::build_map_id()
{
    //generate map_id for the city, building, nuitem type (worker type, sublevel)
    std::stringstream constructable_map_id_ss;
    const std::string divider = "__";
    constructable_map_id_ss << "city" << divider;
    constructable_map_id_ss << this->building_name << divider;
    constructable_map_id_ss << "UpgradeBuildingShopNuItem" << divider;
    constructable_map_id_ss << "building_level_" << (int)this->building_level;
    std::string constructable_map_id = constructable_map_id_ss.str();
    return constructable_map_id;
}

void UpgradeBuildingShopNuItemBlueprint::serialize(rjDocument& document, rjAllocator& allocator)
{
    std::string map_id = this->build_map_id();
    Blueprint::serialize(document, allocator);

    rjValue& value = document[map_id.c_str()];
    value.AddMember("building_level",(int)this->building_level, allocator);
}

spBlueprint UpgradeBuildingShopNuItemBlueprint::load(rjValue& container, rjAllocator& allocator)
{
    std::string building_name = container["building_name"].GetString();
    int building_level = container["building_level"].GetInt();
    spBlueprint created_blueprint = std::make_shared<UpgradeBuildingShopNuItemBlueprint>(building_name, building_level);

    return created_blueprint;
};
