#pragma once
#ifndef TECHNOLOGY_H
#define TECHNOLOGY_H

#include "constants.h"


#include "SubTypes.h"

using TechMap = std::map<TechSubType, res_count_t>;

///Container and handler for the tech researched
class TechTree
{
    public:
        TechTree();


        vsTechnology technologies;
        TechMap tech_map;

        TechMap& get_tech_map();

};

class Technology
{
    private:
        bool _been_unlocked;
        spRecipe _ing_requirements;

    public:
        using SubType = TechSubType;
        Technology(Technology::SubType sub_type);

        SubType sub_type;
        static const std::map<Technology::SubType, std::string> type_map;
        static std::string type_to_string(SubType sub_type);
        static SubType string_to_type(std::string sub_type);

        //can only be bought once
        bool is_unique;

        bool get_been_unlocked();
        void set_been_unlocked(bool val);

        spRecipe get_ingredient_requirements(spBuilding building);
        void set_ingredient_requirements(spRecipe requirements);
};


#endif
