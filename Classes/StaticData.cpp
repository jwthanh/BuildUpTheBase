#include "StaticData.h"

#include <json/document.h>
#include <json/reader.h>

#include "FileOperation.h"

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
    this->_filename = "buildings.json";
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
