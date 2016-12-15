#include "GameLogic.h"

#include "NodeBuilder.h"
#include "base/CCDirector.h"
#include "2d/CCSprite.h"

#include "ui/UIListView.h"
#include "ui/UILayout.h"
#include "ui/UIText.h"
#include "ui/UIButton.h"
#include "ui/UITextField.h"
#include "magic_particles/MagicEmitter.h"
#include "magic_particles/mp_cocos.h"

#include "Beatup.h"
#include "HouseBuilding.h"
#include "DataManager.h"
#include "NuMenu.h"
#include "Serializer.h"
#include "Util.h"
#include "MiscUI.h"
#include "Modal.h"
#include "external/easylogging.h"
#include "HarvestScene.h"
#include "Item.h"
#include "StaticData.h"
#include "BuildingsSideBar.h"
#include "Miner.h"
#include "FShake.h"
#include "ItemScene.h"
#include "Equipment.h"



USING_NS_CC;

GameLogic* GameLogic::_instance = NULL;

GameLogic::GameLogic()
    : is_loaded(false), _can_vibrate(true),
        beatup(nullptr), buildup(nullptr),
        city_investment(0), appeasements(0)
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
        popup->set_string("Hey there! Don't forget to set a username in green along the right side of the screen, and check out how you stack up on the leaderboard");
        popup->animate_open();
    };

    CCLOG("loading game on startup, this should only happen once");
    GameLogic::load_all();

    bool show_welcome_modal = true;
    // bool show_welcome_modal = false;
    if (show_welcome_modal)
    {
        auto scene = cocos2d::Director::getInstance()->getRunningScene()->getChildByName("HarvestScene");
        TextBlobModal* modal = new TextBlobModal(scene);

        std::string modal_content = "";
        time_t from_file = static_cast<time_t>(DataManager::get_int_from_data(Beatup::last_login_key));
        if (from_file == 0)
        {
            modal->set_title("Welcome to Build Up The Base!");
            modal_content = this->new_player_load();
        }
        else
        {
            modal->set_title("Welcome Back!");
            modal_content = this->existing_player_load();
        }

        //TODO clean up modal memory

        modal->set_body(modal_content);
        
    }

    //set the last login time, set here and on save
    BEATUP->set_last_login();
    LOG(INFO) << "Game loaded!";

    this->is_loaded = true;
}

std::string GameLogic::new_player_load()
{
    std::stringstream ss;
    ss << "Tap the farm in the middle of the screen to harvest, then sell by tapping the grain icon along the bottom." << std::endl << std::endl;
    ss << "Don't forget to enter a customized username so you can compete on the leaderboards at http://buildupthebase.com" << std::endl << std::endl;

    return ss.str();

}

std::string GameLogic::existing_player_load()
{
    std::stringstream gains_ss, at_capacity_ss;
    std::chrono::duration<double, std::ratio<3600>> hours_since_last_login = BEATUP->hours_since_last_login();


    auto original_ingredients = BUILDUP->get_all_ingredients(); //NOTE create a copy to reference later
    auto seconds = std::chrono::duration_cast<std::chrono::seconds>(hours_since_last_login);
    long long seconds_count = seconds.count();


    //if the time is is over a day in the passed, assume that they're cheating
    // anything less than 24 hours could be a timezone thing
    bool is_cheating = hours_since_last_login.count() < -24.0;

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

                ////FIXME since moving to shared ingredients,
                //building->is_storage_full_of_ingredients checks global
                //ingredients, so this needs to check only the appropriate
                //building
                // for (spBuilding building : BUILDUP->city->buildings)
                // {
                //     if (building->is_storage_full_of_ingredients(ing_type))
                //     {
                //         at_capacity_ss << "- Upgrade " << building->name << " to fit more " << Ingredient::type_to_string(ing_type) << "!" << std::endl;
                //     }

                // }

                gains_ss << std::endl;
            }
        }
    }
    else
    {
        gains_ss << "Awfully suspicious..." << std::endl;
        LOG(WARNING) << "time jump cheat detected";

        CCLOG("!!!todo load ingredients from file");

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
    else if (hours_since_login < 48.0)
    {
        gains_ss << "\nIt's been " << beautify_double(hours_since_login) << " hours since last login.";
    }
    else
    {
        typedef std::chrono::duration<int, std::ratio<86400> > days;
        auto days_since_last_login = std::chrono::duration_cast<days>(hours_since_last_login);
        res_count_t days_since_login = days_since_last_login.count();
        gains_ss << "\nIt's been " << beautify_double(days_since_login) << " days since last login.";
    }

    CCLOG("%s", gains_ss.str().c_str());

    std::string result = "";
    if (at_capacity_ss.str() != "")
    {
        result = at_capacity_ss.str() + "\n---\n\n" + gains_ss.str();
    }
    else
    {
        result = gains_ss.str();
    }

    return result;

}

bool GameLogic::init()
{
    GameLogic* instance = GameLogic::getInstance();
    instance->beatup = Beatup::create();
    instance->beatup->setName("beatup");
    instance->beatup->retain();
    instance->buildup = instance->beatup->buildup;
    instance->equipment = std::unique_ptr<Equipment>(new Equipment());

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

double GameLogic::get_city_investment()
{
    return DataManager::get_double_from_data("city_investment");
};

void GameLogic::add_appeasements(double value)
{
    DataManager::incr_key("appeasements", value);
};

double GameLogic::get_appeasements()
{
    return DataManager::get_double_from_data("appeasements");
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

    auto item_serializer = ItemSerializer("alpha_items.json");
    item_serializer.serialize();

    auto equipment_serializer = EquipmentSerializer("alpha_equipment.json");
    equipment_serializer.serialize();

    auto ingredient_serializer = IngredientSerializer("alpha_ingredients.json");
    ingredient_serializer.serialize();

    //save coins
    DataManager::set_double_from_data(Beatup::total_coin_key, BEATUP->get_total_coins());

    //save last targeted building and tab
    DataManager::set_string_from_data("target_building", BUILDUP->get_target_building()->name);
    cocos2d::Scene* scene = cocos2d::Director::getInstance()->getRunningScene();
    HarvestScene* harvest_scene = dynamic_cast<HarvestScene*>(scene->getChildByName("HarvestScene"));
    auto sidebar = harvest_scene->sidebar;
    DataManager::set_int_from_data("active_tab", (int)sidebar->tabs.active_tab);

    //save vibration
    std::string vibrate_key = "can_vibrate";
    DataManager::set_bool_from_data(vibrate_key, GameLogic::getInstance()->get_can_vibrate());

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
    std::string vibrate_key = "can_vibrate";
    GameLogic::getInstance()->set_can_vibrate(DataManager::get_bool_from_data(vibrate_key, true));

    for (spBuilding building : BUILDUP->city->buildings)
    {
        auto bldg_serializer = BuildingSerializer("test_building.json", building);
        bldg_serializer.load();
    };

    auto item_serializer = ItemSerializer("alpha_items.json");
    item_serializer.load();

    auto ingredient_serializer = IngredientSerializer("alpha_ingredients.json");
    ingredient_serializer.load();

    auto equipment_serializer = EquipmentSerializer("alpha_equipment.json");
    equipment_serializer.load();

    auto coins_from_data = DataManager::get_double_from_data(Beatup::total_coin_key);
    if (coins_from_data < 0.0)
    {
        LOG(INFO) << "Coins from data was less than zero. Should not be possible.";
        coins_from_data = 0.0;
    }
    BEATUP->_total_coins = coins_from_data;
	// BEATUP->_total_coins = 500000000000000000000000000000000000000000000000000.0; //NOTE use to test Infinity in android xml double parsing

    BUILDUP->city->name = DataManager::get_string_from_data("city_name", "");

    //load last targeted building and tab
    std::string target_building_name = DataManager::get_string_from_data("target_building", "The Farm");
    spBuilding building = BUILDUP->city->building_by_name(target_building_name);
    BUILDUP->set_target_building(building);

    cocos2d::Scene* scene = cocos2d::Director::getInstance()->getRunningScene();
    HarvestScene* harvest_scene = dynamic_cast<HarvestScene*>(scene->getChildByName("HarvestScene"));
    auto sidebar = harvest_scene->sidebar;
    TabManager::TabTypes tab_type = (TabManager::TabTypes)DataManager::get_int_from_data("active_tab", (int)TabManager::TabTypes::ShopTab);
    sidebar->tabs.set_tab_active(tab_type, building);

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

void GameLogic::set_can_vibrate(bool can_vibrate)
{
    this->_can_vibrate = can_vibrate;
};

bool GameLogic::get_can_vibrate()
{
    return this->_can_vibrate;
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
    auto city_menu_scene_node = get_prebuilt_node_from_csb("editor/scenes/city_menu_scene.csb");

    city_menu_scene_node->removeFromParent();
    auto panel = city_menu_scene_node->getChildByName("panel");

    auto header = dynamic_cast<ui::Text*>(panel->getChildByName("title_lbl"));
    header->setTouchEnabled(true);
    header->addTouchEventListener([](Ref* ref, ui::Widget::TouchEventType type)
    {
        if (type == ui::Widget::TouchEventType::ENDED)
        {
            auto director = Director::getInstance();
            director->setDisplayStats(!director->isDisplayStats());
        }
    });

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

    //use outline from NuItems ttf_config
    ui::UICCTextField* renderer = dynamic_cast<ui::UICCTextField*>(city_name_input->getVirtualRenderer());
    cocos2d::Label* label = dynamic_cast<Label*>(renderer);
    label->setTTFConfig(NuItem::ttf_config);

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
            do_vibrate(32); //vibrate for two frames
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
    Label* invest_renderer = invest_button->getTitleRenderer();
    invest_renderer->setTTFConfig(ttf_config);

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
            do_vibrate(32); //vibrate for two frames
            mistIngredient& city_ingredients = BUILDUP->get_all_ingredients();
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
        mistIngredient& city_ingredients = BUILDUP->get_all_ingredients();
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
            do_vibrate(16);
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
            do_vibrate(16);
            GameDirector::switch_to_items_menu();
        }
    });
    load_default_button_textures(items_scene_btn);

    auto miner_scene_btn = dynamic_cast<ui::Button*>(panel->getChildByName("miner_scene_btn"));
    Label* miner_scene_lbl = miner_scene_btn->getTitleRenderer();
    miner_scene_lbl->setTextColor(Color4B::WHITE);
    miner_scene_lbl->enableOutline(Color4B::BLACK, 2);

    miner_scene_btn->addTouchEventListener([](Ref* touch, ui::Widget::TouchEventType type){
        if (type == ui::Widget::TouchEventType::ENDED)
        {
            do_vibrate(16);
            GameDirector::switch_to_miner_menu();
        }
    });
    load_default_button_textures(miner_scene_btn);

    auto item_equip_scene_btn = dynamic_cast<ui::Button*>(panel->getChildByName("item_equip_scene_btn"));
    Label* item_equip_btn_lbl = item_equip_scene_btn->getTitleRenderer();
    item_equip_btn_lbl->setTextColor(Color4B::WHITE);
    item_equip_btn_lbl->enableOutline(Color4B::BLACK, 2);

    item_equip_scene_btn->addTouchEventListener([](Ref* touch, ui::Widget::TouchEventType type){
        if (type == ui::Widget::TouchEventType::ENDED)
        {
            do_vibrate(16);
            GameDirector::switch_to_equipment_menu();
        }
    });
    load_default_button_textures(item_equip_scene_btn);

    auto director = cocos2d::Director::getInstance();
    director->pushScene(scene);
};

void GameDirector::switch_to_miner_menu()
{
    auto miner_scene = get_prebuilt_node_from_csb("editor/scenes/miner_scene.csb");
    miner_scene->removeFromParent();

    std::shared_ptr<Miner> miner = std::make_shared<Miner>(miner_scene);

    auto info_panel = dynamic_cast<ui::Layout*>(miner_scene->getChildByName("info_panel"));
    auto rails_count_lbl = dynamic_cast<ui::Text*>(info_panel->getChildByName("mine_rails_count"));
    auto cart_count_lbl = dynamic_cast<ui::Text*>(info_panel->getChildByName("mine_cart_count"));
    auto depth_display = dynamic_cast<ui::Text*>(info_panel->getChildByName("mine_depth_count"));

    auto tilemap_nav = miner_scene->getChildByName("tilemap_nav");
    for (cocos2d::Node* child : tilemap_nav->getChildren())
    {
        ui::Button* nav_button = dynamic_cast<ui::Button*>(child);
        if (nav_button)
        {
            load_default_button_textures(nav_button);
            cocos2d::TTFConfig ttf_config = TTFConfig("pixelmix.ttf", 24, GlyphCollection::ASCII, NULL, false, 2);
            Label* invest_renderer = nav_button->getTitleRenderer();
            invest_renderer->setTTFConfig(ttf_config);

            VoidFuncNoArgs move_active_func;
            std::string arrow_sprite_path;
            const std::string button_name = nav_button->getName();
            Vec2 tile_offset;
            if (button_name == "move_top_right")
            {
                move_active_func = CC_CALLBACK_0(Miner::move_active_top_right, miner);
                arrow_sprite_path = "upRight.png";
                tile_offset = {0.0f, -1.0f};
            }
            else if (button_name == "move_top_left")
            {
                move_active_func = CC_CALLBACK_0(Miner::move_active_top_left, miner);
                arrow_sprite_path = "upLeft.png";
                tile_offset = {-1.0f, 0.0f};
            }
            else if (button_name == "move_bot_left")
            {
                move_active_func = CC_CALLBACK_0(Miner::move_active_bottom_left, miner);
                arrow_sprite_path = "downLeft.png";
                tile_offset = {0.0f, 1.0f};
            }
            else if (button_name == "move_bot_right")
            {
                move_active_func = CC_CALLBACK_0(Miner::move_active_bottom_right, miner);
                arrow_sprite_path = "downRight.png";
                tile_offset = {1.0f, 0.0f};
            }

            Sprite* arrow_sprite = Sprite::createWithSpriteFrameName(arrow_sprite_path);
            auto arrow_pos = nav_button->getChildByName("arrow_sprite")->getPosition();
            arrow_sprite->setPosition(arrow_pos);
            arrow_sprite->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
            nav_button->addChild(arrow_sprite);
            nav_button->setTitleText(" "); //hide text

            nav_button->addTouchEventListener([miner, move_active_func, rails_count_lbl, info_panel](Ref* sender, ui::Widget::TouchEventType type)
            {
                if (type == ui::Widget::TouchEventType::ENDED)
                {
                    //if there's at least 1 rail, build a rail and then remove it
                    res_count_t num_rails = BUILDUP->count_ingredients(Ingredient::SubType::MineRails);
                    if (num_rails > 0)
                    {
                        move_active_func();
                        BUILDUP->remove_shared_ingredients_from_all(Ingredient::SubType::MineRails, 1);
                        MinerSerializer serializer = MinerSerializer("alpha_tilemap.json", miner.get());
                        serializer.serialize();
                    }
                    else
                    {
                        CCLOG("need more rails");
                        animate_flash_action(rails_count_lbl, 0.1f, 1.5f);
                        auto shake = FShake::actionWithDuration(0.1f, 2.5f, 2.5f);
                        info_panel->runAction(shake);
                    }
                };
            });

            auto update_nav_button = [nav_button, button_name, miner, tile_offset](float dt){
                if (!nav_button->isHighlighted()) //if its highlighted it means its being pressed
                {
                    if (miner->get_tile_is_blocked_pos(tile_offset + miner->active_tile_pos) == false){
                        nav_button->setBright(true);
                    }
                    else {
                        //dont actually disable the buttons, just set brightness so it
                        //can still be clicked
                        nav_button->setBright(false);
                    };
                }
            };
            nav_button->schedule(update_nav_button, AVERAGE_DELAY, "update_nav_button");

        } else {
            CCLOG("No nav button");
        };
    }

    auto update_rails_count_cb = [rails_count_lbl](float dt)
    {
        auto rails_count = BUILDUP->count_ingredients(Ingredient::SubType::MineRails);
        std::string pretty_count = beautify_double(rails_count);
        rails_count_lbl->setString(pretty_count);
    };
    update_rails_count_cb(0);
    rails_count_lbl->schedule(update_rails_count_cb, AVERAGE_DELAY, "update_rails_count_cb");

    auto update_cart_count_cb = [cart_count_lbl](float dt)
    {
        auto cart_count = BUILDUP->count_ingredients(Ingredient::SubType::Minecart);
        std::string pretty_count = beautify_double(cart_count);
        cart_count_lbl->setString(pretty_count);
    };
    update_cart_count_cb(0);
    cart_count_lbl->schedule(update_cart_count_cb, AVERAGE_DELAY, "update_cart_count_cb");

    auto update_depth_display_cb = [miner, depth_display](float dt)
    {
        auto cart_count = miner->depth;
        std::string pretty_count = beautify_double(cart_count);
        depth_display->setString(pretty_count);
    };
    update_depth_display_cb(0);
    depth_display->schedule(update_depth_display_cb, AVERAGE_DELAY, "update_depth_display_cb");

    info_panel->addTouchEventListener([miner](Ref* sender, ui::Widget::TouchEventType type){
            if (type == ui::Widget::TouchEventType::ENDED) {
                CCLOG("touched info");
                MinerSerializer serializer = MinerSerializer("alpha_tilemap.json", miner.get());
                serializer.load();
                CCLOG("done loading");
            };
    });

    auto scene = cocos2d::Scene::create();
    scene->setName("city_wrapper_scene");
    scene->addChild(miner_scene);

    //title
    //auto title_lbl = dynamic_cast<ui::Text*>(panel->getChildByName("title_lbl"));
    //set_aliasing(title_lbl);

    auto back_btn = dynamic_cast<ui::Button*>(miner_scene->getChildByName("back_btn"));
    Label* button_lbl = back_btn->getTitleRenderer();
    button_lbl->setTextColor(Color4B::WHITE);
    button_lbl->enableOutline(Color4B::BLACK, 2);

    back_btn->addTouchEventListener([miner](Ref* touch, ui::Widget::TouchEventType type){
        if (type == ui::Widget::TouchEventType::ENDED)
        {
            MinerSerializer serializer = MinerSerializer("alpha_tilemap.json", miner.get());
            serializer.serialize();
            auto director = Director::getInstance();
            director->popScene();
            do_vibrate(16);
        }
    });
    load_default_button_textures(back_btn);

    auto explode_btn = dynamic_cast<ui::Button*>(miner_scene->getChildByName("explode_btn"));
    Label* explode_button_lbl = explode_btn->getTitleRenderer();
    explode_button_lbl->setTextColor(Color4B::WHITE);
    explode_button_lbl->enableOutline(Color4B::BLACK, 2);

	auto button_highlight_particle = MagicEmitter::create("Square");
	button_highlight_particle->setPosition(explode_btn->getPosition());
	MP_Emitter_Cocos* emitter = button_highlight_particle->GetEmitter();
	emitter->SetScale(0.5f);
	miner_scene->addChild(button_highlight_particle);

    explode_btn->addTouchEventListener([miner](Ref* touch, ui::Widget::TouchEventType type){
        if (type == ui::Widget::TouchEventType::ENDED)
        {
            MinerSerializer serializer = MinerSerializer("alpha_tilemap.json", miner.get());
            serializer.serialize();

            GameDirector::switch_to_item_altar_menu();
            miner->reset(); //TODO reenable this after testing
            do_vibrate(16);
        }
    });
    load_default_button_textures(explode_btn);

	auto check_tiles_cb = [explode_btn, miner, emitter](float dt){
		bool rail_connected = miner->rails_connect_a_resource();
		if (rail_connected)
		{
			explode_btn->setEnabled(true);
			emitter->SetState(MAGIC_STATE_UPDATE);
		}
		else
		{

			emitter->SetState(MAGIC_STATE_STOP);
			explode_btn->setEnabled(false);
		}
	};
    check_tiles_cb(0);
    explode_btn->schedule(check_tiles_cb, AVERAGE_DELAY, "check_tiles_cb");

    auto dig_btn = dynamic_cast<ui::Button*>(miner_scene->getChildByName("dig_btn"));
    Label* dig_button_lbl = dig_btn->getTitleRenderer();
    dig_button_lbl->setTextColor(Color4B::WHITE);
    dig_button_lbl->enableOutline(Color4B::BLACK, 2);

    dig_btn->addTouchEventListener([miner, cart_count_lbl, info_panel](Ref* touch, ui::Widget::TouchEventType type){
        if (type == ui::Widget::TouchEventType::ENDED)
        {
            res_count_t num_carts = BUILDUP->count_ingredients(Ingredient::SubType::Minecart);
            if (num_carts > 0)
            {
                BUILDUP->remove_shared_ingredients_from_all(Ingredient::SubType::Minecart, 1);
                miner->reset();
                do_vibrate(16);

                MinerSerializer serializer = MinerSerializer("alpha_tilemap.json", miner.get());
                serializer.serialize();
            }
            else
            {
                CCLOG("need more carts");
                animate_flash_action(cart_count_lbl, 0.1f, 1.5f);
                auto shake = FShake::actionWithDuration(0.1f, 2.5f, 2.5f);
                info_panel->runAction(shake);
            }
        }
    });
    load_default_button_textures(dig_btn);

    auto check_carts_cb = [dig_btn, cart_count_lbl, miner](float dt){
        res_count_t num_carts = BUILDUP->count_ingredients(Ingredient::SubType::Minecart);
        dig_btn->setBright(num_carts > 0); //setBright because we want to trigger the label animation
    };
    check_carts_cb(0);
    dig_btn->schedule(check_carts_cb, AVERAGE_DELAY, "check_carts_cb");


    auto director = cocos2d::Director::getInstance();
    director->pushScene(scene);
};

void GameDirector::switch_to_items_menu()
{

	auto scene = ItemScene::createScene();
	scene->setName("item_scene");

    auto director = cocos2d::Director::getInstance();
    director->pushScene(scene);
};

void GameDirector::switch_to_item_altar_menu()
{

    auto scene = AltarItemScene::createScene();
    scene->setName("altar_item_scene");


    auto director = cocos2d::Director::getInstance();
    director->pushScene(scene);
};

void GameDirector::switch_to_item_equip_menu(ItemSlotType slot_type)
{

    auto scene = EquipItemScene::createScene(slot_type);
    scene->setName("equip_item_scene");

    auto director = cocos2d::Director::getInstance();
    director->pushScene(scene);
};

void GameDirector::switch_to_equipment_menu()
{
    auto scene = cocos2d::Scene::create();
    scene->setName("equipment_scene");

    auto equipment_scene = get_prebuilt_node_from_csb("editor/scenes/equipment_scene.csb");
    auto panel = dynamic_cast<cocos2d::ui::Layout*>(equipment_scene->getChildByName("panel"));
    panel->removeFromParent();
    scene->addChild(panel);

	/// combat slot
    auto weapon_panel = panel->getChildByName("combat_panel");
    auto mining_panel = panel->getChildByName("mining_panel");
    auto recipe_panel = panel->getChildByName("recipe_panel");

	ui::Text* weapon_label = dynamic_cast<ui::Text*>(weapon_panel->getChildByName("name_lbl"));
	auto update_equipped_weapon = [weapon_label](float dt){
		if (GameLogic::getInstance()->equipment->combat_slot->has_item())
		{
			spItem item = GameLogic::getInstance()->equipment->combat_slot->get_item();
			weapon_label->setString(item->get_name());
		}
		else
		{
			weapon_label->setString("Equip a weapon");
		};
	};
    update_equipped_weapon(0);
    weapon_label->schedule(update_equipped_weapon, SHORT_DELAY, "update_equipped_weapon");

    ui::Button* equip_weapon_btn = dynamic_cast<ui::Button*>(weapon_panel->getChildByName("item_btn"));
    prep_button(equip_weapon_btn);
    equip_weapon_btn->addTouchEventListener([](Ref* sender, ui::Widget::TouchEventType type){
        if (type == ui::Widget::TouchEventType::ENDED)
        {
            GameDirector::switch_to_item_equip_menu(ItemSlotType::Combat);
        }
    });

	/// mining
	ui::Text* mining_label = dynamic_cast<ui::Text*>(mining_panel->getChildByName("name_lbl"));
	auto update_equipped_mining = [mining_label](float dt){
		if (GameLogic::getInstance()->equipment->mining_slot->has_item())
		{
			spItem item = GameLogic::getInstance()->equipment->mining_slot->get_item();
			mining_label->setString(item->get_name());
		}
		else
		{
			mining_label->setString("Equip a mining tool");
		};
	};
    update_equipped_mining(0);
    mining_label->schedule(update_equipped_mining, SHORT_DELAY, "update_equipped_mining");


    ui::Button* equip_mining_btn = dynamic_cast<ui::Button*>(mining_panel->getChildByName("item_btn"));
    prep_button(equip_mining_btn);
    equip_mining_btn->addTouchEventListener([](Ref* sender, ui::Widget::TouchEventType type){
        if (type == ui::Widget::TouchEventType::ENDED)
        {
            GameDirector::switch_to_item_equip_menu(ItemSlotType::Mining);
        }
    });

	///recipe
	ui::Text* recipe_label = dynamic_cast<ui::Text*>(recipe_panel->getChildByName("name_lbl"));
	auto update_equipped_recipe = [recipe_label](float dt){
		if (GameLogic::getInstance()->equipment->recipe_slot->has_item())
		{
			spItem item = GameLogic::getInstance()->equipment->recipe_slot->get_item();
			recipe_label->setString(item->get_name());
		}
		else
		{
			recipe_label->setString("Equip a crafting tool");
		};
	};
    update_equipped_recipe(0);
    recipe_label->schedule(update_equipped_recipe, SHORT_DELAY, "update_equipped_recipe");

    ui::Button* equip_recipe_btn = dynamic_cast<ui::Button*>(recipe_panel->getChildByName("item_btn"));
    prep_button(equip_recipe_btn);
    equip_recipe_btn->addTouchEventListener([](Ref* sender, ui::Widget::TouchEventType type){
        if (type == ui::Widget::TouchEventType::ENDED)
        {
            GameDirector::switch_to_item_equip_menu(ItemSlotType::Recipe);
        }
    });

    auto back_btn = dynamic_cast<ui::Button*>(panel->getChildByName("back_btn"));
    prep_back_button(back_btn);

    auto director = cocos2d::Director::getInstance();
    director->pushScene(scene);
};
