#pragma once
#ifndef HOUSEBUILDING_H
#define HOUSEBUILDING_H

#include <memory>
#include <map>

#include "Clock.h"
#include "Ingredients.h"

#include "Buyable.h"

#include "constants.h"
#include "Nameable.h"

#include "Updateable.h"

#include "Worker.h" //it would be nice to not need this, but mistHarvesters uses it


using HarvesterCache = std::shared_ptr<std::map<work_ing_t, std::shared_ptr<Harvester>>>;
using SalesmenCache = std::shared_ptr<std::map<work_ing_t, std::shared_ptr<Salesman>>>;
using ConsumerCache = std::shared_ptr<std::map<work_ing_t, std::shared_ptr<ConsumerHarvester>>>;

static const std::map<int, res_count_t> building_storage_limit = {
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
    public:

        spBuildingData data;

        Village* city;

        Ingredient::SubType punched_sub_type;

        vsWorker workers;

        spVisitors visitors;

        spTechTree techtree;

        mistHarvester harvesters;
        HarvesterCache _harvester_cache;
        mistHarvester salesmen;
        SalesmenCache _salesmen_cache;
        mistHarvester consumers;
        ConsumerCache _consumer_cache;

        Clock spawn_clock;

        int building_level; //affects only resource limits for now
        res_count_t get_storage_space();
        bool is_storage_full_of_ingredients(Ingredient::SubType);
        bool can_fit_more_ingredients(Ingredient::SubType sub_type, res_count_t quantity = 1);

        unsigned int num_workers; //people who work here, help make things faster

        Building(Village* city, std::string name, std::string id_key);
        void create_ingredients(Ingredient::SubType sub_type, res_count_t quantity);

        void consume_recipe(Recipe* recipe);

        void update(float dt);

        res_count_t get_total_harvester_output();
        res_count_t get_total_salesmen_output();


};

class Village : public Nameable, public Updateable
{
    public:

        Buildup* buildup;

        vsBuilding buildings;

        Village(Buildup* buildup, std::string name)
            : Nameable(name), Updateable(), buildup(buildup)
        {
            buildings = {};
        };

        void update(float dt);
        void update_buildings(float dt);

        spBuilding building_by_name(std::string name);
};


class Person : public Nameable, public Updateable
{
    public:
        Person(std::string name) : Nameable(name), Updateable() {
        };
};

class Player : public Person
{
    public:
        unsigned int coins;
        Player(std::string name) : Person(name) {};

        void update(float dt);
};

class Enemy : public Person
{
    public:
        Enemy(std::string name) : Person(name) {};
};

class Buildup
{
    private:
        spBuilding _target_building;

    public:
        Player* player;
        spFighter fighter;
        spFighter brawler;

        vsItem items;

        spBuilding get_target_building();
        void set_target_building(spBuilding building);

        Village* city;

        static Village* init_city(Buildup*);
        res_count_t count_ingredients();
        res_count_t count_ingredients(Ingredient::SubType ing_type);

        Clock server_clock;

    private:
        mistIngredient _all_ingredients;
    public:
        mistIngredient& get_all_ingredients();
        void remove_shared_ingredients_from_all(Ingredient::SubType ing_type, res_count_t count);

        Buildup();
        void update(float dt);

        void post_update();
};

template<typename from_V>
void remove_if_sized(from_V& from_vs, unsigned int condition_size, unsigned int remove_count, VoidFunc callback );

void move_if_sized(Resource::ResourceType res_type, unsigned int condition_size, unsigned int move_count, spBuilding from_bldg, spBuilding to_bldng, VoidFunc callback);
#endif
