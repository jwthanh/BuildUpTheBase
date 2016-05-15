#include "Technology.h"


TechTree::TechTree()
    : technologies({})
{

};

Technology::Technology()
    : _been_unlocked(false),
    _ing_requirements({})
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

mistIngredient Technology::get_ingredient_requirements()
{
    return this->_ing_requirements;
};

void Technology::set_ingredient_requirements(mistIngredient requirements)
{
    this->_ing_requirements = requirements;
};
