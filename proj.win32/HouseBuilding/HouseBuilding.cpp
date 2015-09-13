// ConsoleApplication1.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <memory>
#include <string>
#include <vector>
#include <iostream>

#include "HouseBuilding.h"


Updateable::Updateable()
{
    this->update_clock = new Clock(15.0f);
};

void Updateable::update(float dt)
{
    this->update_clock->update(dt);
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

void main_update(float dt)
{
    

}


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

