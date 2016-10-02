#include "Serializer.h"
#include <sstream>

#include <json/document.h>
#include <json/stringbuffer.h>
#include <json/writer.h>

#include "cocos2d.h"

#include "FileOperation.h"

#include "HouseBuilding.h"

#include "Technology.h"
#include "GameLogic.h"
#include "Item.h"
#include "StaticData.h"

BaseSerializer::BaseSerializer(std::string filename)
    : filename(filename)
{

};

rjDocument BaseSerializer::get_document(std::string filename) const
{
    if (filename == "") filename = this->filename;

    return FileIO::open_json(filename, false);
}

void BaseSerializer::save_document(rjDocument& doc, std::string filename) const
{
    if (filename == "") filename = this->filename;

    FileIO::save_json(filename, doc, false);
}

void BaseSerializer::add_member(rjDocument& doc, std::string key, rjValue& value)
{
    //build the Values that become the key and values
    auto& allocator = doc.GetAllocator();
    rjValue doc_key = rjValue();

    //NOTE need to use allocator here, AND move the key later it seems like,
    //otherwise the value gets cleaned up
    doc_key.SetString(key.c_str(), key.size(), allocator);

    //all the member to the document
    this->_add_member(doc, doc_key, value, allocator);
};

rjValue& BaseSerializer::get_member(rjDocument& doc, std::string key)
{
    //build the Values that become the key and values
    rjDocument::AllocatorType& allocator = doc.GetAllocator();
    rjValue doc_key = rjValue();

    doc_key.SetString(key.c_str(), key.size());

    return this->_get_member(doc, doc_key, allocator);
};

void BaseSerializer::_add_member(rjDocument& doc, rjValue& key, rjValue& value, rapidjson::CrtAllocator& allocator)
{
    doc.AddMember(key.Move(), value.Move(), allocator);
};

rjValue& BaseSerializer::_get_member(rjDocument& doc, rjValue& key, rjDocument::AllocatorType& allocator)
{
    rjValue& value = doc[key];
    return value;
};

void BaseSerializer::set_string(rjDocument & doc, std::string key, std::string value)
{
    rjDocument::AllocatorType& allocator = doc.GetAllocator();
    rjValue doc_value = rjValue();
    doc_value.SetString(value.c_str(), value.size(), allocator);
    
    this->add_member(doc, key, doc_value);

};

void BaseSerializer::set_double(rjDocument & doc, std::string key, double value)
{
    rjValue doc_value = rjValue();
    doc_value.SetDouble(value);
    
    this->add_member(doc, key, doc_value);

};

void BaseSerializer::set_int(rjDocument & doc, std::string key, int value)
{
    rjValue doc_value = rjValue();
    doc_value.SetInt(value);

    this->add_member(doc, key, doc_value);
};

std::string BaseSerializer::get_string(rjDocument & doc, std::string key, std::string fallback)
{
    rjValue& doc_value = this->get_member(doc, key);
    if (doc_value.IsObject()) 
    {
        //assume its returned a Document, since I don't know how to return a null reference
        return fallback;
    }
    return doc_value.GetString();

};

double BaseSerializer::get_double(rjDocument & doc, std::string key, double fallback)
{
    auto& doc_value = this->get_member(doc, key);
    if (doc_value.IsObject()) 
    {
        //assume its returned a Document, since I don't know how to return a null reference
        return fallback;
    }
    return doc_value.GetDouble();

};

int BaseSerializer::get_int(rjDocument & doc, std::string key, int fallback)
{
    rjValue& doc_value = this->get_member(doc, key);
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
    rjDocument doc = rjDocument();
    doc.SetObject();

    this->serialize_building_level(doc);
    this->serialize_ingredients(doc);
    this->serialize_workers(doc);
    this->serialize_tech(doc);

    this->save_document(doc);
}

void BuildingSerializer::load()
{
    rjDocument doc = this->get_document();
    //dont do work if there's nothing to do
    if (doc.IsObject() == false){ return; }

    this->load_building_level(doc);
    this->load_ingredients(doc);
    this->load_workers(doc);
    this->load_tech(doc);
}

void BuildingSerializer::serialize_building_level(rjDocument& doc)
{
    this->set_int(doc, "building_level", this->building->building_level);
}

void BuildingSerializer::load_building_level(rjDocument& doc)
{
    //dont do work if there's nothing to do
    if (doc.IsObject() == false){ return; }

    int new_level = this->get_int(doc, "building_level", -1);
    if (new_level != -1)
    {
        //only used saved building level
        this->building->building_level = new_level;
    }
}

void BuildingSerializer::serialize_ingredients(rjDocument& doc)
{
    for (auto mist : this->building->ingredients)
    {
        IngredientSubType sub_type = mist.first;
        res_count_t count = mist.second;

        this->set_double(doc, Ingredient::type_to_string(sub_type), count);
    }
}

void BuildingSerializer::load_ingredients(rjDocument& doc)
{
    //dont do work if there's nothing to do
    if (doc.IsObject() == false){ return; }

    for (std::pair<Ingredient::SubType, std::string> pair : Ingredient::type_map)
    {
        //NOTE this isn't res_count_t aka long double, so we are going to lose data eventually!!!!
        double ing_count = this->get_double(doc, pair.second, -1);
        if (ing_count != -1)
        {
            this->building->ingredients[pair.first] = ing_count;
        }
    }
}

void BuildingSerializer::serialize_workers(rjDocument& doc)
{
    auto save_worker = [&](std::string prefix, mistHarvester& workers, std::map<std::pair<Worker::SubType, Ingredient::SubType>, res_count_t>::value_type mist) {
        std::stringstream ss;
        WorkerSubType worker_type = mist.first.first;
        IngredientSubType ing_type = mist.first.second;
        res_count_t count = mist.second;

        ss << prefix << "_" << Ingredient::type_to_string(ing_type) << "_" << static_cast<int>(worker_type);
        this->set_double(doc, ss.str(), (double)count); //TODO fix res_count_t to double data loss here
    };

    for (auto mist : this->building->harvesters)
    {
        save_worker("harvester", this->building->harvesters, mist);
    }

    for (auto mist : this->building->salesmen)
    {
        save_worker("salesmen", this->building->salesmen, mist);
    }

    for (auto mist : this->building->consumers)
    {
        save_worker("consumer", this->building->consumers, mist);
    }
}

void BuildingSerializer::load_workers(rjDocument& doc)
{
    //dont do work if there's nothing to do
    if (doc.IsObject() == false){ return; }

    auto load_worker = [&](std::string prefix, mistHarvester& workers, std::string& type_str, int& i, IngredientSubType& ing_type) {
        std::stringstream ss;
        ss << prefix << "_" << type_str << "_" << i;
        double harv_count;

        //hardcode a default 1 salesmen for Farm on new game
        if (ss.str() == "salesmen_grain_1" && this->building->name == "The Farm")
        {
            harv_count = this->get_double(doc, ss.str(), 1);
        }
        //load workers defaulting to -1
        else 
        {
            harv_count = this->get_double(doc, ss.str(), -1);
        }

        if (harv_count != -1)
        {
            std::pair<WorkerSubType, Ingredient::SubType> type_pair = { static_cast<WorkerSubType>(i), ing_type };
            workers[type_pair] = harv_count;
        };
    };

    //NOTE this goes through each ingredient, then for each ing, makes up to 10
    //queries on the doc per worker type. this could get slow, but C++ is great so who knows
    for (std::pair<Ingredient::SubType, std::string> pair : Ingredient::type_map)
    {
        Ingredient::SubType ing_type = pair.first;
        std::string type_str = pair.second;

        for (int i = 1; i <= 20; i++){
            load_worker("harvester", this->building->harvesters, type_str, i, ing_type);
            load_worker("salesmen", this->building->salesmen, type_str, i, ing_type);
            load_worker("consumer", this->building->consumers, type_str, i, ing_type);
        };
    }
}

void BuildingSerializer::serialize_tech(rjDocument& doc)
{
    auto save_tech = [&](std::string prefix, std::map<TechSubType, res_count_t>::value_type mist) {
        std::stringstream ss;
        TechSubType worker_type = mist.first;
        res_count_t count = mist.second;

        ss << prefix << "_" << Technology::type_to_string(worker_type);
        this->set_double(doc, ss.str(), (double)count); //TODO fix res_count_t to double data loss here
    };

    for (auto mist : this->building->techtree->get_tech_map())
    {
        save_tech("tech", mist);
    }

}

void BuildingSerializer::load_tech(rjDocument& doc)
{
    //dont do work if there's nothing to do
    if (doc.IsObject() == false){ return; }

    auto load_tech = [&](std::string prefix, TechMap& tech_map, std::string& type_str, TechSubType& ing_type) {
        std::stringstream ss;
        ss << prefix << "_" << type_str;
        double harv_count = this->get_double(doc, ss.str(), -1);
        if (harv_count != -1)
        {
            tech_map[ing_type] = harv_count;
        };
    };

    for (std::pair<Technology::SubType, std::string> pair : Technology::type_map)
    {
        Technology::SubType tech_type = pair.first;
        std::string type_str = pair.second;

        load_tech("tech", this->building->techtree->tech_map, type_str, tech_type);
    }
}

void BuildingSerializer::_add_member(rjDocument& doc, rjValue& key, rjValue& value, rjDocument::AllocatorType& allocator)
{
    if (doc.HasMember(this->building->name.c_str()) == false)
    {
        doc.AddMember(
            rjValue().SetString(rj::StringRef(building->name.c_str())),
            rjValue(rj::kObjectType).Move(),
            allocator
        );
    }
    doc[building->name.c_str()].AddMember(key.Move(), value.Move(), allocator);
};

rjValue& BuildingSerializer::_get_member(rjDocument& doc, rjValue& key, rjDocument::AllocatorType& allocator)
{
    if (doc.HasMember(this->building->name.c_str()) == false ||
        doc[this->building->name.c_str()].HasMember(key) == false)
    {
        return doc;
    }
    return doc[building->name.c_str()][key];
};

ItemSerializer::ItemSerializer(std::string filename)
    : BaseSerializer(filename)
{
}

void ItemSerializer::serialize()
{

    //NOTE this nukes the existing items array
    auto doc = this->get_document();
    auto allocator = doc.GetAllocator();
    doc.SetArray();
    auto build_str_member = [&allocator](rjValue& row, std::string key, std::string value){

        rjValue rj_key = rjValue();
        auto key_str = key.c_str();
        auto key_len = key.length();
        rj_key.SetString(key_str, key_len, allocator);

        rjValue rj_value = rjValue();
        auto value_str = value.c_str();
        auto value_len = value.length();
        rj_value.SetString(value_str, value_len, allocator);


        row.AddMember(rj_key.Move(), rj_value.Move(), allocator);
    };
    auto build_dbl_member = [&allocator](rjValue& row, std::string key, res_count_t value){

        rjValue rj_key = rjValue();
        auto key_str = key.c_str();
        auto key_len = key.length();
        rj_key.SetString(key_str, key_len, allocator);

        rjValue rj_value = rjValue();
        rj_value.SetDouble(value);


        row.AddMember(rj_key.Move(), rj_value.Move(), allocator);
    };

    for (spItem item : BUILDUP->items)
    {
        rjValue row = rjValue();
        row.SetObject();
        
        build_str_member(row, "type_name", item->type_name);
        build_str_member(row, "rarity", ITEM_RARITY_STRINGS.at(item->rarity));
        build_dbl_member(row, "level", item->level);

        doc.PushBack(row, allocator);
    }

    this->save_document(doc);
}

void ItemSerializer::load()
{
    BUILDUP->items = {};
    auto doc = this->get_document();
    if (doc.IsArray())
    {
        for (auto it = doc.Begin(); it != doc.End(); it++)
        {
            rjValue& data = *it;

            std::string type_name = data["type_name"].GetString();
            std::string rarity_str = data["rarity"].GetString();
            res_count_t item_level = data["level"].GetDouble();

            auto result_it = std::find_if(ITEM_RARITY_STRINGS.begin(), ITEM_RARITY_STRINGS.end(), [rarity_str](std::pair<RarityType, std::string> pair){return pair.second == rarity_str; });
            RarityType rarity = RarityType::Unset;
            if (result_it != ITEM_RARITY_STRINGS.end())
            {
                rarity = (*result_it).first;
            }

            spItem item = ItemData().get_item(type_name);
            item->rarity = rarity;
            item->level = item_level;
            BUILDUP->items.push_back(item);
        }
        CCLOG("found an array of items for Items, as expected");
    }
    else
    {
        CCLOG("item doc is not an array, so we're assuming it doesnt exist");
        //assert(false && "item json is not an array");
    }
}

void ItemSerializer::serialize_item(spItem item)
{
}

void ItemSerializer::load_item(rjValue& item_value)
{
}

void ItemSerializer::_add_member(rjDocument& doc, rjValue& key, rjValue& value, rjDocument::AllocatorType& allocator)
{
}

rjValue& ItemSerializer::_get_member(rjDocument& doc, rjValue& key, rjDocument::AllocatorType& allocator)
{
    CCLOG("WARNING THIS SHOULD NOT BE CALLED");
    auto temp = new rjValue();
    return *temp;
}
