#pragma once
#ifndef ANIMAL_H
#define ANIMAL_H

#include "Nameable.h"
#include "constants.h"
#include "Resources.h"

class Animal : public Nameable
{
    public:
        Animal(std::string name) : Nameable(name) {};

        //deprecated I guess
        void b2b_transfer(spBuilding from_bldg, spBuilding to_bldg, Resource::ResourceType res_type, int quantity);

        void transfer_ingredients(spBuilding from_bldg, spBuilding to_bldg, Ingredient::SubType sub_type, int quantity);
        void transfer_products(spBuilding from_bldg, spBuilding to_bldg, Product::SubType sub_type, int quantity);
        void transfer_wastes(spBuilding from_bldg, spBuilding to_bldg, Waste::SubType sub_type, int quantity);
};

#endif
