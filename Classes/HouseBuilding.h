#pragma once
#ifndef HOUSEBUILDING_H
#define HOUSEBUILDING_H

#include <memory>
#include <map>

#include "SubTypes.h"

#include "Buyable.h"

#include "constants.h"
#include "Nameable.h"

#include "Updateable.h"

enum class BuildingTypes
{
    TheFarm = 0,
    TheArena,
    TheUnderscape,
    TheMarketplace,
    TheDump,
    TheWorkshop,
    TheMine,
    TheGraveyard,
    TheForest
};

const std::map<BuildingTypes, std::string> BuildingTypes_to_name = {
    { BuildingTypes::TheFarm, "The Farm" },
    { BuildingTypes::TheArena, "The Arena" },
    { BuildingTypes::TheUnderscape, "The Underscape" },
    { BuildingTypes::TheMarketplace, "The Marketplace" },
    { BuildingTypes::TheDump, "The Dump" },
    { BuildingTypes::TheWorkshop, "The Workshop" },
    { BuildingTypes::TheMine, "The Mine" },
    { BuildingTypes::TheGraveyard, "The Graveyard" },
    { BuildingTypes::TheForest, "The Forest" }
};


using HarvesterCache = std::map<work_ing_t, std::shared_ptr<Harvester>>;
using spHarvesterCache = std::shared_ptr<HarvesterCache>;

using SalesmenCache = std::map<work_ing_t, std::shared_ptr<Salesman>>;
using spSalesmenCache = std::shared_ptr<SalesmenCache>;

using ConsumerCache = std::map<work_ing_t, std::shared_ptr<ConsumerHarvester>>;
using spConsumerCache = std::shared_ptr<ConsumerCache>;

using ScavengerCache = std::map<work_ing_t, std::shared_ptr<ScavengerHarvester>>;
using spScavengerCache = std::shared_ptr<ScavengerCache>;

static const std::map<int, res_count_t> BUILDING_LEVEL_STORAGE_LIMIT = {
    { 1, 25.0},
    { 2, 50.0},
    { 3, 250.0},
    { 4, 1000.0},
    { 5, 5000.0},
    { 6, 150000.0},
    { 7, 500000.0},
    { 8, 1000000.0},
    { 9, 6500000.0},
    {10, 12000000.0},
    {11, 500000000.0},
    {12, 800000000.0},
    {13, 1000000000.0},
    {14, 50000000000.0},
    {15, 1000000000000.0},
    {16, 5000000000000.0}
};


class Building : public Nameable, public Updateable, public Buyable, public std::enable_shared_from_this<Building>
{
    private:
        void _update_harvesters(float dt);
        spHarvesterCache _harvester_cache;
        void _update_salesmen(float dt);
        spSalesmenCache _salesmen_cache;
        void _update_consumers(float dt);
        spConsumerCache _consumer_cache;
        void _update_scavengers(float dt);
        spScavengerCache _scavenger_cache;

    public:
        BuildingTypes type;

        spBuildingData data;

        City* city;

        IngredientSubType punched_sub_type;

        vsWorker workers;

        spVisitors visitors;

        spTechTree techtree;

        mistHarvester harvesters;
        mistHarvester salesmen;
        mistHarvester consumers;
        mistHarvester scavengers;

        int building_level; //affects only resource limits for now
        res_count_t get_storage_space();
        bool is_storage_full_of_ingredients(IngredientSubType);
        bool can_fit_more_ingredients(IngredientSubType sub_type, res_count_t quantity = 1);

        Building(City* city, BuildingTypes type, std::string id_key);
        void create_ingredients(IngredientSubType sub_type, res_count_t quantity);

        void consume_recipe(Recipe* recipe);

        void update(float dt);

        res_count_t get_total_harvester_output();
        res_count_t get_total_salesmen_output();


};

class City : public Nameable, public Updateable
{
    public:

        Buildup* buildup;

        std::map<BuildingTypes, spBuilding> buildings;

        City(Buildup* buildup, std::string name)
            : Nameable(name), Updateable(), buildup(buildup)
        {
        };

        void update(float dt);
        void update_buildings(float dt);

        spBuilding building_by_name(const std::string& name);
};

#endif
