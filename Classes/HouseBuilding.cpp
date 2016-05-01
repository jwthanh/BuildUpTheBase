// ConsoleApplication1.cpp : Defines the entry point for the console application.
//

#include <memory>
#include <string>
#include <vector>
#include <iostream>
#include <ctime>
#include <assert.h>
#include <sstream>
#include <algorithm>
#include <locale>

#include <tinyxml2/tinyxml2.h>
#include <json/document.h>

#include "cocos2d.h"

#include "HouseBuilding.h"

#include "Recipe.h"
#include "Worker.h"
#include "RandomWeightMap.h"

#include "Beatup.h"
#include "Util.h"
#include "FShake.h"

#include "FileOperation.h"
#include "GameLogic.h"
#include "StaticData.h"

#include "Animal.h"
#include "Fighter.h"

#include "ProgressBar.h"
#include "attribute_container.h"
#include "attribute.h"

USING_NS_CC;


Buildup::Buildup()
{
    this->city = NULL;
    this->set_target_building(NULL);

    //TODO change this to player_avatar or something. I don't want to wait for a recompile atm
    this->fighter = std::make_shared<Fighter>("Jimothy2");
    this->fighter->attrs->health->set_vals(100);

    this->player = NULL;
};




void Village::update(float dt)
{
    Updateable::update(dt);

    this->update_buildings(dt);
};


void Village::update_buildings(float dt)
{
    // printj("\n\nupdating buildings");
    for (std::shared_ptr<Building> building : this->buildings)
    {
        building->update(dt);
    };

};

spBuilding Village::building_by_name(std::string name)
{
    for (auto bldg : this->buildings)
    {
        if (bldg->name == name)
            return bldg;
    };

    return NULL;
};

template<typename from_V>
void remove_if_sized(from_V& from_vs, unsigned int condition_size, unsigned int remove_count, VoidFunc callback )
{
    if (from_vs.size() > condition_size)
    {

        if (remove_count == 0) { remove_count = condition_size; };
        printj1("removing " << remove_count);
        from_vs.erase(from_vs.begin(), from_vs.begin()+remove_count);
        callback();
    };
};

void move_if_sized(Resource::ResourceType res_type,
        unsigned int condition_size, unsigned int move_count,
        spBuilding from_bldg, spBuilding to_bldg, VoidFunc callback )
{
    unsigned int from_size;
    if (res_type == Resource::Ingredient) from_size = from_bldg->ingredients.size();
    else if (res_type == Resource::Product) from_size = from_bldg->products.size();
    else if (res_type == Resource::Waste) from_size = from_bldg->wastes.size();

    if (from_size >= condition_size)
    {
        if (move_count == 0) { move_count = condition_size; };
        Animal animal = Animal("Horse");
        animal.b2b_transfer(
            from_bldg,
            to_bldg,
            res_type,
            move_count
            );
        callback();
    };
};


template<typename T>
std::shared_ptr<T> create_one(typename T::SubType sub_type)
{
    return std::make_shared<T>(sub_type);
};


template<typename mistT>
void create(mistT& mist, int quantity, typename mistT::key_type sub_type)
{
    if (mist.count(sub_type) == 0)
    {
        //this resets 0 to 0 if it already exists instead of only adding a default
        mist[sub_type] = 0;
    }

    mist[sub_type] += quantity;

};

Building::Building(Village* city, std::string name, std::string id_key) :
             Nameable(name), Buyable(id_key), Updateable(), city(city)
{
    num_workers = 1;

    update_clock->set_threshold(1.0f);
    spawn_clock = new Clock(3);

    ingredients = mistIngredient();
    products = mistProduct();
    wastes = mistWaste();

    fighters = vsFighter();
    workers = vsWorker();

    harvesters = mistHarvester();

    menu_items = {};

    std::stringstream ss;

    //auto file_utils = FileUtils::getInstance();
    // std::string xmlBuffer = file_utils->getStringFromFile("joshtest.dat");
    //tinyxml2::XMLDocument* xmlDoc = new tinyxml2::XMLDocument();
    //xmlDoc->Parse(xmlBuffer.c_str(), xmlBuffer.size());
    //auto root = xmlDoc->RootElement();

    auto jsonDoc = FileIO::open_json("buildings_data.json");
    assert(jsonDoc.HasMember("buildings"));
    if (jsonDoc["buildings"].HasMember(name.c_str()))
    {
        auto all_buildings = &jsonDoc["buildings"];
        auto this_building = &(*all_buildings)[name.c_str()];

        if (this_building->HasMember("task_name")) {
            auto task_name = &(*this_building)["task_name"];
            ss << task_name->GetString() << " task";
        } else {
            ss << name << " task";
        };
    }
    else {
        ss << name << " task";
    };

     menu_items = {
         {},
     };

     this->data = std::make_shared<BuildingData>(name);
    this->_shop_cost = atoi(this->data->get_gold_cost().c_str());

    //TODO make these buyable again
    this->set_been_bought(true);

};

res_count_t Building::get_total_harvester_output()
{
    res_count_t total = 0;

    for (auto h_mist : this->harvesters)
    {
        Harvester::SubType harv_type = h_mist.first.first;
        Ingredient::SubType ing_type = h_mist.first.second;
        res_count_t active_count = h_mist.second;
        total += Harvester::get_harvested_count(harv_type)*active_count;
    };

    return total;
};

res_count_t Building::count_ingredients(Ingredient::SubType ing_type)
{
    return this->ingredients[ing_type];
};

res_count_t Building::count_products(Product::SubType pro_type)
{
    return this->products[pro_type];
};

res_count_t Building::count_wastes(Waste::SubType wst_type)
{
    return this->wastes[wst_type];
};

void Building::create_ingredients(Ingredient::SubType sub_type, int quantity)
{
    create<mistIngredient>(this->ingredients, quantity, sub_type);
};

void Building::create_products(Product::SubType sub_type, int quantity)
{
    create<mistProduct>(this->products, quantity, sub_type);
};

void Building::create_wastes(Waste::SubType sub_type, int quantity)
{
    create<mistWaste>(this->wastes, quantity, sub_type);
};


void Building::consume_recipe(Recipe* recipe)
{
    if (recipe->is_satisfied(this->ingredients))
    {
        recipe->consume(this->ingredients);
    }
};

void Building::update(float dt)
{
    Updateable::update(dt);


    if (update_clock->passed_threshold())
    {
        /* this->spawn_clock->update(dt); */
        // this->do_task(dt);
        // CCLOG("building update %s", this->name.c_str());

        update_clock->reset();

        for (auto harvester : this->harvesters) {
            std::pair<Harvester::SubType, Ingredient::SubType> sub_type = harvester.first;
            auto harv_type = sub_type.first;
            auto ing_type = sub_type.second;
            res_count_t count = harvester.second;

            auto temp_harvester = std::make_shared<Harvester>(this->shared_from_this(), "test worker", ing_type, harv_type);
            temp_harvester->active_count = count;
            temp_harvester->update(dt);
        };
    }
    // else
    // {
    // }
};

std::string Building::get_specifics()
{
    return this->get_ingredients() + " " +
    this->get_products() + " " +
    this->get_wastes();
    // this->get_fighters();

};

void Building::print_specifics()
{
    this->print_ingredients();
    this->print_products();
    this->print_wastes();
    this->print_fighters();
};

#define PRINT_RESOURCE(Rtype, Rlowertype) \
res_count_t Building::count_##Rlowertype##s() \
{ \
    res_count_t total = 0; \
 \
    for (auto type_str : Rtype::type_map) \
            { \
        Rtype::SubType type = type_str.first; \
 \
        res_count_t count = map_get(this->Rlowertype##s, type, 0);\
 \
        total += count; \
    }; \
    return total;\
 \
};\
std::string Building::get_##Rlowertype##s() \
{ \
    std::stringstream ss; \
 \
    for (auto type_str : Rtype::type_map) \
        { \
        Rtype::SubType type = type_str.first; \
        std::string str = type_str.second; \
 \
        unsigned int count = this->Rlowertype##s[type];\
 \
        if (count != 0) \
        { \
            ss << str << "(x" << count << ") "; \
        } \
    }; \
    return ss.str();\
 \
};\
\
void Building::print_##Rlowertype##s() \
{ \
    std::string ss = this->get_##Rlowertype##s(); \
    if (!ss.empty()) \
    { \
        printj("   " << #Rtype << ": " << ss); \
    }\
};

PRINT_RESOURCE(Ingredient, ingredient);
PRINT_RESOURCE(Product, product);
PRINT_RESOURCE(Waste, waste);

void Building::print_fighters()
{
    std::stringstream ss;
    for (spFighter fighter : this->fighters) 
    {
        ss << fighter->get_stats() << " ";
    };

    if (!ss.str().empty()) 
    {
        std::cout << "   " << ss.str() << std::endl;
    };
};


std::string Building::get_inventory()
{
    std::stringstream ss;
    ss << "ING: " << this->ingredients.size();
    ss << " PDT: " << this->products.size();
    ss << " WST: " << this->wastes.size();
    ss << " FIT: " << this->fighters.size();
    // ss << std::endl;

    return ss.str();
};

void Building::print_inventory()
{
    std::string inventory = this->get_inventory();
    printj("   " << inventory);
    this->print_specifics();

};

void test_conditions()
{
    mistIngredient inputs = {
        { Ingredient::Grain , 2 },
        { Ingredient::Iron, 1 }
    };

    auto city = new Village(NULL, "The Test City");
    auto farm = std::make_shared<Building>(city, "The Test Farm", "test_farm");
    farm->ingredients = inputs;
    
    ResourceCondition* rc = ResourceCondition::create_ingredient_condition(Ingredient::Grain, 2, "test condition");
    // IngredientCondition rc = IngredientCondition(
    //         IngredientCondition::TypeChoices::Ingredient,
    //         Ingredient::Grain,
    //         2,
    //         "test condition");

    assert(rc->is_satisfied(farm) == true);

    farm->ingredients = mistIngredient({
        { Ingredient::Iron, 1 }
    });
    assert(rc->is_satisfied(farm) == false);

    rc->ing_type = Ingredient::Iron;
    assert(rc->is_satisfied(farm) == false);
    rc->quantity = 1;
    assert(rc->is_satisfied(farm) == true);

    delete rc;

};

///TODO fix this to work with mist
//void test_recipe()
//{
//    mistIngredient inputs = {
//        { Ingredient::Grain, 2 },
//        { Ingredient::Iron, 1 }
//    };
//
//    Recipe recipe = Recipe("test recipe");
//    recipe.components = ComponentMap();
//    recipe.components[Ingredient::SubType::Grain] = 2;
//    recipe.components[Ingredient::SubType::Iron] = 1;
//    bool result = recipe.is_satisfied(inputs);
//    assert(result && "everythings there");
//    std::cout << "is the recipe satisfied? " << std::boolalpha << result << std::endl << std::endl;
//
//    recipe.components = ComponentMap();
//    recipe.components[Ingredient::SubType::Grain] = 3;
//    result = recipe.is_satisfied(inputs);
//    assert(!result && " missing one type but has the other");
//
//    recipe.components = ComponentMap();
//    recipe.components[Ingredient::SubType::Grain] = 1;
//    result = recipe.is_satisfied(inputs);
//    assert(result && "over shoots reqs");
//
//    recipe.components = ComponentMap();
//    recipe.components[Ingredient::SubType::Fly] = 5;
//    result = recipe.is_satisfied(inputs);
//    assert(!result && "looks for ingredient not in input");
//
//}

void Buildup::update(float dt)
{
    this->city->update(dt);
    this->player->update(dt);
};

//this isn't used anymore
void Buildup::main_loop()
{
    Clock game_clock = Clock(CLOCKS_PER_SEC);
    clock_t start_time = clock() / CLOCKS_PER_SEC;

    printj("starting tests...");
    //test_recipe(); //TODO fix with mist
    test_conditions();
    printj("...done tests");
    int total_loops = 0;

    int current_ticks = 0;
    while (true)
    {
        total_loops++;
        game_clock.update((float)current_ticks);

        //current_ticks = clock() / CLOCKS_PER_SEC - start_time;
        current_ticks += 1000;
        if (game_clock.passed_threshold())
        {
            this->city->update(game_clock.start_time);
            this->player->update(game_clock.start_time);

            game_clock.reset();

            current_ticks = 0;
            start_time = clock() / CLOCKS_PER_SEC;

            std::string temp;
            std::cout << "enter to continue " << std::endl;
            std::getline(std::cin, temp);
        }
    }


}

Village* Buildup::init_city(Buildup* buildup)
{
    auto city = new Village(buildup, "Burlington");

    auto farm = std::make_shared<Building>(city, "The Farm", "the_farm");
    farm->workers.push_back(std::make_shared<Worker>(farm, "Farmer"));
    farm->punched_sub_type = "grain";
    buildup->set_target_building(farm);

    auto dump = std::make_shared<Building>(city, "The Dump", "the_dump");
    dump->punched_sub_type = "fly";

    auto workshop = std::make_shared<Building>(city, "The Workshop", "the_workshop");
    workshop->punched_sub_type = "wood";

    auto marketplace = std::make_shared<Building>(city, "The Marketplace", "the_marketplace");

    auto arena = std::make_shared<Building>(city, "The Arena", "the_arena");
    arena->punched_sub_type = "sand";

    auto mine = std::make_shared<Building>(city, "The Mine", "the_mine");
    mine->punched_sub_type = "copper";

    auto grave = std::make_shared<Building>(city, "The Graveyard", "the_graveyard");
    grave->punched_sub_type = "flesh";

    auto necro = std::make_shared<Building>(city, "The Underscape", "the_underscape");
    necro->punched_sub_type = "blood";

    auto forest = std::make_shared<Building>(city, "The Forest", "the_forest");
    forest->punched_sub_type = "berry";

    buildup->fighter = std::make_shared<Fighter>("Fighter");
    buildup->fighter->sprite_name = "townsmen8x8.png";
    buildup->fighter->team = Fighter::TeamOne;
    buildup->fighter->attrs->health->set_vals(100);

    buildup->brawler = std::make_shared<Fighter>("Brawler");
    buildup->brawler->team = Fighter::TeamOne;
    buildup->brawler->sprite_name = "ogre10x10.png";
    buildup->brawler->attrs->health->set_vals(200);


    arena->fighters.push_back(buildup->fighter);
    arena->fighters.push_back(buildup->brawler);

    Animal animal("ASD");
    animal.b2b_transfer(farm, dump, Resource::Ingredient, 10);

    city->buildings.push_back(farm);
    city->buildings.push_back(dump);
    city->buildings.push_back(workshop);
    city->buildings.push_back(marketplace);
    city->buildings.push_back(arena);
    city->buildings.push_back(mine);
    city->buildings.push_back(grave);
    city->buildings.push_back(necro);
    city->buildings.push_back(forest);

    return city;
};

spBuilding Buildup::get_target_building()
{
    return this->_target_building;
};

void Buildup::set_target_building(spBuilding building)
{
    this->_target_building = building;
};

void Player::update(float dt)
{
    // printj("   The Player has "<< this->coins << "coins");
};

