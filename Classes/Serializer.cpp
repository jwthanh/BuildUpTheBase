#include "Serializer.h"


#include <json/document.h>
#include <json/stringbuffer.h>
#include <json/writer.h>

#include "cocos2d.h"

#include "FileOperation.h"

#include "HouseBuilding.h"

BaseSerializer::BaseSerializer(std::string filename)
    : filename(filename)
{

};

rj::Document BaseSerializer::get_document(std::string filename) const
{
    if (filename == "") filename = this->filename;

    return FileIO::open_json(filename);
}

void BaseSerializer::save_document(rj::Document& doc, std::string filename) const
{
    if (filename == "") filename = this->filename;

    //TODO make this use a document
    FileIO::save_json(filename, doc);
}

void BaseSerializer::set_string(rj::Document doc, std::string key, std::string value)
{
    //build the Values that become the key and values
    rapidjson::Value doc_key = rapidjson::Value();
    doc_key.SetString(key.c_str(), key.size());

    rapidjson::Value doc_value = rapidjson::Value();
    doc_value.SetString(value.c_str(), value.size());

    //all the member to the document
    rapidjson::Document::AllocatorType& allocator = doc.GetAllocator();
    doc.AddMember(doc_key, doc_value, allocator);

};

void BaseSerializer::set_double(rj::Document doc, std::string key, double value)
{
    //build the Values that become the key and values
    rapidjson::Value doc_key = rapidjson::Value();
    doc_key.SetString(key.c_str(), key.size());

    rapidjson::Value doc_value = rapidjson::Value();
    doc_value.SetDouble(value);

    //all the member to the document
    rapidjson::Document::AllocatorType& allocator = doc.GetAllocator();
    doc.AddMember(doc_key, doc_value, allocator);

};

void BaseSerializer::set_int(rj::Document doc, std::string key, int value)
{
    //build the Values that become the key and values
    rapidjson::Value doc_key = rapidjson::Value();
    doc_key.SetString(key.c_str(), key.size());

    rapidjson::Value doc_value = rapidjson::Value();
    doc_value.SetInt(value);

    //all the member to the document
    rapidjson::Document::AllocatorType& allocator = doc.GetAllocator();
    doc.AddMember(doc_key, doc_value, allocator);

};

BuildingSerializer::BuildingSerializer(std::string filename, spBuilding building)
    : BaseSerializer(filename), building(building)
{
}

void BuildingSerializer::serialize()
{
    rapidjson::Document doc = rapidjson::Document();
    doc.SetObject();

    this->serialize_building_level(doc);
    this->serialize_ingredients(doc);
    this->serialize_workers(doc);

    this->save_document(doc, this->building->name + "_serialized.json");
}

void BuildingSerializer::serialize_building_level(rapidjson::Document& doc)
{
}

void BuildingSerializer::serialize_ingredients(rapidjson::Document& doc)
{
}

void BuildingSerializer::serialize_workers(rapidjson::Document& doc)
{
}
