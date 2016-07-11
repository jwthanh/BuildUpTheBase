#pragma once
#ifndef SERIALIZER_H
#define SERIALIZER_H

#include <json/document.h>

#include "constants.h"


namespace rj = rapidjson;

///base for serializing classes, needs to handle at least writing to a file and
//reading it. Ideally it'd have backups and validators, but for now it'll only
//need to handle opening and closing files. Caching will be secondary
class BaseSerializer
{
    public:
        BaseSerializer(std::string filename);
        std::string filename;

        rj::Document get_document(std::string filename = "") const;
        void save_document(rj::Document& doc, std::string filename = "") const;

        void add_member(rj::Document& doc, std::string key, rj::Value& value);

    private:
        //subclasses change the way it accesses the root doc, so buildings use doc[building_name] to add to instead.
        virtual void _add_member(rj::Document& doc, rj::Value& key, rj::Value& value, rapidjson::Document::AllocatorType& allocator);

    public:
        void set_string(rj::Document & doc, std::string key, std::string value);
        void set_double(rj::Document & doc, std::string key, double value);
        void set_int(rj::Document & doc, std::string key, int value);

        //override this to handle serializng the object
        virtual void serialize(){};


};

class BuildingSerializer : public BaseSerializer
{
    public:
        spBuilding building;
        BuildingSerializer(std::string filename, spBuilding building);

        void serialize() override;
        void serialize_building_level(rapidjson::Document& doc);
        void serialize_ingredients(rapidjson::Document& doc);
        void serialize_workers(rapidjson::Document& doc);

    private:
        virtual void _add_member(rj::Document& doc, rj::Value& key, rj::Value& value, rapidjson::Document::AllocatorType& allocator) override;
};

#endif
