#pragma once
#ifndef HOUSEBUILDING_H
#define HOUSEBUILDING_H

#include <functional>
#include <memory>
#include <iostream>
#include <random>

#include "../../Classes/Clock.h"
#include "../../Classes/attribute.h"
#include "../../Classes/attribute_container.h"
#include "../../Classes/RandomWeightMap.h"


#define print(msg) std::cout << msg << std::endl
#define print1(msg) std::cout << "\t" << msg << std::endl
#define print2(msg) std::cout << "\t\t" << msg << std::endl
#define NO_CB [](){}


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

//typedef void(*VoidFuncBuilding)(Building*);
typedef void(*TaskFunc)(Building*, float);
typedef bool(*BoolFuncBuilding)(Building*);

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
            Seed,
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
            Shield
        };
        TYPE_MAP_CONVERSION(Product, product);

        Product(std::string name) : Resource(name) {
            this->product_type = Product::string_to_type(name);
        };

        Product(const Product& other) : Resource(other) {
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
};

class Fighter : public Nameable, public Updateable
{
    public:
        AttributeContainer* attrs;
        float damage;

        Village* city;
        Fighter(Village* city, std::string name) : Nameable(name), Updateable(), city(city) {
            this->attrs = new AttributeContainer();
        }
        void update(float dt);

        std::string get_stats() {
            std::stringstream ss;
            ss << this->name << " ";
            int effective_dmg = this->attrs->damage->current_val;
            ss << this->attrs->health->current_val << "/" << this->attrs->health->max_val << "/" << effective_dmg;
            // for (std::string attr : this->attrs->PrettyVector())
            // {
            //     ss << attr;
            // }

            return ss.str();
        };
};

class Battle : public Updateable
{
    public:
        vsFighter combatants;

        Battle() : Updateable()
    {
        this->combatants = vsFighter();
    };

        void update(float dt)
        {
            Updateable::update(dt);
            this->do_battle();
        }

        void do_battle();
};

class Building : public Nameable, public Updateable
{
    public:

        Village* city;

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

            spawn_clock = new Clock(3000);

            products = vsProduct();
            wastes = vsWaste();
            ingredients = vsIngredient();

            fighters = vsFighter();
            workers = vsWorker();

        };

        void create_resources(Resource::ResourceType type, int quantity, std::string name);

        void update(float dt);

        void print_inventory();

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

        Building* building_by_name(std::string name);
};


class Animal : public Nameable
{
    public:
        Animal(std::string name) : Nameable(name) {};

        void b2b_transfer(Building* from_bldg, Building* to_bldg, Resource::ResourceType res_type, int quantity);
};

class Person : public Nameable, public Updateable
{
    public:
        Person(std::string name) : Nameable(name), Updateable() {};
};

class Player : public Person
{
    public:
        unsigned int coins;
        Player(std::string name) : Person(name) {};

        void update(float dt);
};

class Buildup
{
    public:
        Player* player;

        Village* city;

        Buildup();
        void main_loop();
};

template<typename from_V>
void remove_if_sized(from_V& from_vs, unsigned int condition_size, unsigned int remove_count, VoidFunc callback );

void move_if_sized(Resource::ResourceType res_type, unsigned int condition_size, unsigned int move_count, Building* from_bldg, Building* to_bldng, VoidFunc callback);
#endif
