#include "Technology.h"


TechTree::TechTree()
    : technologies({}), tech_map({})
{

};

TechMap TechTree::get_tech_map()
{
    return this->tech_map;
    // TechMap tech_map;

    // for (auto tech : this->technologies) {
    //     tech_map[tech->sub_type] = tech->get_been_unlocked();
    // };

    // return tech_map;
};

Technology::Technology(Technology::SubType sub_type)
    : _been_unlocked(false),
    _ing_requirements({}),
    sub_type(sub_type)
{

};

void Technology::set_been_unlocked(bool val)
{
    this->_been_unlocked = val;
};

bool Technology::get_been_unlocked()
{
    return this->_been_unlocked;
};

spRecipe Technology::get_ingredient_requirements()
{
    return this->_ing_requirements;
};

void Technology::set_ingredient_requirements(spRecipe requirements)
{
    this->_ing_requirements = requirements;
};
