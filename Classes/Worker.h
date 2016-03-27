#pragma once
#ifndef WORKER_H
#define WORKER_H

#include "HouseBuilding.h"

class ResourceCondition : public Nameable
{
    public:
        Resource::ResourceType type_choice;

        Ingredient::IngredientType ing_type;
        Product::ProductType pro_type;
        Waste::WasteType wst_type;

        int quantity;

        ResourceCondition(Resource::ResourceType type_choice, int quantity, std::string name) : Nameable(name) {
            this->type_choice = type_choice;
            this->quantity = quantity;
        };

        static ResourceCondition* create_ingredient_condition(Ingredient::IngredientType ing_type, int quantity, std::string condition_name);
        static ResourceCondition* create_product_condition(Product::ProductType pro_type, int quantity, std::string condition_name);
        static ResourceCondition* create_waste_condition(Waste::WasteType wst_type, int quantity, std::string condition_name);

        bool is_satisfied(spBuilding building) {
            if (this->type_choice == Resource::Ingredient) {
                return (int)building->ingredients.size() >= this->quantity;
            }
            else if (this->type_choice == Resource::Product) {
                return (int)building->products.size() >= this->quantity;
            }
            else if (this->type_choice == Resource::Ingredient) {
                return (int)building->wastes.size() >= this->quantity;
            }
            else {
                unsigned int size = building->ingredients.size() +
                    building->products.size() +
                    building->wastes.size();
                return (int)size >= this->quantity;
            };
        };
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

        //update calls on_update after threshold has been passed
        void update(float dt);
        virtual void on_update();
};

class Harvester : public Worker
{
    public:
        Harvester(spBuilding building, std::string name);

        virtual void on_update();
};

#endif
