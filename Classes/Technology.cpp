#include "Technology.h"

#include "cocos2d.h"
#include "HouseBuilding.h"
#include "Recipe.h"
#include "Util.h"

const std::map<Technology::SubType, std::string> Technology::type_map = {
    {Technology::SubType::None, "none_tech_subtype"},
    {Technology::SubType::ClickDoublePower, "click_dbl_power"},
    {Technology::SubType::CombatDamage, "combat_damage"},
    {Technology::SubType::CombatArmor, "combat_armor"},
    {Technology::SubType::CombatCritChance, "combat_crit_chance"},
    {Technology::SubType::CombatCritFactor, "combat_crit_factor"}
};


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
        ss << "Increases combat damage\n-- costs " << beautify_double(souls_cost) << " souls";
    }
    else if (this->sub_type == Technology::SubType::CombatCritChance)
    {
        recipe = std::make_shared<Recipe>("combat_crit_chance", "no desc for tech recipe");
        auto tech_map = building->techtree->tech_map;
        res_count_t _def = 0;
        res_count_t blood_cost = std::floor(scale_number(2.0L, map_get(tech_map, this->sub_type, _def), 2.45L));
        recipe->components = mistIngredient({{ Ingredient::SubType::Blood, blood_cost }});
        ss << "Raises chance to WOOSH\n-- costs " << beautify_double(blood_cost) << " blood";
    }
    else if (this->sub_type == Technology::SubType::CombatCritFactor)
    {
        recipe = std::make_shared<Recipe>("combat_crit_factor", "no desc for tech recipe");
        auto tech_map = building->techtree->tech_map;
        res_count_t _def = 0;
        res_count_t fly_cost = std::floor(scale_number(2.0L, map_get(tech_map, this->sub_type, _def), 2.45L));
        recipe->components = mistIngredient({{ Ingredient::SubType::Fly, fly_cost }});
        ss << "Improves WOOSH damage\n-- costs " << beautify_double(fly_cost) << " flies";
    }
    else if (this->sub_type == Technology::SubType::ClickDoublePower)
    {
        recipe = std::make_shared<Recipe>("double_click_power", "no desc for tech recipe");

        auto tech_map = building->techtree->tech_map;
        res_count_t _def = 0;
        res_count_t paper_cost = std::floor(scale_number(2.0L, map_get(tech_map, this->sub_type, _def), 9.45L));
        recipe->components = mistIngredient({{ Ingredient::SubType::Paper, paper_cost }});

        ss << "Doubles click output here\n-- costs " << beautify_double(paper_cost) << " paper";
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

std::string Technology::type_to_string(Technology::SubType type)
{
    std::string result = "none";
    for (auto pair : Technology::type_map)
    {
        if (type == pair.first)
        {
            return pair.second;
        }
    }

    return result;
};

Technology::SubType Technology::string_to_type(std::string string_type)
{
    std::transform(string_type.begin(), string_type.end(), string_type.begin(), ::tolower);
    for (auto pair : Technology::type_map)
    {
        if (pair.second == string_type)
        {
            return pair.first;
        }
    }
    printj("type id " << string_type);
    assert(false && "unknown type");

    Technology::SubType result = Technology::SubType::None;
    return result;
};
