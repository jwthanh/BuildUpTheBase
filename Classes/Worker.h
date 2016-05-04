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
class Worker : public Nameable, Updateable
{
    public:
        spBuilding building;

        //amount of workers of this type, ie if this unlocks 1 grain per update,
        //and there's 10 active, it'll get you 10 grain
        int active_count = 0; 

        Worker(spBuilding building, std::string name);

        void update(float dt);
        virtual void on_update(float dt);
};

class Harvester : public Worker
{
    public:
        Ingredient::SubType ing_type;

        enum class SubType {
            One, Two, Three, Four, Five, Six, Seven, Eigth, Nine, Ten,
            eleven, twelve, thirteen, fourteen, fifteen, sixteen, seventeen, eighteen, nineteen, twenty
        } sub_type;

        Harvester(spBuilding building, std::string name, Ingredient::SubType ing_type, SubType sub_type);

        virtual void on_update(float dt);

        static res_count_t get_base_shop_cost(Harvester::SubType harv_type);
        static res_count_t get_harvested_count(Harvester::SubType harv_type);
};

typedef std::map<std::pair<Harvester::SubType, Ingredient::SubType>, res_count_t> mistHarvester;

#endif
