#pragma once
#ifndef STATICDATA_H
#define STATICDATA_H

#include <string>

class BaseData
{
    private:
    protected:
        std::string _filename;
        std::string _get_data(std::string key_top, std::string key_child="", std::string key_grandchild="");
    public:

        //BaseData(){};

        virtual std::string getter(std::string key) = 0;
};

class BuildingData : public BaseData
{
    public:
        std::string building_name;

        BuildingData(std::string building_name);

        std::string getter(std::string key) override;

        std::string get_task_name();
        std::string get_description();
        std::string get_gold_cost();
        std::string get_img_large();
};

#endif
