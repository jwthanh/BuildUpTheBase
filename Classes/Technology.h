#pragma once
#ifndef TECHNOLOGY_H
#define TECHNOLOGY_H

#include "constants.h"

#include "Resources.h"

#include "SubTypes.h"

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
        spRecipe _ing_requirements;

    public:
        Technology();

        using SubType = TechSubType;

        bool get_been_unlocked();
        void set_been_unlocked(bool val);

        spRecipe get_ingredient_requirements();
        void set_ingredient_requirements(spRecipe requirements);
};

using TechMap = std::map<Technology::SubType, bool>;


#endif
