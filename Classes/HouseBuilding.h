#pragma once
#ifndef HOUSEBUILDING_H
#define HOUSEBUILDING_H

#include <memory>
#include <map>

#include "Clock.h"
#include "Resources.h"

#include "Buyable.h"

#include "constants.h"
#include "Nameable.h"

//it would be nice to not need this, but mistHarvesters uses it
#include "Worker.h"
#include "Updateable.h"


using HarvesterCache = std::shared_ptr<std::map<work_ing_t, std::shared_ptr<Harvester>>>;
using SalesmenCache = std::shared_ptr<std::map<work_ing_t, std::shared_ptr<Salesman>>>;
using ConsumerCache = std::shared_ptr<std::map<work_ing_t, std::shared_ptr<ConsumerHarvester>>>;

class Building : public Nameable, public Updateable, public Buyable, public std::enable_shared_from_this<Building>
{
    public:

        spBuildingData data;

        Village* city;

        Ingredient::SubType punched_sub_type;

        //mistIngredient ingredients;
        mistProduct products;
        mistWaste wastes;

        vsFighter fighters;
        vsWorker workers;

        spVisitors visitors;

        spTechTree techtree;

        mistHarvester harvesters;
        HarvesterCache _harvester_cache;
        mistHarvester salesmen;
        SalesmenCache _salesmen_cache;
        mistHarvester consumers;
        ConsumerCache _consumer_cache;

        Clock* spawn_clock;

        int building_level; //affects only resource limits for now
        res_count_t get_storage_space();
        bool is_storage_full_of_ingredients(Ingredient::SubType);
        bool can_fit_more_ingredients(Ingredient::SubType sub_type, res_count_t quantity = 1);

        unsigned int num_workers; //people who work here, help make things faster

        Building(Village* city, std::string name, std::string id_key);
        void create_ingredients(Ingredient::SubType sub_type, res_count_t quantity);
        void create_products(Product::SubType sub_type, res_count_t quantity);
        void create_wastes(Waste::SubType sub_type, res_count_t quantity);
    
        void consume_recipe(Recipe* recipe);

        void update(float dt);

        res_count_t get_total_harvester_output();
        res_count_t get_total_salesmen_output();

        res_count_t count_ingredients();
        res_count_t count_ingredients(Ingredient::SubType ing_type);
        res_count_t count_products();
        res_count_t count_products(Product::SubType pro_type);
        res_count_t count_wastes();
        res_count_t count_wastes(Waste::SubType wst_type);

        std::string get_products();
        std::string get_wastes();

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

        spClock server_clock;

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
