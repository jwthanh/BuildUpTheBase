#include "Technology.h"

#include "cocos2d.h"
#include "HouseBuilding.h"
#include "Recipe.h"
#include "Util.h"


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
    sub_type(sub_type),
    is_unique(false)
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

spRecipe Technology::get_ingredient_requirements(spBuilding building)
{
    spRecipe recipe;
    std::stringstream ss;
    if (this->sub_type == Technology::SubType::CombatDamage)
    {
        recipe = std::make_shared<Recipe>("combat_damage", "no desc for tech recipe");
        auto tech_map = building->techtree->tech_map;
        res_count_t _def = 0;
        res_count_t souls_cost = std::floor(scale_number(2.0L, map_get(tech_map, this->sub_type, _def), 1.45L));
        recipe->components = mistIngredient({{ Ingredient::SubType::Soul, souls_cost }});
        ss << "Increases combat damage\n-- costs " << souls_cost << " souls";
    }
    else if (this->sub_type == Technology::SubType::ClickDoublePower)
    {
        recipe = std::make_shared<Recipe>("double_click_power", "no desc for tech recipe");

        auto tech_map = building->techtree->tech_map;
        res_count_t _def = 0;
        res_count_t paper_cost = std::floor(scale_number(2.0L, map_get(tech_map, this->sub_type, _def), 9.45L));
        recipe->components = mistIngredient({{ Ingredient::SubType::Paper, paper_cost }});

        ss << "Doubles click output here\n-- costs " << paper_cost << " paper";
    }
    else
    {
        CCLOG("ERR: missing tech sub type");
    }

    recipe->description = ss.str();
    this->_ing_requirements = recipe;

    return this->_ing_requirements;
};

void Technology::set_ingredient_requirements(spRecipe requirements)
{
    this->_ing_requirements = requirements;
};
