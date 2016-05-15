#pragma once
#ifndef TECHNOLOGY_H
#define TECHNOLOGY_H

#include "constants.h"

#include "Resources.h"


///Container and handler for the tech researched
class TechTree
{
    public:
        TechTree();

        vsTechnology technologies;

};

class Technology
{
    private:
        bool _been_unlocked;
        mistIngredient _ing_requirements;

    public:
        Technology();

        bool get_been_unlocked();
        void set_been_unlocked(bool val);

        mistIngredient get_ingredient_requirements();
};

#endif
