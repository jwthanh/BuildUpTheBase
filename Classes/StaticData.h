#pragma once
#ifndef STATICDATA_H
#define STATICDATA_H

#include <string>

#include "constants.h"
#include <json/allocators.h>
#include <json/document.h>

class BaseData
{
    private:
        std::map<std::string, std::string> _cache;
    protected:
        std::string _filename;
        std::string _get_data(std::string key_top, std::string key_child="", std::string key_grandchild="");
    public:

        //BaseData(){};

        virtual std::string getter(std::string key) = 0;
};

class BuildingData : public BaseData
{
    private:
        rjDocument recipe_doc;

    public:
        std::string building_name;

        BuildingData(std::string building_name);

        std::string getter(std::string key) override;

        std::string get_task_name();
        std::string get_description();
        std::string get_gold_cost();
        std::string get_img_large();

        vsRecipe get_all_recipes();
        spRecipe build_recipe(rapidjson::GenericValue<rapidjson::UTF8<>, rapidjson::CrtAllocator>* recipe_data);
        spRecipe get_recipe(std::string recipe_key);
};

class IngredientData : public BaseData
{
    public:
        std::string resource_id;
        IngredientData(std::string resource_id);

        std::string getter(std::string key) override;

        std::string get_description();
        std::string get_img_large();

};

#endif
