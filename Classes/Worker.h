#pragma once
#ifndef WORKER_H
#define WORKER_H

#include "constants.h"

#include "Updateable.h"
#include "Nameable.h"


#include "Resources.h"

class ResourceCondition : public Nameable
{
    public:
        Resource::ResourceType type_choice;

        Ingredient::SubType ing_type;
        Product::SubType pro_type;
        Waste::SubType wst_type;

        int quantity;

        ResourceCondition(Resource::ResourceType type_choice, int quantity, std::string name) : Nameable(name) {
            this->type_choice = type_choice;
            this->quantity = quantity;
        };

        static ResourceCondition* create_ingredient_condition(Ingredient::SubType ing_type, int quantity, std::string condition_name);
        static ResourceCondition* create_product_condition(Product::SubType pro_type, int quantity, std::string condition_name);
        static ResourceCondition* create_waste_condition(Waste::SubType wst_type, int quantity, std::string condition_name);

        bool is_satisfied(spBuilding building);
};


//if this is satisfied, the action can happen
class Condition : public Nameable
{

};

//
class Action : public Nameable
{

};

//
//attaches to a building and does something once certain conditions are hit
//
enum class WorkerSubType
{
    ZERO = 0, //ZERO is unset
    One, Two, Three, Four, Five, Six, Seven, Eigth, Nine, Ten,
    Eleven, Twelve, Thirteen, Fourteen, Fifteen, Sixteen, Seventeen, Eighteen, Nineteen, Twenty
};

class Worker : public Nameable, Updateable
{
    public:
        WorkerSubType sub_type;

        spBuilding building;

        //amount of workers of this type, ie if this unlocks 1 grain per update,
        //and there's 10 active, it'll get you 10 grain
        int active_count = 0; 

        Worker(spBuilding building, std::string name, WorkerSubType sub_type);

        void update(float dt);
        virtual void on_update(float dt);
};

class Harvester : public Worker
{
    public:
        Ingredient::SubType ing_type;

        Harvester(spBuilding building, std::string name, Ingredient::SubType ing_type, WorkerSubType sub_type);

        virtual void on_update(float dt) override;

        static res_count_t get_base_shop_cost(WorkerSubType harv_type);
        static res_count_t get_harvested_count(WorkerSubType harv_type);
};

class Salesman : public Harvester
{
    public:
        virtual void on_update(float dt) override;
        static res_count_t get_base_shop_cost(WorkerSubType sub_type);
        static res_count_t get_sold_count(WorkerSubType sub_type);
};

typedef std::map<std::pair<WorkerSubType, Ingredient::SubType>, res_count_t> mistHarvester;
typedef std::map<WorkerSubType, res_count_t> mistWorkerSubType;

#endif
