// ConsoleApplication1.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <memory>
#include <string>
#include <vector>
#include <iostream>

class Building;

typedef void(*VoidFuncBuilding)(const Building*);
typedef bool(*BoolFuncBuilding)(const Building*);

class Nameable
{
    public:
        std::string name;

        Nameable(std::string name) :name(name) {};
};

class Building : public Nameable
{
    public:
        unsigned int num_workers; //people who work here, help make things faster
        Building(std::string name, VoidFuncBuilding task) : task(task), Nameable(name)
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

class Village : public Nameable
{
    public:
        Village(std::string name) : Nameable(name)
    {
        buildings = {};
    };

        void run_tasks_once()
        {
            for (std::shared_ptr<Building> building : this->buildings)
            {
                building->do_task();
            };
        };

        std::vector<std::shared_ptr<Building>> buildings = NULL;

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


void farm_task(const Building* building)
{
    std::cout << "Doing farm stuff" << std::endl;
};

void workshop_task(const Building* building)
{
    std::cout << "Doing workshop stuff" << std::endl;
};

void dump_task(const Building* building)
{
    std::cout << "Doing dump stuff" << std::endl;
};

void marketplace_task(const Building* building)
{
    std::cout << "Doing marketplace stuff" << std::endl;
};


int _tmain(int argc, _TCHAR* argv[])
{
    auto city = std::make_shared<Village>("Burlington");

    auto farm = std::make_shared<Building>("The Farm", farm_task);
    auto dump= std::make_shared<Building>("The Dump", dump_task);
    auto workshop = std::make_shared<Building>("The Workshop", workshop_task);
    auto marketplace = std::make_shared<Building>("The Marketplace", marketplace_task);

    city->buildings.push_back(farm);
    city->buildings.push_back(dump);
    city->buildings.push_back(workshop);
    city->buildings.push_back(marketplace);

    city->run_tasks_once();


    auto player = std::make_shared<Player>("Jimothy");
    player->coins = 100;

    auto animal = std::make_shared<Animal>("Tank");

    std::cout << std::endl << "Done?";
    std::string val;
    std::cin.ignore();
    std::cout << "Peace" << std::endl;


    return 0;
}

