#include "StaticData.h"
#include <json/document.h>
#include <json/reader.h>

#include "FileOperation.h"

std::string BaseStaticData::_filename = "buildings_data.json";

std::string BaseStaticData::get_data(std::string key_top, std::string key_child="", std::string key_grandchild="")
{
    auto jsonDoc = FileIO::open_json(filename());

    auto body = &jsonDoc[key_top.c_str()];

    if (key_child == "") { return body->GetString(); };

    auto child = &(*body)[key_child.c_str()];
    if (key_grandchild == "") { return child->GetString(); };

    auto grand_child = &(*child)[key_grandchild.c_str()];
    
    return grand_child->GetString();
}
