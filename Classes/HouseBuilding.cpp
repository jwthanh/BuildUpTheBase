// ConsoleApplication1.cpp : Defines the entry point for the console application.
//

#include <memory>
#include <string>
#include <vector>
#include <iostream>
#include <ctime>
#include <assert.h>
#include <sstream>

#include <tinyxml2/tinyxml2.h>
#include <json/document.h>

#include "cocos2d.h"

#include "HouseBuilding.h"

#include "Recipe.h"
#include "Worker.h"
#include "RandomWeightMap.h"

#include "Util.h"

#include "FileOperation.h"
#include "StaticData.h"

#include "Animal.h"
#include "Fighter.h"

#include "attribute_container.h"
#include "attribute.h"
#include "Technology.h"

//this is all for the server stuff
#include "DataManager.h"
#include "GameLogic.h"
#include "Beatup.h"
#include "Network.h"
#include "Clock.h"
#include <json/stringbuffer.h>
#include <json/writer.h>

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

Building::Building(Village* city, std::string name, std::string id_key) :
             Nameable(name), Buyable(id_key), Updateable(), city(city)
{
    num_workers = 1;

    building_level = 1;

    update_clock->set_threshold(1.0f);
    spawn_clock = new Clock(3);

    ingredients = mistIngredient();
    products = mistProduct();
    wastes = mistWaste();

    fighters = vsFighter();
    workers = vsWorker();

    harvesters = mistHarvester();

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

    this->data = std::make_shared<BuildingData>(name);
    this->_shop_cost = atoi(this->data->get_gold_cost().c_str());

    //TODO make these buyable again
    this->set_been_bought(true);


    this->techtree = std::make_shared<TechTree>();

};

res_count_t Building::get_total_harvester_output()
{
    res_count_t total = 0;

    for (auto h_mist : this->harvesters)
    {
        WorkerSubType harv_type = h_mist.first.first;
        Ingredient::SubType ing_type = h_mist.first.second;
        res_count_t active_count = h_mist.second;
        total += Harvester::get_to_harvest_count(harv_type)*active_count;
    };

    return total;
};

res_count_t Building::get_total_salesmen_output()
{
    res_count_t total = 0;

    for (auto h_mist : this->salesmen)
    {
        WorkerSubType harv_type = h_mist.first.first;
        Ingredient::SubType ing_type = h_mist.first.second;
        res_count_t active_count = h_mist.second;
        total += Salesman::get_to_sell_count(harv_type)*active_count*this->building_level;
    };

    return total;
};

res_count_t Building::count_ingredients(Ingredient::SubType ing_type)
{
    return map_get(this->ingredients, ing_type, 0);
};

res_count_t Building::count_products(Product::SubType pro_type)
{
    return map_get(this->products, pro_type, 0);
};

res_count_t Building::count_wastes(Waste::SubType wst_type)
{
    return map_get(this->wastes, wst_type, 0);
};

template<typename T>
std::shared_ptr<T> create_one(typename T::SubType sub_type)
{
    return std::make_shared<T>(sub_type);
};

template<typename mistT>
void create(mistT& mist, res_count_t quantity, typename mistT::key_type sub_type)
{
    auto exisiting_count = map_get(mist, sub_type, 0);
    mist[sub_type] = exisiting_count + quantity;
};

void Building::create_ingredients(Ingredient::SubType sub_type, res_count_t quantity)
{
    if (this->count_ingredients(sub_type) + quantity > this->get_storage_space())
    {
        quantity = this->get_storage_space() - this->count_ingredients(sub_type);
    };

    if (quantity > 0)
    {
        create<mistIngredient>(this->ingredients, quantity, sub_type);
    } 
    else
    {
    };
};

void Building::create_products(Product::SubType sub_type, res_count_t quantity)
{
    create<mistProduct>(this->products, quantity, sub_type);
};

void Building::create_wastes(Waste::SubType sub_type, res_count_t quantity)
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

    for (auto harvester : this->harvesters) {
        std::pair<WorkerSubType, Ingredient::SubType> sub_type = harvester.first;
        WorkerSubType harv_type = sub_type.first;
        Ingredient::SubType ing_type = sub_type.second;
        res_count_t count = harvester.second;

        auto temp_harvester = std::make_shared<Harvester>(this->shared_from_this(), "test worker", ing_type, harv_type);
        temp_harvester->active_count = count;
        temp_harvester->update(dt);
    };

    for (auto consumer : this->consumers) {
        std::pair<WorkerSubType, Ingredient::SubType> sub_type = consumer.first;
        WorkerSubType harv_type = sub_type.first;
        Ingredient::SubType ing_type = sub_type.second;
        res_count_t count = consumer.second;

        auto temp_consumer = std::make_shared<ConsumerHarvester>(this->shared_from_this(), "test worker", ing_type, harv_type);
        temp_consumer->active_count = count;
        temp_consumer->update(dt);
    };


    if (update_clock->passed_threshold())
    {
        /* this->spawn_clock->update(dt); */
        // this->do_task(dt);
        // CCLOG("building update %s", this->name.c_str());

        update_clock->reset();

        for (auto saleman : this->salesmen) {
            std::pair<WorkerSubType, Ingredient::SubType> sub_type = saleman.first;
            WorkerSubType harv_type = sub_type.first;
            Ingredient::SubType ing_type = sub_type.second;
            res_count_t count = saleman.second;

            auto temp_harvester = std::make_shared<Salesman>(this->shared_from_this(), "test salesman", ing_type, harv_type);
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

res_count_t Building::count_ingredients()
{
    res_count_t total = 0;
    for (auto type_str : Ingredient::type_map)
    {
        Ingredient::SubType type = type_str.first;
        res_count_t count = map_get(this->ingredients, type, 0);
        total += count;
    };
    return total;
};

std::string Building::get_ingredients()
{
    std::stringstream ss;
    for (auto type_str : Ingredient::type_map)
    {
        Ingredient::SubType type = type_str.first;
        std::string str = type_str.second;
        unsigned int count = this->ingredients[type];
        if (count != 0)
        {
            ss << str << "(x" << count << ") ";
        }
    };
    return ss.str();
};

res_count_t Building::count_products()
{
    res_count_t total = 0;
    for (auto type_str : Product::type_map)
    {
        Product::SubType type = type_str.first;
        res_count_t count = map_get(this->products, type, 0);
        total += count;
    };
    return total;
};

std::string Building::get_products()
{
    std::stringstream ss;
    for (auto type_str : Product::type_map)
    {
        Product::SubType type = type_str.first;
        std::string str = type_str.second;
        unsigned int count = this->products[type];
        if (count != 0)
        {
            ss << str << "(x" << count << ") ";
        }
    };
    return ss.str();
};

res_count_t Building::count_wastes()
{
    res_count_t total = 0;
    for (auto type_str : Waste::type_map)
    {
        Waste::SubType type = type_str.first;
        res_count_t count = map_get(this->wastes, type, 0);
        total += count;
    };
    return total;
};

std::string Building::get_wastes()
{
    std::stringstream ss;
    for (auto type_str : Waste::type_map)
    {
        Waste::SubType type = type_str.first;
        std::string str = type_str.second;
        unsigned int count = this->wastes[type];
        if (count != 0)
        {
            ss << str << "(x" << count << ") ";
        }
    };
    return ss.str();
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

res_count_t Building::get_storage_space()
{
    std::map<int, res_count_t> level_output = {
        { 1, 25.0},
        { 2, 50.0},
        { 3, 500.0},
        { 4, 5000.0},
        { 5, 50000.0},
        { 6, 5000000.0},
        { 7, 50000000.0},
        { 8, 500000000.0},
        { 9, 5000000000.0},
        {10, 50000000000.0},
        {11, 500000000000.0},
        {12, 5000000000000.0},
        {13, 50000000000000.0},
        {14, 500000000000000.0},
        {15, 5000000000000000.0},
        {16, 500000000000000000.0}
    };
    return map_get(level_output, this->building_level, 9999999);
}

bool Building::is_storage_full_of_ingredients(Ingredient::SubType sub_type)
{
    return this->can_fit_more_ingredients(sub_type, 0);
};

bool Building::can_fit_more_ingredients(Ingredient::SubType sub_type, res_count_t quantity /*= 1*/)
{
    return this->count_ingredients(sub_type) + quantity <= this->get_storage_space();
};

void test_conditions()
{
    mistIngredient inputs = {
        { Ingredient::SubType::Grain , 2 },
        { Ingredient::SubType::Iron, 1 }
    };

    auto city = new Village(NULL, "The Test City");
    auto farm = std::make_shared<Building>(city, "The Test Farm", "test_farm");
    farm->ingredients = inputs;
    
    ResourceCondition* rc = ResourceCondition::create_ingredient_condition(Ingredient::SubType::Grain, 2, "test condition");
    // IngredientCondition rc = IngredientCondition(
    //         IngredientCondition::TypeChoices::Ingredient,
    //         Ingredient::SubType::Grain,
    //         2,
    //         "test condition");

    assert(rc->is_satisfied(farm) == true);

    farm->ingredients = mistIngredient({
        { Ingredient::SubType::Iron, 1 }
    });
    assert(rc->is_satisfied(farm) == false);

    rc->ing_type = Ingredient::SubType::Iron;
    assert(rc->is_satisfied(farm) == false);
    rc->quantity = 1;
    assert(rc->is_satisfied(farm) == true);

    delete rc;

};

///TODO fix this to work with mist
//void test_recipe()
//{
//    mistIngredient inputs = {
//        { Ingredient::SubType::Grain, 2 },
//        { Ingredient::SubType::Iron, 1 }
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

    this->server_clock->update(dt);
    if (this->server_clock->passed_threshold()){
        this->server_clock->reset();

        //try to update remote server
        this->post_update();

    };
};

void Buildup::post_update()
{
    //Use username to auto update
    auto username = DataManager::get_string_from_data("username", "");
    CCLOG("the existing username is '%s'", username.c_str());

    if (username == "") {
        CCLOG("blank username, not going to update");
        return;
    }

    //auto server_url = "http://tankorsmash.webfactional.com/users/"+username;
    auto server_url = "http://localhost:8080/users/" + username;

    //build up string of coins
    auto coins = (long long)BEATUP->get_total_coins();
    std::stringstream coin_stream;
    coin_stream << coins;
    std::string coins_string = coin_stream.str();
    coins_string = coins_string.substr(0, coins_string.find('.'));

    //create a json doc, set the { 'coins' : coins } json obj
    rapidjson::Document doc = rapidjson::Document();
    doc.SetObject();

    //build the Values that become the key and values
    rapidjson::Value key = rapidjson::Value(rapidjson::kStringType);
    key.SetString("coins");
    rapidjson::Value value = rapidjson::Value();
    value.SetString(coins_string.c_str(), coins_string.size());

    //all the member to the document
    rapidjson::Document::AllocatorType& allocator = doc.GetAllocator();
    doc.AddMember(key, value, allocator);

    //write out the json string
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    doc.Accept(writer);

    NetworkConsole::post_helper(server_url,
        buffer.GetString(),
        [server_url](std::string response)
    {
        CCLOG("response from %s:\n %s", server_url.c_str(), response.c_str());
    });

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


    buildup->server_clock = std::make_shared<Clock>(5.0f); //update server every 5 seconds

    auto city = new Village(buildup, "Burlington");

    struct BuildingConfig {
        std::string name;
        std::string id;
        Ingredient::SubType ing_type;
    };

    std::vector<BuildingConfig> configs = {
        {
            "The Farm", "the_farm", Ingredient::SubType::Grain
        }, {
            "The Arena", "the_arena", Ingredient::SubType::Sand
        }, {
            "The Underscape", "the_underscape", Ingredient::SubType::Blood
        }, {
            "The Marketplace", "the_marketplace", Ingredient::SubType::Seed
        }, {
            "The Dump", "the_dump", Ingredient::SubType::Fly
        }, {
            "The Workshop", "the_workshop", Ingredient::SubType::Seed
        }, {
            "The Mine", "the_mine", Ingredient::SubType::Copper
        }, {
            "The Graveyard", "the_graveyard", Ingredient::SubType::Flesh
        }, {
            "The Forest", "the_forest", Ingredient::SubType::Berry
        }
    };

    auto create_building = [city](BuildingConfig config) {
        auto building = std::make_shared<Building>(city, config.name, config.id);
        building->punched_sub_type = config.ing_type;
        city->buildings.push_back(building);
    };

    for (auto config : configs) {
        create_building(config);
    };

    auto farm = city->building_by_name("The Farm");
    farm->workers.push_back(std::make_shared<Worker>(farm, "Farmer", WorkerSubType::ZERO));
    buildup->set_target_building(farm);

    buildup->fighter = std::make_shared<Fighter>("PC");
    buildup->fighter->sprite_name = "townsmen8x8.png";
    buildup->fighter->team = Fighter::TeamOne;
    buildup->fighter->attrs->health->set_vals(100);

    buildup->brawler = std::make_shared<Fighter>("BRAWLER");
    buildup->brawler->team = Fighter::TeamOne;
    buildup->brawler->sprite_name = "ogre10x10.png";
    buildup->brawler->attrs->health->set_vals(200);


    auto arena = city->building_by_name("The Arena");
    arena->fighters.push_back(buildup->fighter);
    arena->fighters.push_back(buildup->brawler);

    Animal animal("ASD");
    auto dump = city->building_by_name("The Dump");
    animal.b2b_transfer(farm, dump, Resource::Ingredient, 10);


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

