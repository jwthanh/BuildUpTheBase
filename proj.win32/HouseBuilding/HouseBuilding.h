#pragma once
#ifndef HOUSEBUILDING_H
#define HOUSEBUILDING_H

#include "../../Classes/Clock.h"
#include "../../Classes/attribute.h"
#include "../../Classes/attribute_container.h"


class Building;
class Buildup;
class Village;
class Product;
class Ingredient;
class Waste;
class Resource;
class Fighter;
class Battle;

typedef void(*VoidFuncBuilding)(Building*);
typedef bool(*BoolFuncBuilding)(Building*);

typedef std::shared_ptr<Building> spBuilding;
typedef std::vector<spBuilding> vsBuilding;

typedef std::shared_ptr<Resource> spResource;
typedef std::vector<spResource> vsResource;

typedef std::shared_ptr<Product> spProduct;
typedef std::vector<spProduct> vsProduct;

typedef std::shared_ptr<Ingredient> spIngredient;
typedef std::vector<spIngredient> vsIngredient;

typedef std::shared_ptr<Waste> spWaste;
typedef std::vector<spWaste> vsWaste;

typedef std::shared_ptr<Battle> spBattle;
typedef std::vector<spBattle> vsBattle;

typedef std::shared_ptr<Fighter> spFighter;
typedef std::vector<spFighter> vsFighter;

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

        static const ResourceType resource_type;

        Resource(std::string name) :Nameable(name){};
};


class Ingredient : public Resource
{
    public:
        static const ResourceType resource_type = Resource::ResourceType::Ingredient;
        Ingredient(std::string name) : Resource(name) {};
};

class Product : public Resource
{
    public:
        enum ProductTypes {
            Veggies,
            Meat,
            Dairy,
            Cereals
        };
        Product(std::string name) : Resource(name) {};

        Product(const Product& other) : Resource(other) {
        };
};

class Waste : public Resource
{
    public:
        Waste(std::string name) : Resource(name) {};
};

class Fighter : public Nameable, public Updateable
{
    public:
        AttributeContainer* attrs;
        float damage;

        Village* city;
        Fighter(Village* city, std::string name) : Nameable(name), Updateable(), city(city)
        {
            this->attrs = new AttributeContainer();
        }
        void update(float dt);
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

    void do_battle()
    {
        if (this->combatants.size() != 0)
        {
            std::cout << "A fight between";
            for (auto fighter : this->combatants)
            {

                std::cout << " ..." << fighter->name;
                fighter->attrs->health->current_val -= 10;
                std::cout << " at " << fighter->attrs->health->current_val << " hp;";
            }
            std::cout << "... and that's it!" << std::endl;
        }
        else
        {
            std::cout << "no one to fight" << std::endl;
        }
        return;
    }
};

class Building : public Nameable, public Updateable
{
    public:

        Village* city;

        vsProduct products;
        vsWaste wastes;
        vsIngredient ingredients;

        vsFighter fighters;

        unsigned int num_workers; //people who work here, help make things faster
        VoidFuncBuilding task = NULL; //shop might sell product, farm creates ingredients, etc
        Building(Village* city, std::string name, VoidFuncBuilding task)
            : task(task), Nameable(name), Updateable(), city(city)
        {
            num_workers = 1;

            products = vsProduct();
            wastes = vsWaste();
            ingredients = vsIngredient();

            fighters = vsFighter();
        };

        void create_resources(Resource::ResourceType type, int quantity, std::string name);

        void update(float dt);
        void print_inventory();

        void do_task();

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
#endif
