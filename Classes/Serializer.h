#pragma once
#ifndef SERIALIZER_H
#define SERIALIZER_H

#include <json/document.h>

#include "constants.h"

#include "SubTypes.h"


class Miner;

namespace cocos2d
{
    class TMXTiledMap;
    class TMXLayer;
};

using tile_gid_t = uint32_t; //copied from Miner.h


namespace rj = rapidjson;

///base for serializing classes, needs to handle at least writing to a file and
//reading it. Ideally it'd have backups and validators, but for now it'll only
//need to handle opening and closing files. Caching will be secondary
class BaseSerializer
{
    private:
        //subclasses change the way it accesses the root doc, so buildings use doc[building_name] to add to instead.
        virtual void _add_member(rjDocument& doc, rjValue& key, rjValue& value, rjDocument::AllocatorType& allocator);
        virtual rjValue& _get_member(rjDocument& doc, rjValue& key, rjDocument::AllocatorType& allocator);

    public:
        BaseSerializer(std::string filename);
        std::string filename;

        rjDocument get_document(std::string filename = "") const;
        void save_document(rjDocument& doc, std::string filename = "") const;

        void add_member(rjDocument& doc, std::string key, rjValue& value);
        rjValue& get_member(rjDocument& doc, std::string key);

        std::string get_string(rjDocument & doc, std::string key, std::string fallback="");
        double get_double(rjDocument & doc, std::string key, double fallback=0.0);
        int get_int(rjDocument & doc, std::string key, int fallback=0);

        void set_string(rjDocument & doc, std::string key, std::string value);
        void set_double(rjDocument & doc, std::string key, double value);
        void set_int(rjDocument & doc, std::string key, int value);

        //override this to handle serializng the object
        virtual void serialize(){};
        virtual void load(){};


};

class BuildingSerializer : public BaseSerializer
{
    public:
        spBuilding building;
        BuildingSerializer(std::string filename, spBuilding building);

        void serialize() override;
        void load() override;

        void serialize_building_level(rjDocument& doc);
        void load_building_level(rjDocument& doc);

        void serialize_ingredients(rjDocument& doc);
        void load_ingredients(rjDocument& doc);

        void serialize_workers(rjDocument& doc);
        void load_workers(rjDocument& doc);

        void serialize_tech(rjDocument& doc);
        void load_tech(rjDocument& doc);

    private:
        virtual void _add_member(rjDocument& doc, rjValue& key, rjValue& value, rjDocument::AllocatorType& allocator) override;
        virtual rjValue& _get_member(rjDocument& doc, rjValue& key, rjDocument::AllocatorType& allocator);
};

/*
 * since Items are unique, like you can't have 1000+ copies 
 * of an Item, we serialize each one and then write it to file
 * in a list, instead of pulling it out by key like the Buildings
 */
class ItemSerializer : public BaseSerializer
{
    public:
        ItemSerializer(std::string filename);

        void serialize() override;
        void load() override;

        void serialize_item(spItem item);
        void load_item(rjValue& item_value);

    private:
        virtual void _add_member(rjDocument& doc, rjValue& key, rjValue& value, rjDocument::AllocatorType& allocator) override;
        virtual rjValue& _get_member(rjDocument& doc, rjValue& key, rjDocument::AllocatorType& allocator) override;
};

class IngredientSerializer : public BaseSerializer
{
    public:
        IngredientSerializer(std::string filename);

        void serialize() override;
        void serialize_ing_type(rjDocument& doc, rapidjson::CrtAllocator& allocator, IngredientSubType ing_type, res_count_t count);
        void load() override;

    private:
        virtual void _add_member(rjDocument& doc, rjValue& key, rjValue& value, rjDocument::AllocatorType& allocator) override;
        virtual rjValue& _get_member(rjDocument& doc, rjValue& key, rjDocument::AllocatorType& allocator);
};

class MinerSerializer : public BaseSerializer
{
    public:
        MinerSerializer(std::string filename, Miner* miner);
        Miner* miner;

        bool existing_json_found;

        void serialize() override;
        std::vector<tile_gid_t> serialize_layer(cocos2d::TMXLayer* layer);
        void load() override;

    private:
        virtual void _add_member(rjDocument& doc, rjValue& key, rjValue& value, rjDocument::AllocatorType& allocator) override;
        virtual rjValue& _get_member(rjDocument& doc, rjValue& key, rjDocument::AllocatorType& allocator);
};

class EquipmentSerializer : public BaseSerializer
{
    public:
        EquipmentSerializer(std::string filename);

        void serialize() override;
        void load() override;

    private:
        virtual void _add_member(rjDocument& doc, rjValue& key, rjValue& value, rjDocument::AllocatorType& allocator) override;
        virtual rjValue& _get_member(rjDocument& doc, rjValue& key, rjDocument::AllocatorType& allocator);
};

#endif
