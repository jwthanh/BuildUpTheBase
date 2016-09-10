#include "GameLogic.h"

#include "Beatup.h"
#include "HouseBuilding.h"
#include "DataManager.h"
#include "NuMenu.h"
#include "Serializer.h"
#include "Util.h"
#include "cocostudio/ActionTimeline/CSLoader.h"
#include "MiscUI.h"
#include "Modal.h"
#include "external/easylogging.h"
#include "HarvestScene.h"

USING_NS_CC;

GameLogic* GameLogic::_instance = NULL;

GameLogic::GameLogic()
    : is_loaded(false)
{

};

void GameLogic::post_load()
{

    //dont re-load if done once already. this is because post_load gets called
    //onEnter, originally only from initializing game, but now we're using
    //different scenes
    if (is_loaded == true)
    {
        return;
    };

    //cheat to reset data to blank on load
    auto username = DataManager::get_string_from_data("username");
    if (username == "__resetdata")
    {
        DataManager::set_string_from_data("username", "");
        return;
    };

    CCLOG("loading game on startup, this should only happen once");
    GameLogic::load_all();

    std::stringstream gains_ss, at_capacity_ss;
    std::chrono::duration<double, std::ratio<3600>> hours_since_last_login = BEATUP->hours_since_last_login();


    auto original_ingredients = BUILDUP->get_all_ingredients();
    auto seconds = std::chrono::duration_cast<std::chrono::seconds>(hours_since_last_login);
    long long seconds_count = seconds.count();


    //if the time is is over a day in the passed, assume that they're cheating
    // anything less than 24 hours could be a timezone thing
    bool is_cheating = false;
    if (hours_since_last_login.count() < -24.0)
    {
        is_cheating = true;
    }

    if (is_cheating == false)
    {
        BUILDUP->city->update(seconds_count);

        auto new_ingredients = BUILDUP->get_all_ingredients();
        for (auto mist_ing : new_ingredients)
        {
            Ingredient::SubType ing_type = mist_ing.first;
            res_count_t new_count = mist_ing.second;

            res_count_t _def = 0;
            res_count_t old_count = map_get(original_ingredients, ing_type, _def);

            if (new_count - old_count > 0.0)
            {
                res_count_t gained = new_count - old_count;
                gains_ss << "+Gained " << beautify_double(gained) << " " << Ingredient::type_to_string(ing_type);

                for (spBuilding building : BUILDUP->city->buildings)
                {
                    if (building->is_storage_full_of_ingredients(ing_type))
                    {
                        at_capacity_ss << "- Upgrade " << building->name << " to fit more " << Ingredient::type_to_string(ing_type) << "!" << std::endl;
                    }

                }

                gains_ss << std::endl;
            }
        }
    }
    else
    {
        gains_ss << "Awfully suspicious..." << std::endl;
        LOG(WARNING) << "time jump cheat detected";

        for (spBuilding building : BUILDUP->city->buildings)
        {
            building->ingredients = mistIngredient();
        }

        BEATUP->_total_coins = 0;

        GameLogic::load_all_as_cheater();
    }


    res_count_t hours_since_login = hours_since_last_login.count();
    if (hours_since_login < 1.0)
    {
        auto minutes_since_last_login = std::chrono::duration_cast<std::chrono::minutes>(hours_since_last_login);
        res_count_t minutes_since_login = minutes_since_last_login.count();
        gains_ss << "\nIt's been " << beautify_double(minutes_since_login) << " minutes since last login.";
    }
    else
    {
        gains_ss << "\nIt's been " << beautify_double(hours_since_login) << " hours since last login.";
    }

    CCLOG(gains_ss.str().c_str());

    auto scene = cocos2d::Director::getInstance()->getRunningScene()->getChildByName("HarvestScene");

    //TODO clean up modal memory
    TextBlobModal* modal = new TextBlobModal(scene);
    modal->set_title("Welcome Back!");

    if (at_capacity_ss.str() != "")
    {
        modal->set_body(at_capacity_ss.str() + "\n---\n\n" + gains_ss.str());
    }
    else
    {
        modal->set_body(gains_ss.str());
    }

    //set the last login time, set here and on save
    BEATUP->set_last_login();
    LOG(INFO) << "Game loaded!";

    this->is_loaded = true;
}

bool GameLogic::init()
{
    GameLogic* instance = GameLogic::getInstance();
    instance->beatup = Beatup::create();
    instance->beatup->setName("beatup");
    instance->beatup->retain();
    instance->buildup = instance->beatup->buildup;

    this->coin_save_clock = std::make_shared<Clock>(15.0f);
    this->coin_rate_per_sec_clock = std::make_shared<Clock>(1.0f);
    this->power_sell_all_cooldown = std::make_shared<Clock>(5.0f);
    this->power_sell_all_cooldown->start_time = 99999.0f; //start this timer off completed

    return true;
};

void GameLogic::update(float dt)
{
    this->beatup->update_buildup(dt);

    this->coin_save_clock->update(dt);
    if (this->coin_save_clock->passed_threshold())
    {
        this->coin_save_clock->reset();
        DataManager::set_double_from_data(Beatup::total_coin_key, this->beatup->get_total_coins());
    }

    this->coin_rate_per_sec_clock->update(dt);
    if (this->coin_rate_per_sec_clock->passed_threshold())
    {
        this->coin_rate_per_sec_clock->reset();
        auto scene = cocos2d::Director::getInstance()->getRunningScene()->getChildByName("HarvestScene");
        if (scene) //might be at some other screen
        {

            ui::Layout* player_info_panel = dynamic_cast<ui::Layout*>(scene->getChildByName("player_info_panel"));
            auto player_gold_per_sec_lbl = dynamic_cast<ui::Text*>(player_info_panel->getChildByName("player_gold_per_sec_lbl"));

            res_count_t coin_diff = BEATUP->get_total_coins() - BEATUP->_last_total_coins;
            std::string prefix = "+";
            if (coin_diff < 0.0) {
                prefix = "";
            };

            std::string beautified_diff = prefix+beautify_double(coin_diff)+"/sec";
            player_gold_per_sec_lbl->setString(beautified_diff);

            BEATUP->_last_total_coins = BEATUP->get_total_coins();

        };
    }

    //only ever increment this, it'll get reset on callback for power in sidebar
    this->power_sell_all_cooldown->update(dt);
};

GameLogic* GameLogic::getInstance()
{
    if (!GameLogic::_instance)
    {
        GameLogic::_instance = new GameLogic();
        GameLogic::_instance->init();
    };

    return GameLogic::_instance;
};


std::shared_ptr<PopupPanel> GameLogic::get_popup_panel()
{
    auto scene = dynamic_cast<HarvestScene*>(cocos2d::Director::getInstance()->getRunningScene()->getChildByName("HarvestScene"));
    if (scene == NULL)
    {
        CCLOG("ERROR: Tried to get_popup_panel and find HarvestScene but couldn't");
    }
    return scene->popup_panel;
};

void GameLogic::add_total_harvests(int value)
{
    DataManager::incr_key("total_harvests", value);
};

int GameLogic::get_total_harvests()
{
    return DataManager::get_int_from_data("total_harvests");
};

void GameLogic::add_total_kills(int value)
{
    DataManager::incr_key("total_kills", value);
};

int GameLogic::get_total_kills()
{
    return DataManager::get_int_from_data("total_kills");
};

void GameLogic::save_all()
{
    LOG(INFO) << "Starting save";
    for (spBuilding building : BUILDUP->city->buildings)
    {
        auto bldg_serializer = BuildingSerializer("test_building.json", building);
        bldg_serializer.serialize();
    };

    //save coins
    DataManager::set_double_from_data(Beatup::total_coin_key, BEATUP->get_total_coins());

    //set the last login time, set here and on load
    LOG(INFO) << "Marking last login";
    BEATUP->set_last_login();

    //update the remote server
    LOG(INFO) << "Posting to leaderboard";
    BUILDUP->post_update();
    LOG(INFO) << "Completed post to leaderboard";
};

void GameLogic::load_all()
{
    for (spBuilding building : BUILDUP->city->buildings)
    {
        auto bldg_serializer = BuildingSerializer("test_building.json", building);
        bldg_serializer.load();
    };
    BEATUP->_total_coins = DataManager::get_double_from_data(Beatup::total_coin_key);
};

void GameLogic::load_all_as_cheater()
{
    for (spBuilding building : BUILDUP->city->buildings)
    {
        auto bldg_serializer = BuildingSerializer("cheat_building.json", building);
        bldg_serializer.load();
    };

    BEATUP->_total_coins = 0.0;
};


void GameDirector::switch_to_building_menu()
{
    auto scene = cocos2d::Scene::create();
    BuildingNuMenu* building_menu = BuildingNuMenu::create(
        BUILDUP->get_target_building()
        );
    scene->addChild(building_menu);

    auto director = cocos2d::Director::getInstance();
    director->pushScene(scene);
};

void GameDirector::switch_to_city_menu()
{
    auto inst = CSLoader::getInstance();
    auto city_menu_scene_node = inst->CSLoader::createNode("editor/scenes/city_menu_scene.csb");
    city_menu_scene_node->removeFromParent();
    auto panel = city_menu_scene_node->getChildByName("panel");

    auto scene = cocos2d::Scene::create();
    scene->setName("city_wrapper_scene");
    scene->addChild(city_menu_scene_node);

    auto back_btn = dynamic_cast<ui::Button*>(panel->getChildByName("back_btn"));
    Label* button_lbl = (Label*)back_btn->getTitleRenderer();
    button_lbl->setTextColor(Color4B::WHITE);
    button_lbl->enableOutline(Color4B::BLACK, 2);

    back_btn->addTouchEventListener([](Ref* touch, ui::Widget::TouchEventType type){
        if (type == ui::Widget::TouchEventType::ENDED)
        {
            auto director = Director::getInstance();
            director->popScene();
        }
    });
    load_default_button_textures(back_btn);

    auto director = cocos2d::Director::getInstance();
    director->pushScene(scene);
};
