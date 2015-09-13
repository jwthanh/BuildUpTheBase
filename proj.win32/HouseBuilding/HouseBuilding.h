#pragma once
#ifndef HOUSEBUILDING_H
#define HOUSEBUILDING_H

#include "../../Classes/Clock.h"


class Building;

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
    unsigned int num_workers; //people who work here, help make things faster
    Building(std::string name, VoidFuncBuilding task) : task(task), Nameable(name), Updateable()
    {

        num_workers = 1;
    };

    void do_task()
    {
        if (this->task)
        {
            this->task(this);
        };
    };

    VoidFuncBuilding task = NULL; //shop might sell product, farm creates ingredients, etc
};

class Village : public Nameable, public Updateable
{
public:

    std::vector<std::shared_ptr<Building>> buildings;

    Village(std::string name) : Nameable(name), Updateable()
    {
        buildings = {};
    };


    void update(float dt);


    void run_tasks_once()
    {
        for (std::shared_ptr<Building> building : this->buildings)
        {
            building->do_task();
        };
    };
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
