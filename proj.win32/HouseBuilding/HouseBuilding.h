#pragma once
#ifndef HOUSEBUILDING_H
#define HOUSEBUILDING_H

#include <functional>
#include <memory>

#include "../../Classes/Clock.h"
#include "../../Classes/attribute_container.h"
//#include "../../Classes/RandomWeightMap.h"

#include "../../cocos2d/cocos/cocos2d.h"

#include <map>


#define print(msg) {std::stringstream ss123; ss123<< msg; CCLOG(ss123.str().c_str());}
#define print1(msg) {std::stringstream ss123; ss123<< "\t" << msg; CCLOG(ss123.str().c_str());}
#define print2(msg) {std::stringstream ss123; ss123<< "\t\t" << msg; CCLOG(ss123.str().c_str());}
#define NO_CB [](){}
#include "../../Classes/combat.h"
#include <ProgressBar.h>


class Recipe;
class Beatup;
class Building;
class Buildup;
class Village;
class Product;
class Ingredient;
class Waste;
class Resource;
class Fighter;
class Battle;
class Worker;


typedef std::function<void()> VoidFunc;

#define _MAKE_SP(Cls) typedef std::shared_ptr<##Cls> sp##Cls
#define _MAKE_VS(Cls) typedef std::vector<sp##Cls> vs##Cls

#define MAKE_SP_VS(Cls) _MAKE_SP(##Cls);_MAKE_VS(##Cls)

MAKE_SP_VS(Building);
MAKE_SP_VS(Resource);
MAKE_SP_VS(Product);
MAKE_SP_VS(Ingredient);
MAKE_SP_VS(Waste);
MAKE_SP_VS(Battle);
MAKE_SP_VS(Fighter);
MAKE_SP_VS(Worker);

#undef MAKE_SP_VS
#undef _MAKE_VS
#undef _MAKE_SP

//typedef void(*VoidFuncBuilding)(spBuilding);
typedef void(*TaskFunc)(spBuilding, float);
typedef bool(*BoolFuncBuilding)(spBuilding);

class Updateable
{
    public:
        Clock* update_clock;

        Updateable();
        void update(float dt);
};

class Nameable
{
    public:
        std::string name;

        Nameable(std::string name) :name(name) {};
        Nameable(const Nameable& other)
        {
            this->name = other.name;
        }
};

class Resource : public Nameable
{
    public:
        enum ResourceType {
            Ingredient = 0,
            Product = 1,
            Waste = 2
        };

        // convert type to string
        static std::string type_str(ResourceType type);

        static const ResourceType resource_type;

        Resource(std::string name) :Nameable(name){};
};

#define TYPE_MAP_CONVERSION(Rtype, Rlowertype) \
    static const std::map<##Rtype##Type, std::string> type_map; \
##Rtype##Type Rlowertype##_type; \
static std::string type_to_string(Rtype::##Rtype##Type type); \
static Rtype::##Rtype##Type string_to_type(std::string string_type); 

class Ingredient : public Resource
{
    public:
        static const ResourceType resource_type = Resource::ResourceType::Ingredient;

        enum IngredientType {
            None,
            Grain,
            PileOfGrain,
            Seed,
            Tree,
            Iron,
            Copper,
            Wood,
            Fly,
            Flesh,
            Berry
        };

        TYPE_MAP_CONVERSION(Ingredient, ingredient);

        Ingredient(std::string name) : Resource(name) {
            this->ingredient_type = Ingredient::string_to_type(name);
        };

        Ingredient(const Ingredient& other) : Resource(other) {
            this->ingredient_type = other.ingredient_type;
        };

};

class Product : public Resource
{
    public:
        static const ResourceType resource_type = Resource::ResourceType::Product;

        enum ProductType {
            None,
            Veggies,
            Meat,
            Dairy,
            Cereals,
            Sword,
            Shield
        };

        TYPE_MAP_CONVERSION(Product, product);

        Product(std::string name) : Resource(name) {
            this->product_type = Product::string_to_type(name);
        };

        Product(const Product& other) : Resource(other) {
            this->product_type = other.product_type;
        };
};

class Waste : public Resource
{
    public:
        static const ResourceType resource_type = Resource::ResourceType::Waste;

        enum WasteType {
            None,
            Corpse,
            Wasted_Iron
        };

        TYPE_MAP_CONVERSION(Waste, waste);


        Waste(std::string name) : Resource(name) {
            this->waste_type = Waste::string_to_type(name);
        };

        Waste(const Waste& other) : Resource(other) {
            this->waste_type = other.waste_type;
        };
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

class FighterNode : public cocos2d::Node
{
    public:
        CREATE_FUNC(FighterNode);
        static FighterNode* create(Beatup* beatup, spFighter fighter);

        void update(float dt);

        void clear_fighter();

        Beatup* beatup;
        spFighter fighter;

        cocos2d::Sprite* sprite;
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

class Building : public Nameable, public Updateable, public std::enable_shared_from_this<Building>
{
    public:

        Village* city;

        std::string punched_ingredient_type = "wood";

        vsProduct products;
        vsWaste wastes;
        vsIngredient ingredients;

        vsFighter fighters;
        vsWorker workers;

        Clock* spawn_clock;

        unsigned int num_workers; //people who work here, help make things faster
        TaskFunc task = NULL; //shop might sell product, farm creates ingredients, etc
        Building(Village* city, std::string name, TaskFunc task)
            : task(task), Nameable(name), Updateable(), city(city)
        {
            num_workers = 1;

            update_clock->set_threshold(1.0f);
            spawn_clock = new Clock(3);

            products = vsProduct();
            wastes = vsWaste();
            ingredients = vsIngredient();

            fighters = vsFighter();
            workers = vsWorker();

        };

        void create_resources(Resource::ResourceType type, int quantity, std::string name);
        void consume_recipe(Recipe* recipe);

        void update(float dt);

        std::string get_inventory();
        void print_inventory();

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
