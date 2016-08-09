#include "GameLogic.h"

#include "Beatup.h"
#include "HouseBuilding.h"
#include "DataManager.h"
#include "NuMenu.h"
#include "Serializer.h"
#include "Util.h"
#include "cocostudio/ActionTimeline/CSLoader.h"
#include "MiscUI.h"

USING_NS_CC;

GameLogic* GameLogic::_instance = NULL;

GameLogic::GameLogic()
{

};

void GameLogic::post_load()
{

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

        for (spBuilding building : BUILDUP->city->buildings)
        {
            building->ingredients = mistIngredient();
        }

        BEATUP->_total_coins = 0;
    }



    res_count_t hours_since_login = hours_since_last_login.count();
    if (hours_since_login < 1.0)
    {
        auto minutes_since_last_login = std::chrono::duration_cast<std::chrono::minutes>(hours_since_last_login);
        res_count_t minutes_since_login = minutes_since_last_login.count();
        gains_ss << "\nIt's been " << beautify_double(minutes_since_login) << " minutes since last login";
    }
    else
    {
        gains_ss << "\nIt's been " << beautify_double(hours_since_login) << " hours since last login";
    }

    CCLOG(gains_ss.str().c_str());

    auto scene = cocos2d::Director::getInstance()->getRunningScene()->getChildByName("HarvestScene");
    auto inst = cocos2d::CSLoader::getInstance();
    cocos2d::Node* root_message_node = inst->CSLoader::createNode("editor/details/message_detail.csb");
    cocos2d::Node* message_panel = root_message_node->getChildByName("message_panel");
    message_panel->removeFromParent();
    message_panel->setPosition(GameLayer::get_center_pos());

    auto title_lbl = dynamic_cast<cocos2d::ui::Text*>(message_panel->getChildByName("title_lbl"));
    title_lbl->setString("Welcome Back!");
    set_aliasing(title_lbl, true);

    //set scrollbar up
    ui::ScrollView* body_scroll = dynamic_cast<ui::ScrollView*>(message_panel->getChildByName("body_scroll"));
    body_scroll->setScrollBarAutoHideEnabled(false);
    body_scroll->setScrollBarEnabled(true);
    body_scroll->scrollToTop(0.0f, false);

    //fill message up
    auto body_lbl = dynamic_cast<cocos2d::ui::Text*>(body_scroll->getChildByName("body_lbl"));
    if (at_capacity_ss.str() != "")
    {
        body_lbl->setString(at_capacity_ss.str() + "\n---\n\n" + gains_ss.str());
    }
    else
    {
        body_lbl->setString(gains_ss.str());
    }
    scene->addChild(message_panel);

    ui::Layout* close_panel = dynamic_cast<ui::Layout*>(message_panel->getChildByName("close_panel"));
    auto cb = [message_panel](Ref* target, ui::Widget::TouchEventType type) {
        if (type == ui::Widget::TouchEventType::ENDED)
        {
            message_panel->removeFromParent();
        };
    };
    close_panel->addTouchEventListener(cb);

    //set the last login time, set here and on save
    BEATUP->set_last_login();
}

bool GameLogic::init()
{
    GameLogic* instance = GameLogic::getInstance();
    instance->beatup = Beatup::create();
    instance->beatup->setName("beatup");
    instance->beatup->retain();
    instance->buildup = instance->beatup->buildup;

    this->coin_save_clock = new Clock(15.0f);
    this->coin_rate_per_sec_clock = new Clock(1.0f);

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
            std::string beautified_diff = "+"+beautify_double(coin_diff)+"/sec";
            player_gold_per_sec_lbl->setString(beautified_diff);

            BEATUP->_last_total_coins = BEATUP->get_total_coins();

        };
    }
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

void GameLogic::add_total_harvests(int value)
{
    DataManager::incr_key("total_harvests", value);
};

int GameLogic::get_total_harvests()
{
    return DataManager::get_int_from_data("total_harvests");
};

void GameLogic::save_all()
{
    for (spBuilding building : BUILDUP->city->buildings)
    {
        auto bldg_serializer = BuildingSerializer("test_building.json", building);
        bldg_serializer.serialize();
    };

    //save coins
    DataManager::set_double_from_data(Beatup::total_coin_key, BEATUP->get_total_coins());

    //set the last login time, set here and on load
    BEATUP->set_last_login();

    //update the remote server
    BUILDUP->post_update();
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
