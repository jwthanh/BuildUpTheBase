#include "Buildup.h"

#include <sstream>

#include <ck/ck.h>
#include <json/stringbuffer.h>
#include <json/writer.h>

#include "Fighter.h"
#include "combat.h"
#include "GameLogic.h"
#include "Worker.h"
#include "Beatup.h"
#include "attribute_container.h"
#include "attribute.h"
#include "progress/GameProgress.h"
#include "HouseBuilding.h"
#include "DataManager.h"
#include "FileOperation.h"
#include "Network.h"
#include "Util.h"

Buildup::Buildup()
{
    this->city = NULL;
    this->set_target_building(NULL);

    this->fighter = std::make_shared<Fighter>("PlayerAvatar");
	auto fighter_combat = std::make_shared<Combat>("playeravatar combat", this->fighter);
	this->fighter->combat = fighter_combat;
    this->fighter->attrs->health->set_vals(100);

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

void Buildup::update(float dt)
{
    this->city->update(dt);

    CkUpdate();

    this->server_clock.update(dt);
    if (this->server_clock.passed_threshold()){
        this->server_clock.reset();

        //try to update remote server
        this->post_update();

    };

    GameProgress* game_progress = GameProgress::getInstance();
    game_progress->update(dt, this->city);
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

City* Buildup::init_city(Buildup* buildup)
{
    test_beautify_double();

    buildup->server_clock = Clock(30.0f); //update server every 30 seconds

    auto city = new City(buildup, "Burlington");

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
    farm->set_been_bought(true);
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
    return map_get(BUILDUP->get_all_ingredients(), ing_type, def);
};
