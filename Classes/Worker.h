#pragma once
#ifndef WORKER_H
#define WORKER_H

#include "constants.h"

#include "Updateable.h"
#include "Nameable.h"

#include "SubTypes.h"

//
//attaches to a building and does something once certain conditions are hit
//

class Worker : public Nameable, Updateable
{
    public:
        using SubType = WorkerSubType;
        SubType sub_type;

        spBuilding building;

        //amount of workers of this type, ie if this unlocks 1 grain per update,
        //and there's 10 active, it'll get you 10 grain
        res_count_t active_count = 0;

        Worker(spBuilding building, std::string name, SubType sub_type);

        void update(float dt);
        virtual void on_update(float dt);
};

class Harvester : public Worker
{
    public:
        IngredientSubType ing_type;

        Harvester(spBuilding building, std::string name, IngredientSubType ing_type, SubType sub_type);

        virtual void on_update(float dt) override;

        static res_count_t get_base_shop_cost(SubType harv_type);
        static res_count_t get_to_harvest_count(SubType harv_type, IngredientSubType ing_type);
};

class Salesman : public Harvester
{
    public:
        Salesman(spBuilding building, std::string name, IngredientSubType ing_type, SubType sub_type);

        virtual void on_update(float dt) override;
        static res_count_t get_base_shop_cost(SubType sub_type);
        static res_count_t get_to_sell_count(SubType sub_type);
};

class ConsumerHarvester : public Harvester
{
    public:
        ConsumerHarvester(spBuilding building, std::string name, IngredientSubType ing_type, SubType sub_type);
        virtual void on_update(float dt) override;

};

class ScavengerHarvester : public Harvester
{
    public:
        ScavengerHarvester(spBuilding building, std::string name, IngredientSubType ing_type, SubType sub_type);
        virtual void on_update(float dt) override;

};


#endif
