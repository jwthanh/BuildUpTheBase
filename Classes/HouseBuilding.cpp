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

#include "HouseBuilding.h"

#include "Recipe.h"
#include "Worker.h"

#include "Util.h"

#include "FileOperation.h"
#include "StaticData.h"

#include "Fighter.h"
#include "Combat.h"

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
#include "Item.h"



USING_NS_CC;


Buildup::Buildup()
{
    this->city = NULL;
    this->set_target_building(NULL);

    this->fighter = std::make_shared<Fighter>("PlayerAvatar");
	auto fighter_combat = std::make_shared<Combat>("playeravatar combat", this->fighter);
	this->fighter->combat = fighter_combat;
    this->fighter->attrs->health->set_vals(100);

    // this->items = {};

    ItemData item_data = ItemData();
    spItem dagger = item_data.get_item("dagger");
    dagger->rarity = ItemRarityType::Poor;
    spItem homunc = item_data.get_item("homunculus");
    homunc->rarity = ItemRarityType::Rare;
    spItem ashen_mirror = item_data.get_item("ashen_mirror");
    spItem ashen_mirror_lv2 = item_data.get_item("ashen_mirror");
    ashen_mirror_lv2->level = 2.0f;

    this->items = {
        dagger,
        homunc,
        ashen_mirror,
        ashen_mirror_lv2
    };


    this->player = NULL;
};

///for building in buildings, sum up their ingredients and return the map
mistIngredient& Buildup::get_all_ingredients()
{
    return this->_all_ingredients;
}

//remove ing type count from list, making sure it doesnt result in less than 0
void Buildup::remove_shared_ingredients_from_all(Ingredient::SubType ing_type, res_count_t removals)
{
    mistIngredient& all_ingredients = this->get_all_ingredients();
    all_ingredients[ing_type] -= removals;

	//make sure there's never less than 0
    if (all_ingredients[ing_type] < 0.0)
    {
        all_ingredients[ing_type] = 0.0;
    };
};

void Village::update(float dt)
{
    Updateable::update(dt);

    this->update_buildings(dt);
};


void Village::update_buildings(float dt)
{
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
    CCLOG("move if sized shouldnt be called");
    //unsigned int from_size;
    //if (res_type == Resource::Ingredient) from_size = from_bldg->ingredients.size();
    //else if (res_type == Resource::Product) from_size = from_bldg->products.size();
    //else if (res_type == Resource::Waste) from_size = from_bldg->wastes.size();

    //if (from_size >= condition_size)
    //{
    //    if (move_count == 0) { move_count = condition_size; };
    //    Animal animal = Animal("Horse");
    //    animal.b2b_transfer(
    //        from_bldg,
    //        to_bldg,
    //        res_type,
    //        move_count
    //        );
    //    callback();
    //};
};

Building::Building(Village* city, std::string name, std::string id_key) :
             Nameable(name), Buyable(id_key), Updateable(), city(city)
{
    num_workers = 1;

    building_level = 1;

    update_clock.set_threshold(1.0f);
    spawn_clock = Clock(3);

    workers = vsWorker();

    harvesters = mistHarvester();
    _harvester_cache = std::make_shared<std::map<work_ing_t, std::shared_ptr<Harvester>>>(); //cant just HarvesterCache(); this for some reason

    salesmen = mistHarvester();
    _salesmen_cache = std::make_shared<std::map<work_ing_t, std::shared_ptr<Salesman>>>();

    consumers = mistHarvester();
    _consumer_cache = std::make_shared<std::map<work_ing_t, std::shared_ptr<ConsumerHarvester>>>();

    this->data = std::make_shared<BuildingData>(name);
    this->_shop_cost = atoi(this->data->get_gold_cost().c_str());
    this->short_name = this->data->get_short_name();

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
        total += Harvester::get_to_harvest_count(harv_type, ing_type)*active_count;
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

template<typename T>
std::shared_ptr<T> create_one(typename T::SubType sub_type)
{
    return std::make_shared<T>(sub_type);
};

template<typename mistT>
void create(mistT& mist, res_count_t quantity, typename mistT::key_type sub_type)
{
    res_count_t def = 0;
    auto exisiting_count = map_get(mist, sub_type, def);
    mist[sub_type] = exisiting_count + quantity;
};

void Building::create_ingredients(Ingredient::SubType sub_type, res_count_t quantity)
{
    //NOTE this compares this buildings limit against all the ingredients in the city. this might get weird
    if (BUILDUP->count_ingredients(sub_type) + quantity > this->get_storage_space())
    {
        quantity = this->get_storage_space() - BUILDUP->count_ingredients(sub_type);
    };

    if (quantity > 0)
    {
        mistIngredient& all_ingredients = BUILDUP->get_all_ingredients();
        create<mistIngredient>(all_ingredients, quantity, sub_type);
    }
    else
    {
    };
};

void Building::consume_recipe(Recipe* recipe)
{
    auto& all_ingredients = BUILDUP->get_all_ingredients();
    if (recipe->is_satisfied(all_ingredients))
    {
        recipe->consume();
    }
};


template<typename CacheT>
typename CacheT::element_type::mapped_type get_or_create_from_cache(spBuilding& building, CacheT cache, std::pair<WorkerSubType, Ingredient::SubType> key)
{
    typename CacheT::element_type::mapped_type temp_harvester;
    if (cache->find(key) != cache->end())
    {
        //pull it out of cache
        temp_harvester = (*cache)[key];
    }
    else
    {
        //create and add to cache
        temp_harvester = std::make_shared<typename CacheT::element_type::mapped_type::element_type>(building, "test worker", key.second, key.first);
        (*cache)[key] = temp_harvester;
    };

    return temp_harvester;
};

void Building::update(float dt)
{
    Updateable::update(dt);

    spBuilding shared_this = this->shared_from_this();

    for (auto mist : this->harvesters) {
        std::pair<WorkerSubType, Ingredient::SubType> sub_type = mist.first;
        WorkerSubType harv_type = sub_type.first;
        Ingredient::SubType ing_type = sub_type.second;
        res_count_t count = mist.second;

        //find cache worker, otherwise create and add it to cache
        std::shared_ptr<Harvester> harvester;
        std::pair<WorkerSubType, IngredientSubType> key = std::make_pair(harv_type, ing_type);
        harvester = get_or_create_from_cache(shared_this, this->_harvester_cache, key);



        harvester->active_count = count;
        harvester->update(dt);
    };

    if (update_clock.passed_threshold())
    {
        update_clock.reset();

        for (auto mist : this->salesmen) {
            std::pair<WorkerSubType, Ingredient::SubType> sub_type = mist.first;
            WorkerSubType harv_type = sub_type.first;
            Ingredient::SubType ing_type = sub_type.second;
            res_count_t count = mist.second;

            std::pair<WorkerSubType, IngredientSubType> key = std::make_pair(harv_type, ing_type);
            std::shared_ptr<Salesman> salesman = get_or_create_from_cache(shared_this, this->_salesmen_cache, key);
            salesman->active_count = count;
            salesman->update(dt);
        };

        for (auto mist : this->consumers) {
            std::pair<WorkerSubType, Ingredient::SubType> sub_type = mist.first;
            WorkerSubType harv_type = sub_type.first;
            Ingredient::SubType ing_type = sub_type.second;
            res_count_t count = mist.second;

            std::pair<WorkerSubType, IngredientSubType> key = std::make_pair(harv_type, ing_type);
            std::shared_ptr<ConsumerHarvester> consumer = get_or_create_from_cache(shared_this, this->_consumer_cache, key);
            consumer->active_count = count;
            consumer->update(dt);
        };


    }
};

res_count_t Building::get_storage_space()
{
    res_count_t _def = 9999999;
    return map_get(building_storage_limit, this->building_level, _def);
}

bool Building::is_storage_full_of_ingredients(Ingredient::SubType sub_type)
{
    return !this->can_fit_more_ingredients(sub_type, 1);
};

bool Building::can_fit_more_ingredients(Ingredient::SubType sub_type, res_count_t quantity /*= 1*/)
{
    //NOTE uses building storage but global ingredients, might get weird
    return BUILDUP->count_ingredients(sub_type) + quantity <= this->get_storage_space();
};

void test_conditions()
{
    //mistIngredient inputs = {
    //    { Ingredient::SubType::Grain , 2 },
    //    { Ingredient::SubType::Iron, 1 }
    //};

    //auto city = new Village(NULL, "The Test City");
    //auto farm = std::make_shared<Building>(city, "The Test Farm", "test_farm");
    //farm->ingredients = inputs;
    //
    //ResourceCondition* rc = ResourceCondition::create_ingredient_condition(Ingredient::SubType::Grain, 2, "test condition");
    //// IngredientCondition rc = IngredientCondition(
    ////         IngredientCondition::TypeChoices::Ingredient,
    ////         Ingredient::SubType::Grain,
    ////         2,
    ////         "test condition");

    //assert(rc->is_satisfied(farm) == true);

    //farm->ingredients = mistIngredient({
    //    { Ingredient::SubType::Iron, 1 }
    //});
    //assert(rc->is_satisfied(farm) == false);

    //rc->ing_type = Ingredient::SubType::Iron;
    //assert(rc->is_satisfied(farm) == false);
    //rc->quantity = 1;
    //assert(rc->is_satisfied(farm) == true);

    //delete rc;

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

    this->server_clock.update(dt);
    if (this->server_clock.passed_threshold()){
        this->server_clock.reset();

        //try to update remote server
        this->post_update();

    };
};

void Buildup::post_update()
{
    CCLOG("post_update to server");
    //Use username to auto update
    auto username = DataManager::get_string_from_data("username", "");

    if (username == "") {
        CCLOG("blank username, not going to update");
        return;
    }

    auto server_url = "http://tankorsmash.webfactional.com/users/"+username+"/";
    //auto server_url = "http://localhost:8080/users/"+ username+"/";

    //build up string of coins
    res_count_t coins = BEATUP->get_total_coins();
    std::stringstream coin_stream;
    coin_stream << coins;
    std::string coins_string = coin_stream.str();
    if (coins_string.find("e") == std::string::npos)
    {
        coins_string = coins_string.substr(0, coins_string.find('.'));
    }

    //create a json doc, set the { 'coins' : coins } json obj, along with the buildings json
    std::string savefile_path = "test_building.json";
    rjDocument doc = FileIO::open_json(savefile_path, false);

    //build the rjValues that become the key and values in the dict
    rjValue key = rjValue(rapidjson::kStringType);
    key.SetString("coins");
    rjValue value = rjValue();
    const char* raw_coin_cs = coins_string.c_str();
    value.SetString(raw_coin_cs, coins_string.size());

    //add the member to the document
    auto& allocator = doc.GetAllocator();
    doc.AddMember(key, value, allocator);


    //build last_login, overriding old vars
    time_t from_file = (time_t)DataManager::get_int_from_data(Beatup::last_login_key);
    key = rjValue(rapidjson::kStringType);
    key.SetString("last_login");
    value = rjValue();
    value.SetInt(from_file);

    doc.AddMember(key, value, allocator);


    //write out the json string
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    doc.Accept(writer);

    NetworkConsole::post_helper(server_url,
        buffer.GetString(),
        [server_url](std::string response)
    {
        CCLOG("done posting savefile to server...");
        CCLOG("...server response: %s", response.c_str());
    });

};

Village* Buildup::init_city(Buildup* buildup)
{


    test_beautify_double();


    buildup->server_clock = Clock(30.0f); //update server every 30 seconds

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
            "The Marketplace", "the_marketplace", Ingredient::SubType::Paper
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
	buildup->fighter->combat = std::make_shared<Combat>("PC combat", buildup->fighter);
    buildup->fighter->sprite_name = "townsmen8x8.png";
    buildup->fighter->team = Fighter::TeamOne;
    buildup->fighter->attrs->health->set_vals(100);

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

res_count_t Buildup::count_ingredients()
{
    CCLOG("when is building::count_ingredients called?");
    res_count_t total = 0;
    res_count_t def = 0;
    mistIngredient& all_ingredients = BUILDUP->get_all_ingredients();
    for (auto&& type_str : Ingredient::type_map)
    {
        Ingredient::SubType type = type_str.first;
        res_count_t count = map_get(all_ingredients, type, def);
        total += count;
    };
    return total;
};

res_count_t Buildup::count_ingredients(Ingredient::SubType ing_type)
{
    res_count_t def = 0;
    mistIngredient& all_ingredients = BUILDUP->get_all_ingredients();
    return map_get(all_ingredients, ing_type, def);
};

void Player::update(float dt)
{
};

