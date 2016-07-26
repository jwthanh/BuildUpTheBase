#include "Animal.h"
#include <iostream>

#include "HouseBuilding.h"
#include "Util.h"


template<typename mistMap>
void transfer(mistMap& origin_mi, mistMap& destination_mi,
    res_count_t quantity,
    typename mistMap::key_type sub_type
    )
{
    res_count_t _def = 0;
    auto st = sub_type;
    res_count_t count = map_get(origin_mi, st, _def);

    if (count < quantity )
    {
        std::stringstream ss;
        ss << quantity << " is too many.";
        CCLOG("%s", ss.str().c_str());
        ss.str("");

        quantity = count;
        ss << " new size is " << quantity << std::endl;
        CCLOG("%s", ss.str().c_str());
    }

    if (quantity > 0)
    {
        while (quantity > 0)
        {
            //increment the destination and decrement the origin
            --origin_mi[sub_type];
            ++destination_mi[sub_type];

            quantity--;
        }
    }
};

void Animal::transfer_ingredients(spBuilding from_bldg, spBuilding to_bldg, Ingredient::SubType sub_type, int quantity)
{
    transfer(from_bldg->ingredients, to_bldg->ingredients, quantity , sub_type);
};

void Animal::transfer_products(spBuilding from_bldg, spBuilding to_bldg, Product::SubType sub_type, int quantity)
{
    transfer(from_bldg->products, to_bldg->products, quantity , sub_type);
};

void Animal::transfer_wastes(spBuilding from_bldg, spBuilding to_bldg, Waste::SubType sub_type, int quantity)
{
    transfer(from_bldg->wastes, to_bldg->wastes, quantity , sub_type);
};

void Animal::b2b_transfer(spBuilding from_bldg, spBuilding to_bldg, Resource::ResourceType res_type, int quantity)
{
    if (res_type == Resource::Ingredient)
    {
        transfer(from_bldg->ingredients, to_bldg->ingredients, quantity , Ingredient::SubType::Grain);
    }
    else if (res_type == Resource::Product)
    {
        transfer(from_bldg->products, to_bldg->products, quantity, Product::SubType::Cereals);
    }
    else if (res_type == Resource::Waste)
    {
        transfer(from_bldg->wastes, to_bldg->wastes, quantity, Waste::SubType::Corpse);
    }
    else
    {
        std::cout << "type not recognized" << std::endl;
    }

};
