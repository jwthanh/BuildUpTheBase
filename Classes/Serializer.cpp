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

    FileIO::save_json(filename, doc);
}

void BaseSerializer::set_string(rj::Document & doc, std::string key, std::string value)
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

void BaseSerializer::set_double(rj::Document & doc, std::string key, double value)
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

void BaseSerializer::set_int(rj::Document & doc, std::string key, int value)
{
    //build the Values that become the key and values
    rapidjson::Value doc_key = rapidjson::Value();
    doc_key.SetString(key.c_str(), key.size());

    rapidjson::Value doc_value = rapidjson::Value();
    doc_value.SetInt(value);

    //all the member to the document
    rapidjson::Document::AllocatorType& allocator = doc.GetAllocator();
    doc.AddMember(doc_key, doc_value, allocator);

    auto file_utils = cocos2d::FileUtils::getInstance();

    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    doc.Accept(writer);
    cocos2d::Data data;
    CCLOG("size: %i", buffer.GetSize());
    std::string str(buffer.GetString(), buffer.GetSize());
    CCLOG("buffer json being written:\n%s", str.c_str());
    file_utils->writeStringToFile(str, this->filename);

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

    //this->save_document(doc);
}

void BuildingSerializer::serialize_building_level(rapidjson::Document& doc)
{
    this->set_int(doc, "building_level", this->building->building_level);
}

void BuildingSerializer::serialize_ingredients(rapidjson::Document& doc)
{
}

void BuildingSerializer::serialize_workers(rapidjson::Document& doc)
{
}
