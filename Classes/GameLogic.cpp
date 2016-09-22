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
#include "Item.h"
#include "StaticData.h"

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

    //add popup to message to fill out username
    if (username == "") {
        auto popup = GameLogic::get_popup_panel();
        popup->set_string("Hey there! Don't forget to set a username along the right side of the screen, and check out how you stack up on the leaderboard");
        popup->animate_open();
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

    CCLOG("%s", gains_ss.str().c_str());

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

void GameLogic::add_city_investment(double value)
{
    DataManager::incr_key("city_investment", value);
};

int GameLogic::get_city_investment()
{
    return DataManager::get_int_from_data("city_investment");
};

void GameLogic::add_appeasements(double value)
{
    DataManager::incr_key("appeasements", value);
};

int GameLogic::get_appeasements()
{
    return DataManager::get_int_from_data("appeasements");
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
    BUILDUP->city->name = DataManager::get_string_from_data("city_name", "");
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

    //title
    auto title_lbl = dynamic_cast<ui::Text*>(panel->getChildByName("title_lbl"));
    set_aliasing(title_lbl);

    //city name
    auto city_name_panel = dynamic_cast<ui::Layout*>(panel->getChildByName("city_name_panel"));
    auto city_name_input = dynamic_cast<ui::TextField*>(city_name_panel->getChildByName("city_name_textfield"));
    set_aliasing(city_name_input);
    city_name_input->setString(DataManager::get_string_from_data("city_name", ""));

    auto is_bad_character = [](char character){
        return !(std::isalnum(character) || character == '_');
    };
    auto clean_city_name = [is_bad_character](std::string city_name) {
        city_name.erase(std::remove_if(city_name.begin(), city_name.end(), is_bad_character), city_name.end());
        return city_name;
    };
    auto textfield_listener = [city_name_input, clean_city_name](Ref* target, ui::TextField::EventType evt)
    {
        if (evt == ui::TextField::EventType::ATTACH_WITH_IME || 
                evt == ui::TextField::EventType::DETACH_WITH_IME)
        {
            std::string text = city_name_input->getString();
            CCLOG("Got raw city_name from city_name_input: %s", text.c_str());
            std::string cleaned_city_name = clean_city_name(text);
            CCLOG("is cleaned to %s", cleaned_city_name.c_str());
            city_name_input->setString(cleaned_city_name);
            BUILDUP->city->name = cleaned_city_name;

            DataManager::set_string_from_data("city_name", cleaned_city_name);
        }
    };
    city_name_input->addEventListener(textfield_listener);

    //invest panel
    auto invest_panel = dynamic_cast<ui::Layout*>(panel->getChildByName("invest_panel"));
    auto invest_button = dynamic_cast<ui::Button*>(invest_panel->getChildByName("invest_button"));
    invest_button->addTouchEventListener([](Ref*, ui::Widget::TouchEventType evt)
    {
        if (evt == ui::Widget::TouchEventType::ENDED)
        {
            CCLOG("Add money");
            double coins = BEATUP->get_total_coins();
            if (coins > 10)
            {
                GameLogic::getInstance()->add_city_investment(10.0);
                BEATUP->add_total_coin(-10);
            }
        }
    });
    load_default_button_textures(invest_button);
    cocos2d::TTFConfig ttf_config = TTFConfig("pixelmix.ttf", 24, GlyphCollection::ASCII, NULL, false, 2);
    Label* renderer = invest_button->getTitleRenderer();
    renderer->setTTFConfig(ttf_config);

    auto invested_lbl = dynamic_cast<ui::Text*>(invest_panel->getChildByName("invested_lbl"));
    auto update_invested = [invested_lbl](float dt) {
        res_count_t invested = (res_count_t)GameLogic::getInstance()->get_city_investment();
        invested_lbl->setString(beautify_double(invested));
    };
    invested_lbl->schedule(update_invested, AVERAGE_DELAY, "update_invested");
    update_invested(0);

    //appeasement panel
    auto appeasement_panel = dynamic_cast<ui::Layout*>(panel->getChildByName("appeasement_panel"));
    auto appeasement_button = dynamic_cast<ui::Button*>(appeasement_panel->getChildByName("appeasement_button"));
    appeasement_button->addTouchEventListener([](Ref*, ui::Widget::TouchEventType evt)
    {
        if (evt == ui::Widget::TouchEventType::ENDED)
        {
            CCLOG("TODO add appeasement");
            mistIngredient city_ingredients = BUILDUP->get_all_ingredients();
            res_count_t _def = 0;
            IngredientSubType ing_type = IngredientSubType::Soul;
            res_count_t appease_count = 1.0;
            if (map_get(city_ingredients, ing_type, _def) >= appease_count)
            {
                 GameLogic::getInstance()->add_appeasements(appease_count);
                 BUILDUP->remove_shared_ingredients_from_all(ing_type, appease_count);
            }
        }
    });
    auto appeasement_btn_update = [appeasement_button](float dt) {
        mistIngredient city_ingredients = BUILDUP->get_all_ingredients();
        res_count_t _def = 0;
        auto it = Ingredient::SubType::Soul;
        if (map_get(city_ingredients, it, _def) < 1.0)
        {
            //this doesnt let the button accept touches, so we need to subclass widget or something to fix it
            try_set_enabled(appeasement_button, false);
        }
        else
        {
            try_set_enabled(appeasement_button, true);
        }
    };
    appeasement_button->schedule(appeasement_btn_update, SHORT_DELAY, "update_btn");

    load_default_button_textures(appeasement_button);
    Label* appeasement_renderer = appeasement_button->getTitleRenderer();
    appeasement_renderer->setTTFConfig(ttf_config);

    auto appeased_lbl = dynamic_cast<ui::Text*>(appeasement_panel->getChildByName("appeased_lbl"));
    auto update_appeasemented = [appeased_lbl](float dt) {
        res_count_t appeasemented = (res_count_t)GameLogic::getInstance()->get_appeasements();
        appeased_lbl->setString(beautify_double(appeasemented));
    };
    appeased_lbl->schedule(update_appeasemented, AVERAGE_DELAY, "update_appeasemented");
    update_appeasemented(0);


    auto back_btn = dynamic_cast<ui::Button*>(panel->getChildByName("back_btn"));
    Label* button_lbl = back_btn->getTitleRenderer();
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

    auto items_scene_btn = dynamic_cast<ui::Button*>(panel->getChildByName("items_scene_btn"));
    Label* items_scene_lbl = items_scene_btn->getTitleRenderer();
    items_scene_lbl->setTextColor(Color4B::WHITE);
    items_scene_lbl->enableOutline(Color4B::BLACK, 2);

    items_scene_btn->addTouchEventListener([](Ref* touch, ui::Widget::TouchEventType type){
        if (type == ui::Widget::TouchEventType::ENDED)
        {
            GameDirector::switch_to_items_menu();
        }
    });
    load_default_button_textures(items_scene_btn);

    auto director = cocos2d::Director::getInstance();
    director->pushScene(scene);
};

void GameDirector::switch_to_items_menu()
{
    auto inst = CSLoader::getInstance();
    auto items_scene_node = inst->CSLoader::createNode("editor/scenes/items_scene.csb");
    items_scene_node->removeFromParent();
    auto panel = items_scene_node->getChildByName("panel");

    auto scene = cocos2d::Scene::create();
    scene->setName("items_wrapper_scene");
    scene->addChild(items_scene_node);

    //title
    auto title_lbl = dynamic_cast<ui::Text*>(panel->getChildByName("title_lbl"));
    set_aliasing(title_lbl);

    //items listview
    auto items_listview = dynamic_cast<ui::ListView*>(panel->getChildByName("items_listview"));

    auto item_detail_panel = dynamic_cast<ui::Layout*>(panel->getChildByName("item_detail"));
    auto item_name = dynamic_cast<ui::Text*>(item_detail_panel->getChildByName("item_name"));
    auto item_desc = dynamic_cast<ui::Text*>(item_detail_panel->getChildByName("item_description"));
    auto item_sell_btn = dynamic_cast<ui::Button*>(item_detail_panel->getChildByName("item_sell"));

    auto update_item_detail_panel = [panel, item_name, item_desc, item_sell_btn](spItem item) {
        item_name->setString(item->get_name());
        item_desc->setString(item->description);
    };

    ItemData item_data = ItemData();
    spItem dagger = item_data.get_item("dagger");
    dagger->rarity = RarityType::Poor;
    spItem homunc = item_data.get_item("homunculus");
    homunc->rarity = RarityType::Rare;
    spItem ashen_mirror = item_data.get_item("ashen_mirror");
    spItem ashen_mirror_lv2 = item_data.get_item("ashen_mirror");
    ashen_mirror_lv2->level = 2.0f;

    vsItem items = {
        dagger,
        homunc,
        ashen_mirror,
        ashen_mirror_lv2
    };

    for (spItem item : BUILDUP->items) {
        auto nuitem = NuItem::create(items_listview);
        nuitem->set_title(item->get_name());
        nuitem->set_description(item->summary);

        res_count_t cost = item->get_effective_cost();
        std::string cost_str = beautify_double(cost);
        CCLOG("cost %f, beauty cost %s", cost, cost_str.c_str());
        nuitem->set_cost_lbl(cost_str);

        nuitem->button->addTouchEventListener([item, update_item_detail_panel](Ref* sender, ui::Widget::TouchEventType type){
            update_item_detail_panel(item);
        });
    };

    //back button
    auto back_btn = dynamic_cast<ui::Button*>(panel->getChildByName("back_btn"));
    Label* button_lbl = back_btn->getTitleRenderer();
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
