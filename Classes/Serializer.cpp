#include "Serializer.h"


#include <json/document.h>
#include <json/stringbuffer.h>
#include <json/writer.h>

#include "FileOperation.h"

#include "HouseBuilding.h"
#include <sstream>

BaseSerializer::BaseSerializer(std::string filename)
    : filename(filename)
{

};

rj::Document BaseSerializer::get_document(std::string filename) const
{
    if (filename == "") filename = this->filename;

    return FileIO::open_json(filename, false);
}

void BaseSerializer::save_document(rj::Document& doc, std::string filename) const
{
    if (filename == "") filename = this->filename;

    FileIO::save_json(filename, doc, false);
}

void BaseSerializer::add_member(rj::Document& doc, std::string key, rj::Value& value)
{
    //build the Values that become the key and values
    rapidjson::Document::AllocatorType& allocator = doc.GetAllocator();
    rapidjson::Value doc_key = rapidjson::Value();

    //NOTE need to use allocator here, AND move the key later it seems like,
    //otherwise the value gets cleaned up
    doc_key.SetString(key.c_str(), key.size(), allocator);

    //all the member to the document
    this->_add_member(doc, doc_key, value, allocator);
};

rj::Value& BaseSerializer::get_member(rj::Document& doc, std::string key)
{
    //build the Values that become the key and values
    rapidjson::Document::AllocatorType& allocator = doc.GetAllocator();
    rapidjson::Value doc_key = rapidjson::Value();

    doc_key.SetString(key.c_str(), key.size(), allocator);

    return this->_get_member(doc, doc_key, allocator);
};

void BaseSerializer::_add_member(rj::Document& doc, rj::Value& key, rj::Value& value, rapidjson::Document::AllocatorType& allocator)
{
    doc.AddMember(key.Move(), value.Move(), allocator);
};

rj::Value& BaseSerializer::_get_member(rj::Document& doc, rj::Value& key, rapidjson::Document::AllocatorType& allocator)
{
    rj::Value& value = doc[key];
    return value;
};

void BaseSerializer::set_string(rj::Document & doc, std::string key, std::string value)
{
    rapidjson::Document::AllocatorType& allocator = doc.GetAllocator();
    rapidjson::Value doc_value = rapidjson::Value();
    doc_value.SetString(value.c_str(), value.size(), allocator);
    
    this->add_member(doc, key, doc_value);

};

void BaseSerializer::set_double(rj::Document & doc, std::string key, double value)
{
    rapidjson::Value doc_value = rapidjson::Value();
    doc_value.SetDouble(value);
    
    this->add_member(doc, key, doc_value);

};

void BaseSerializer::set_int(rj::Document & doc, std::string key, int value)
{
    rapidjson::Value doc_value = rapidjson::Value();
    doc_value.SetInt(value);

    this->add_member(doc, key, doc_value);
};

std::string BaseSerializer::get_string(rj::Document & doc, std::string key, std::string fallback)
{
    rj::Value& doc_value = this->get_member(doc, key);
    if (doc_value.IsObject()) 
    {
        //assume its returned a Document, since I don't know how to return a null reference
        return fallback;
    }
    return doc_value.GetString();

};

double BaseSerializer::get_double(rj::Document & doc, std::string key, double fallback)
{
    auto& doc_value = this->get_member(doc, key);
    if (doc_value.IsObject()) 
    {
        //assume its returned a Document, since I don't know how to return a null reference
        return fallback;
    }
    return doc_value.GetDouble();

};

int BaseSerializer::get_int(rj::Document & doc, std::string key, int fallback)
{
    rj::Value& doc_value = this->get_member(doc, key);
    if (doc_value.IsObject()) 
    {
        //assume its returned a Document, since I don't know how to return a null reference
        return fallback;
    }
    return doc_value.GetInt();
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

    this->save_document(doc);
}

void BuildingSerializer::load()
{
    rapidjson::Document doc = this->get_document();

    this->load_building_level(doc);
    this->load_ingredients(doc);
    this->load_workers(doc);
}

void BuildingSerializer::serialize_building_level(rapidjson::Document& doc)
{
    this->set_int(doc, "building_level", this->building->building_level);
}

void BuildingSerializer::load_building_level(rapidjson::Document& doc)
{
    int new_level = this->get_int(doc, "building_level", -1);
    if (new_level != -1)
    {
        //only used saved building level
        this->building->building_level = new_level;
    }
}

void BuildingSerializer::serialize_ingredients(rapidjson::Document& doc)
{
    for (auto mist : this->building->ingredients)
    {
        IngredientSubType sub_type = mist.first;
        res_count_t count = mist.second;

        this->set_double(doc, Ingredient::type_to_string(sub_type), count);
    }
}

void BuildingSerializer::load_ingredients(rapidjson::Document& doc)
{
    //TODO load_ingredients
}

void BuildingSerializer::serialize_workers(rapidjson::Document& doc)
{
    for (auto mist : this->building->harvesters)
    {
        std::pair<WorkerSubType, IngredientSubType> sub_type = mist.first;
        res_count_t count = mist.second;

        std::stringstream ss;
        ss << "harvester_" << Ingredient::type_to_string(sub_type.second) << "_" << static_cast<int>(sub_type.first);
        this->set_double(doc, ss.str(), count);
    }

    for (auto mist : this->building->salesmen)
    {
        std::pair<WorkerSubType, IngredientSubType> sub_type = mist.first;
        res_count_t count = mist.second;

        std::stringstream ss;
        ss << "salesmen_" << Ingredient::type_to_string(sub_type.second) << "_" << static_cast<int>(sub_type.first);
        this->set_double(doc, ss.str(), count);
    }
}

void BuildingSerializer::load_workers(rapidjson::Document& doc)
{
    //TODO load_workers
}

void BuildingSerializer::_add_member(rj::Document& doc, rj::Value& key, rj::Value& value, rapidjson::Document::AllocatorType& allocator)
{
    if (doc.HasMember(this->building->name.c_str()) == false)
    {
        doc.AddMember(
            rj::Value().SetString(rj::StringRef(building->name.c_str())),
            rj::Value(rj::kObjectType).Move(),
            allocator
        );
    }
    doc[building->name.c_str()].AddMember(key.Move(), value.Move(), allocator);
};

rj::Value& BuildingSerializer::_get_member(rj::Document& doc, rj::Value& key, rapidjson::Document::AllocatorType& allocator)
{
    if (doc.HasMember(this->building->name.c_str()) == false)
    {
        return doc;
    }
    return doc[building->name.c_str()][key];
};
