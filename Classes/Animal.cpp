#include "Animal.h"
#include <iostream>

#include "HouseBuilding.h"

//void transfer3(vsIngredient& origin_vs, vsIngredient& destination, unsigned int quantity,
//    //vsIngredient::value_type::element_type::SubType)
//    std::remove_pointer<decltype(origin_vs::at(0))>::type::SubType sub_type)
//{
//    
//}

template<typename ResourceVector>
void transfer(ResourceVector& origin_vs, ResourceVector& destination,
    unsigned int quantity,
    typename ResourceVector::value_type::element_type::SubType sub_type
    //typename std::remove_pointer<decltype(((typename ResourceVector::value_type*)nullptr)->get())>::type::SubType sub_type
    )
{
    typedef std::remove_pointer<decltype(((typename ResourceVector::value_type*)nullptr)->get())> ResourcePOINTER;
    CCLOG("HERE %s", typeid(ResourcePOINTER).name()); //equivalent to Ingredient*
    CCLOG("HERER %s", typeid(ResourcePOINTER::type).name()); //equivalent to Ingredient
    
    typedef decltype(origin_vs.at(0)) ResourcePointer;
    typedef std::remove_pointer<ResourcePointer> ResourceClass;

    if (origin_vs.size() < quantity)
    {
        std::cout << quantity << " is too many.";
        quantity = origin_vs.size();
        std::cout << " new size is" << quantity << std::endl;
    }

    if (quantity > 0)
    {
        auto it = std::next(origin_vs.begin(), quantity);
        std::move(origin_vs.begin(), it, std::back_inserter(destination));
        origin_vs.erase(origin_vs.begin(), it);
    }
};
void transfer2(std::vector<spIngredient>& origin_vs, std::vector<spIngredient>& destination, unsigned int quantity)
{
    typedef std::vector<spIngredient>::value_type qwe;
    typedef decltype(origin_vs.at(110000).get()) IngredientPointer;
    //if (Ingredient::Grain == qwe::SubType::Grain)
    //{
    //    CCLOG("it matches");
    //}

    typedef std::remove_pointer<IngredientPointer> clean;
    auto temp = new Ingredient("grain");
    CCLOG("%s", typeid(qwe).name());
    CCLOG("%s", typeid(IngredientPointer).name());
    CCLOG("%s", typeid(Ingredient).name());
    CCLOG("%s", typeid(temp).name());
    CCLOG("%s", typeid(std::remove_pointer<IngredientPointer>::type).name());

}


void Animal::b2b_transfer(spBuilding from_bldg, spBuilding to_bldg, Resource::ResourceType res_type, int quantity)
{
    //printj1("moving x" << quantity << " " << Resource::type_str(res_type) << " from " << from_bldg->name << " to " << to_bldg->name);
    if (res_type == Resource::Ingredient)
    {
        transfer(from_bldg->ingredients, to_bldg->ingredients, quantity , Ingredient::Grain);
    }
    else if (res_type == Resource::Product)
    {
        transfer(from_bldg->products, to_bldg->products, quantity, Product::Cereals);
    }
    else if (res_type == Resource::Waste)
    {
        transfer(from_bldg->wastes, to_bldg->wastes, quantity, Waste::Corpse);
    }
    else
    {
        std::cout << "type not recognized" << std::endl;
    }

};
