#include "StaticData.h"

#include <json/document.h>
#include <json/reader.h>

#include <sstream>

#include "FileOperation.h"

#include "Recipe.h"
#include "Item.h"

std::string BaseData::_get_data(std::string key_top, std::string key_child, std::string key_grandchild)
{

    std::stringstream ss;
    ss << key_top << "__" << key_child << "__" << key_grandchild;
    std::string cache_key = ss.str();

    if (this->_cache.find(cache_key) != this->_cache.end())
    {
        return this->_cache.at(cache_key);
    }


    rjDocument jsonDoc = FileIO::open_json(this->_filename, true);

    CC_ASSERT(jsonDoc.HasMember(key_top.c_str()) && "this should be here");
    auto body = &jsonDoc[key_top.c_str()];

    if (key_child == "")
    {
        this->_cache[cache_key] = body->GetString();
        return body->GetString();
    };

    auto child = &(*body)[key_child.c_str()];
    if (key_grandchild == "")
    {
        this->_cache[cache_key] = child->GetString();
        return child->GetString();
    };

    auto grand_child = &(*child)[key_grandchild.c_str()];
    
    this->_cache[cache_key] = grand_child->GetString();
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

vsRecipe BuildingData::get_all_recipes()
{
    //assume if its not an object, its null
    if (this->recipe_doc.IsObject() == false)
    {
        this->recipe_doc = FileIO::open_json(this->_filename, true);
    }

    auto& body = this->recipe_doc["buildings"];
    auto& building_info = body[this->building_name.c_str()];

    if (!building_info.HasMember("recipes"))
    {
        return vsRecipe();
    }
    auto& recipe_info = building_info["recipes"];

    vsRecipe recipes;

    for (auto itr = recipe_info.MemberBegin();
        itr != recipe_info.MemberEnd();
        itr += 1)
    {
        std::string recipe_id = itr->name.GetString();
        auto recipe_data = &itr->value;

        recipes.push_back(this->build_recipe(recipe_data));

    };

    return recipes;

};

spRecipe BuildingData::build_recipe(rjValue* raw_data)
{
    rjValue& recipe_ref = *raw_data;
    rjValue& recipe_components = recipe_ref["components"];

    auto recipe_name = recipe_ref["name"].GetString();
    auto recipe_description = recipe_ref["description"].GetString();
    spRecipe result = std::make_shared<Recipe>(recipe_name, recipe_description);

    if (recipe_ref.HasMember("clicks_required"))
    {
        result->clicks_required = recipe_ref["clicks_required"].GetInt();
    }


    for (auto itr = recipe_components.MemberBegin();
         itr != recipe_components.MemberEnd();
         itr+=1)
    {
        std::string val = itr->name.GetString();
        int count = itr->value.GetInt();
        result->components[Ingredient::string_to_type(val)] = count;
    };

    rjValue& recipe_output = recipe_ref["output"];
    for (auto itr = recipe_output.MemberBegin();
         itr != recipe_output.MemberEnd();
         itr+=1)
    {
        std::string val = itr->name.GetString();
        int count = itr->value.GetInt();
        result->outputs[Ingredient::string_to_type(val)] = count;
    };

    return result;
}

spRecipe BuildingData::get_recipe(std::string recipe_key)
{

    auto jsonDoc = FileIO::open_json(this->_filename, true);
    auto& body = jsonDoc["buildings"];
    auto& building_info = body[this->building_name.c_str()];
    auto& recipe_info = building_info["recipes"];

    auto& recipe_data = recipe_info[recipe_key.c_str()];

    return build_recipe(&recipe_data);
};


IngredientData::IngredientData(std::string resource_id)
{
    this->resource_id = resource_id;
    this->_filename = "resources_data.json";
}

std::string IngredientData::getter(std::string key)
{
    return this->_get_data("resources", this->resource_id, key);
};

ItemData::ItemData()
{
    this->_filename = "item_data.json";
}

std::string ItemData::getter(std::string key)
{
    return "unimplemented";
};

spItem ItemData::build_item(rjValue* item_data)
{
    auto& item_ref = *item_data;

    auto name = item_ref["name"].GetString();
    auto summary = item_ref["summary"].GetString();
    auto description = item_ref["description"].GetString();
    auto base_cost = item_ref["base_cost"].GetDouble();

    return std::make_shared<Item>(name, summary, description, base_cost, RarityType::Normal, 1);
};

spItem ItemData::get_item(std::string item_id)
{

    auto jsonDoc = FileIO::open_json(this->_filename, true);
    auto body = &jsonDoc["items"];
    auto item_info = &(*body)[item_id.c_str()];

    return this->build_item(item_info);
};
