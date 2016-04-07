#pragma once
#ifndef HOUSEBUILDING_H
#define HOUSEBUILDING_H

#include <functional>
#include <memory>

#include "../../Classes/Clock.h"
#include "../../Classes/attribute_container.h"
#include "../../Classes/Resources.h"


#include "../../cocos2d/cocos/cocos2d.h"

#include <map>

#include "../../Classes/combat.h"
#include <ProgressBar.h>
#include <BaseMenu.h>
#include "Buyable.h"

#include "constants.h"
#include "Nameable.h"

class Updateable
{
    public:
        Clock* update_clock;

        Updateable();
        void update(float dt);
};


class Fighter : public Nameable, public Updateable
{
    public:
        enum TeamsType {
            TeamOne = 0, TeamTwo, TeamThree, TeamFour,
            TeamFive, TeamSix, TeamSeven, TeamEight,
            TeamNine, TeamTen
        } team;

        //TODO use these bools someway
        bool has_sword = false;
        bool has_armor = false;
        bool has_shield = false;

        Experience* xp;
        AttributeContainer* attrs;
        Village* city;
        Combat* combat;

        std::string sprite_name = "weapon_gauntlet.png";

        Fighter(Village* city, std::string name) : Nameable(name), Updateable(), city(city) {
            this->attrs = new AttributeContainer();
            this->xp = new Experience();
            this->combat = new Combat("unnamed combat", this);
            this->team = TeamTwo; //default 2 for enemy
        }

        void update(float dt);

        std::string get_stats();
};

class FighterNode : public cocos2d::ui::Widget
{
    public:
        CREATE_FUNC(FighterNode);
        static FighterNode* create(Beatup* beatup, spFighter fighter);

        void update(float dt);

        void set_fighter(spFighter fighter);
        void clear_fighter();

        Beatup* beatup;
        spFighter fighter;

        cocos2d::ui::ImageView* sprite;
        void load_new_sprite(std::string name);

        ProgressBar* hp_bar;
        ProgressBar* xp_bar;
};

class Battle : public Updateable
{
    public:
        Buildup* buildup;

        vsFighter combatants;
        vsFighter combatants_by_team(Fighter::TeamsType team);

        Battle(Buildup* buildup) : buildup(buildup), Updateable() {
            this->combatants = vsFighter();
        };

        void update(float dt) {
            Updateable::update(dt);
            this->do_battle();
        }

        void fight(spFighter left, spFighter right);
        void do_battle();
        void distribute_exp(spFighter dead_fighter);
};

class Building : public Nameable, public Updateable, public Buyable, public std::enable_shared_from_this<Building>
{
    public:

        spBuildingData data;

        Village* city;

        std::string punched_ingredient_type = "wood";

        vsProduct products;
        vsWaste wastes;
        vsIngredient ingredients;

        vsFighter fighters;
        vsWorker workers;

        vsHarvester harvesters;

        Clock* spawn_clock;

        unsigned int num_workers; //people who work here, help make things faster

        TaskFunc task = NULL; //shop might sell product, farm creates ingredients, etc
        std::vector<ItemData> menu_items;

        Building(Village* city, std::string name, std::string id_key, TaskFunc task);
        void create_resources(Resource::ResourceType type, int quantity, std::string name);
        void consume_recipe(Recipe* recipe);

        void update(float dt);

        std::string get_inventory();
        void print_inventory();

        std::map<Ingredient::IngredientType, int> get_ingredient_count();
        std::map<Product::ProductType, int> get_product_count();
        std::map<Waste::WasteType, int> get_waste_count();
        std::string get_ingredients();
        std::string get_products();
        std::string get_wastes();
        std::string get_specifics();

        void print_specifics();
        void print_ingredients();
        void print_products();
        void print_wastes();

        void print_fighters();

        void do_task(float dt);


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


class Animal : public Nameable
{
    public:
        Animal(std::string name) : Nameable(name) {};

        void b2b_transfer(spBuilding from_bldg, spBuilding to_bldg, Resource::ResourceType res_type, int quantity);
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
    public:
        Beatup* beatup;
        Player* player;
        spFighter fighter;
        spFighter brawler;

        spBuilding target_building;

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
