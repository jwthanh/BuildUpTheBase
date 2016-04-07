#include "StaticData.h"

#include <json/document.h>
#include <json/reader.h>

#include "FileOperation.h"

#include "cocos2d.h"
#include "Recipe.h"

std::string BaseData::_get_data(std::string key_top, std::string key_child, std::string key_grandchild)
{
    auto jsonDoc = FileIO::open_json(this->_filename);

    auto body = &jsonDoc[key_top.c_str()];

    if (key_child == "") { return body->GetString(); };

    auto child = &(*body)[key_child.c_str()];
    if (key_grandchild == "") { return child->GetString(); };

    auto grand_child = &(*child)[key_grandchild.c_str()];
    
    return grand_child->GetString();
}

BuildingData::BuildingData(std::string building_name)
{
    this->building_name = building_name;
    this->_filename = "buildings_data.json";
}

std::string BuildingData::getter(std::string key)
{
    return this->_get_data("buildings", building_name, key);
};

std::string BuildingData::get_task_name()
{
    return this->getter("task_name");
};

std::string BuildingData::get_description()
{
    return this->getter("description");
};

std::string BuildingData::get_gold_cost()
{
    return this->getter("gold_cost");
};

std::string BuildingData::get_img_large()
{
    return this->getter("img_large");
};

//vsRecipe BuildingData::get_all_recipes()
//{
//    auto jsonDoc = FileIO::open_json(this->_filename);
//    auto body = &jsonDoc["buildings"];
//    auto building_info = &(*body)[this->building_name.c_str()];
//    auto recipe_info = &(*building_info)["recipes"];
//
//    for (rapidjson::Value::MemberIterator itr = recipe_info->MemberBegin();
//        itr != recipe_info->MemberEnd();
//        itr+=1)
//    {
//        std::string recipe_id = itr->name.GetString();
//        auto recipe_data = &(itr->value)[recipe_id.c_str()];
//
//        auto recipe_name = &(*recipe_data)["name"];
//        auto recipe_components = &(*recipe_data)["components"];
//
//    };
//
//};

spRecipe BuildingData::build_recipe(rapidjson::GenericValue<rapidjson::UTF8<>>* recipe_data)
{
    auto recipe_name = &(*recipe_data)["name"];
    auto recipe_components = &(*recipe_data)["components"];

    spRecipe result = std::make_shared<Recipe>();

    CCLOG("Components: ");
    for (rapidjson::Value::MemberIterator itr = recipe_components->MemberBegin();
         itr != recipe_components->MemberEnd();
         itr+=1)
    {
        std::string val = itr->name.GetString();
        int count = itr->value.GetInt();
        CCLOG("\t%s: %d", val.c_str(), count);
        result->components[Ingredient::string_to_type(val)] = count;
    };

    CCLOG("Output: ");
    auto recipe_output = &(*recipe_data)["output"];
    for (rapidjson::Value::MemberIterator itr = recipe_output->MemberBegin();
         itr != recipe_output->MemberEnd();
         itr+=1)
    {
        std::string val = itr->name.GetString();
        int count = itr->value.GetInt();
        CCLOG("\t%s: %d", val.c_str(), count);
        result->outputs[Ingredient::string_to_type(val)] = count;
    };

    CCLOG("raw val: %s", recipe_name->GetString());

    return result;
}

spRecipe BuildingData::get_recipe(std::string recipe_key)
{

    auto jsonDoc = FileIO::open_json(this->_filename);
    auto body = &jsonDoc["buildings"];
    auto building_info = &(*body)[this->building_name.c_str()];
    auto recipe_info = &(*building_info)["recipes"];

    auto recipe_data = &(*recipe_info)[recipe_key.c_str()];

    return build_recipe(recipe_data);
};
