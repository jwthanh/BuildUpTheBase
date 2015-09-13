#pragma once
#ifndef HOUSEBUILDING_H
#define HOUSEBUILDING_H

#include "../../Classes/Clock.h"


class Building;
class Buildup;
class Village;

typedef void(*VoidFuncBuilding)(const Building*);
typedef bool(*BoolFuncBuilding)(const Building*);

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
};

class Building : public Nameable, public Updateable
{
    public:

        Village* city;

        unsigned int num_workers; //people who work here, help make things faster
        VoidFuncBuilding task = NULL; //shop might sell product, farm creates ingredients, etc
        Building(Village* city, std::string name, VoidFuncBuilding task)
            : task(task), Nameable(name), Updateable(), city(city)
        {
            num_workers = 1;
        };

        void update(float dt);

        void do_task();

};

class Village : public Nameable, public Updateable
{
    public:

        Buildup* buildup;

        std::vector<std::shared_ptr<Building>> buildings;

        Village(Buildup* buildup, std::string name) 
            : Nameable(name), Updateable(), buildup(buildup)
        {
            buildings = {};
        };
        void update(float dt);
        void update_buildings(float dt);
};


class Animal : public Nameable
{
    public:
        Animal(std::string name) : Nameable(name) {};
};

class Person : public Nameable
{
    public:
        Person(std::string name) : Nameable(name) {};
};

class Player : public Person
{
    public:
        unsigned int coins;
        Player(std::string name) : Person(name) {};
};

class Ingredient : public Nameable
{
    Ingredient(std::string name) : Nameable(name) {};
};

class Product : public Nameable
{
    Product(std::string name) : Nameable(name) {};
};

class Waste : public Nameable
{
    Waste() : Nameable("Waste") {};
};

class Buildup
{
    public:
        Village* city;

        Buildup();
        void main_loop();
};
#endif
