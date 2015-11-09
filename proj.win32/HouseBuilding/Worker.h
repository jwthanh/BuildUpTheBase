#pragma once
#ifndef WORKER_H
#define WORKER_H

#include "HouseBuilding.h"

template<typename Rtype>
class ResourceCondition : public Nameable
{
    public:
        enum TypeChoices {
            Any = 0,
            Ingredient,
            Product, 
            Waste
        };
        TypeChoices type_choice;
        Rtype choice;

        int quantity;

        ResourceCondition(TypeChoices type_choice, Rtype choice, int quantity, std::string name) : Nameable(name) {
            this->type_choice;
            this->quantity = quantity;
            this->choice = choice;
        };

        bool is_satisfied(spBuilding building) {
            if (this->type_choice == Ingredient)
            {
                return building->ingredients.size() >= this->quantity;
            }
            else if (this->type_choice == Product)
            {
                return building->products.size() >= this->quantity;
            }
            else if (this->type_choice == Ingredient)
            {
                return building->wastes.size() >= this->quantity;
            }
            else
            {
                unsigned int size = building->ingredients.size() +
                    building->products.size() +
                    building->wastes.size();
                return size >= this->quantity;
            };
        };
};

//
//attaches to a building and builds recipes once certain conditions are hit
//
class Worker : public Nameable
{
    public:
        spBuilding building;

        Worker(spBuilding building, std::string name) : Nameable(name) {
            this->building = building;
        };
};

#endif
