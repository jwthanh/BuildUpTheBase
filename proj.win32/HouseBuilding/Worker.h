#pragma once
#ifndef WORKER_H
#define WORKER_H

#include "HouseBuilding.h"

template<typename Rtype>
class ResourceCondition : public Nameable
{
    public:
        Rtype choice;

        //Ingredient::IngredientType ingredient_choice;
        //Product::ProductType product_choice;
        //Waste::WasteType waste_choice;

        int quantity;

        ResourceCondition(/*TypeChoices type_choice,*/ Rtype choice, int quantity, std::string name) : Nameable(name) {
            //this->type_choice = type_choice;
            this->quantity = quantity;

            //this->ingredient_choice = NULL;
            //this->product_choice = NULL;
            //this->waste_choice = NULL;

            this->choice = choice;
            //if (type_choice == Ingredient) {
            //    this->ingredient_choice = reinterpret_cast<Ingredient::IngredientType>(choice);
            //} else if (type_choice == Product) {
            //    this->product_choice = choice; 
            //} else if (type_choice == Waste) {
            //    this->waste_choice = choice;
            //};
        };
};

//
//attaches to a building and builds recipes once certain conditions are hit
//
class Worker : public Nameable
{
    public:
        spBuilding building;

        Worker(spBuilding building, std::string name) : Nameable(name) {
            this->building = building;
        };
};

#endif
