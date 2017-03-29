#pragma once
#ifndef STATICDATA_H
#define STATICDATA_H

#include <string>

#include "constants.h"
#include <json/allocators.h>
#include <json/document.h>


#define GET_JSON_ATTR(attr_name) \
std::string get_##attr_name()\
{\
    return this->getter(#attr_name);\
}\

/// Read only data, do not write, since there's so much caching
class BaseStaticData
{
    private:
        std::map<std::string, std::string> _cache;

    protected:
        std::string _filename;
        std::string _get_data(std::string key_top, std::string key_child="", std::string key_grandchild="");

    public:
		rjDocument jsonDoc; //should be cached on contruction
		BaseStaticData(std::string filename);
		BaseStaticData(const BaseStaticData&);

        virtual std::string getter(std::string key) = 0;
};

class BuildingData : public BaseStaticData
{
    private:

    public:
        std::string building_name;

        BuildingData(std::string building_name);

        std::string getter(std::string key) override;

        GET_JSON_ATTR(task_name);
        GET_JSON_ATTR(description);
        GET_JSON_ATTR(gold_cost);
        GET_JSON_ATTR(img_large);
        GET_JSON_ATTR(short_name);

        vsRecipe get_all_recipes();
        spRecipe build_recipe(rjValue* recipe_data);
        spRecipe get_recipe(std::string recipe_key);
};

class IngredientData : public BaseStaticData
{
    public:
        std::string resource_id;
        IngredientData(std::string resource_id);

        std::string getter(std::string key) override;

        GET_JSON_ATTR(name);
        GET_JSON_ATTR(description);
        GET_JSON_ATTR(img_large);

};

class ItemData : public BaseStaticData
{
    public:
        std::string item_id;
        ItemData();

        std::string getter(std::string key) override;

        spItem build_item(std::string type_name, rjValue* recipe_data);
        spItem get_item(std::string item_id);

        vsItem get_all_items();

};

#endif
