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

        void b2b_transfer(spBuilding from_bldg, spBuilding to_bldg, Resource::ResourceType res_type, int quantity);
};

#endif
