#pragma once
#ifndef HOUSEBUILDING_H
#define HOUSEBUILDING_H

#include <functional>
#include <memory>
#include <map>

#include "Clock.h"
#include "Resources.h"
#include "combat.h"

#include "BaseMenu.h"
#include "Buyable.h"

#include "constants.h"
#include "Nameable.h"

//it would be nice to not need this, but mistHarvesters uses it
#include "Worker.h"
#include "Updateable.h"



class Building : public Nameable, public Updateable, public Buyable, public std::enable_shared_from_this<Building>
{
    public:

        spBuildingData data;

        Village* city;

        Ingredient::SubType punched_sub_type;

        mistIngredient ingredients;
        mistProduct products;
        mistWaste wastes;

        vsFighter fighters;
        vsWorker workers;

        mistHarvester harvesters;

        Clock* spawn_clock;

        unsigned int num_workers; //people who work here, help make things faster

        std::vector<ItemData> menu_items;

        Building(Village* city, std::string name, std::string id_key);
        void create_ingredients(Ingredient::SubType sub_type, int quantity);
        void create_products(Product::SubType sub_type, int quantity);
        void create_wastes(Waste::SubType sub_type, int quantity);
    
        void consume_recipe(Recipe* recipe);

        void update(float dt);

        std::string get_inventory();
        void print_inventory();

        res_count_t get_total_harvester_output();

        res_count_t count_ingredients();
        res_count_t count_ingredients(Ingredient::SubType ing_type);
        res_count_t count_products();
        res_count_t count_products(Product::SubType pro_type);
        res_count_t count_wastes();
        res_count_t count_wastes(Waste::SubType wst_type);

        std::string get_ingredients();
        std::string get_products();
        std::string get_wastes();
        std::string get_specifics();

        void print_specifics();
        void print_ingredients();
        void print_products();
        void print_wastes();


        void print_fighters();

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

        spBuilding get_target_building();
        void set_target_building(spBuilding building);

        Village* city;

        static Village* init_city(Buildup*);

        Buildup();
        void main_loop();
        void update(float dt);
};

template<typename from_V>
void remove_if_sized(from_V& from_vs, unsigned int condition_size, unsigned int remove_count, VoidFunc callback );

void move_if_sized(Resource::ResourceType res_type, unsigned int condition_size, unsigned int move_count, spBuilding from_bldg, spBuilding to_bldng, VoidFunc callback);
#endif
