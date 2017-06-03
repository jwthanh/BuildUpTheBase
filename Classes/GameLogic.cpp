#include "GameLogic.h"

#include "base/CCDirector.h"
#include "base/CCEventListenerKeyboard.h"
#include "base/CCEventDispatcher.h"
#include "2d/CCSprite.h"

#include "ui/UILayout.h"
#include "ui/UIText.h"
#include "ui/UIButton.h"
#include "ui/UIPageView.h"
#include "ui/UITextField.h"
#include "ui/UIImageView.h"
#include "2d/CCTransition.h"

#include "external/easylogging.h"

#include "magic_particles/MagicEmitter.h"
#include "magic_particles/mp_cocos.h"

#include "NodeBuilder.h"
#include "Beatup.h"
#include "HouseBuilding.h"
#include "Buildup.h"
#include "DataManager.h"
#include "NuMenu.h"
#include "Serializer.h"
#include "Util.h"
#include "MiscUI.h"
#include "Modal.h"
#include "PopupPanel.h"
#include "HarvestScene.h"
#include "Item.h"
#include "BuildingsSideBar.h"
#include "Miner.h"
#include "FShake.h"
#include "ItemScene.h"
#include "Equipment.h"
#include "Tutorial.h"
#include "utilities/vibration.h"
#include "goals/Achievement.h"
#include "BuildingDetailScene.h"
#include "banking/Bank.h"
#include "SoundEngine.h"
#include "base/CCEventListenerTouch.h"


USING_NS_CC;

GameLogic* GameLogic::_instance = NULL;

GameLogic::GameLogic()
    : is_loaded(false), _can_vibrate(true),
        beatup(nullptr), buildup(nullptr),
        _city_investment(0), appeasements(0),
        has_learned_wallet_size(false)
{

};

std::string new_player_load()
{

    std::stringstream ss;
    ss << "Tap the farm in the middle of the screen to harvest, then sell by tapping the grain icon along the bottom." << std::endl << std::endl;
    ss << "Don't forget to enter a customized username so you can compete on the leaderboards at http://buildupthebase.com" << std::endl << std::endl;

    return ss.str();

}

std::string existing_player_load()
{
    std::stringstream gains_ss, at_capacity_ss;
    std::chrono::duration<double, std::ratio<3600>> hours_since_last_login = BEATUP->hours_since_last_login();

    auto original_ingredients = BUILDUP->get_all_ingredients();

    auto seconds = std::chrono::duration_cast<std::chrono::seconds>(hours_since_last_login);
    long long seconds_count = seconds.count();


    //if the time is over a day in the passed, assume that they're cheating
    // anything less than 24 hours could be a timezone thing
    bool is_cheating = hours_since_last_login.count() < -24.0;

    if (is_cheating == false)
    {
        //TODO maybe simulate it for up to an hour, and multiply that output by
        //the hours remaining. Alternatively, rewrite it somehow so that it
        //accurately estimates it from nothing.
        BUILDUP->city->update(seconds_count);

        auto refreshed_ingredients = BUILDUP->get_all_ingredients();
        for (auto mist_ing : refreshed_ingredients)
        {
            Ingredient::SubType ing_type = mist_ing.first;
            res_count_t new_count = mist_ing.second;

            res_count_t _def = 0;
            res_count_t old_count = map_get(original_ingredients, ing_type, _def);

            if (new_count - old_count > 0.0)
            {
                res_count_t gained = new_count - old_count;
                gains_ss << "+Gained " << beautify_double(gained) << " " << Ingredient::type_to_string(ing_type);

                //TODO reimplement a per-building storage full message (since its all shared now)

                gains_ss << std::endl;
            }
        }
    }
    else //cheating detected
    {
        gains_ss << "Awfully suspicious..." << std::endl;
        LOG(WARNING) << "time jump cheat detected";

        BEATUP->_total_coins = 0;

        GameLogic::load_all_as_cheater();
    }


    std::stringstream since_last_login_ss;
    res_count_t hours_since_login = hours_since_last_login.count();
    if (hours_since_login < 1.0)
    {
        auto minutes_since_last_login = std::chrono::duration_cast<std::chrono::minutes>(hours_since_last_login);
        res_count_t minutes_since_login = minutes_since_last_login.count();
        since_last_login_ss << "\nIt's been " << beautify_double(minutes_since_login) << " minutes since last login.";
    }
    else if (hours_since_login < 48.0)
    {
        since_last_login_ss << "\nIt's been " << beautify_double(hours_since_login) << " hours since last login.";
    }
    else
    {
        typedef std::chrono::duration<int, std::ratio<86400> > days;
        auto days_since_last_login = std::chrono::duration_cast<days>(hours_since_last_login);
        res_count_t days_since_login = days_since_last_login.count();
        since_last_login_ss << "\nIt's been " << beautify_double(days_since_login) << " days since last login.";
    }

    gains_ss << since_last_login_ss.str();

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



void build_welcome_modal()
{
    auto scene = cocos2d::Director::getInstance()->getRunningScene()->getChildByName("HarvestScene");
    TextBlobModal* modal = TextBlobModal::create();
    scene->addChild(modal);

    std::string modal_content;
    time_t from_file = static_cast<time_t>(DataManager::get_int_from_data(Beatup::last_login_key));
    if (from_file == 0)
    {
        modal->set_title("Welcome to Build Up The Base!");
        modal_content = new_player_load();
    }
    else
    {
        modal->set_title("Welcome Back!");
        modal_content = existing_player_load();
    }

    modal->set_body(modal_content);
};

void GameLogic::load_game()
{

    //dont re-load if done once already. this is because load_game gets called
    //onEnter, originally only from initializing game, but now we're using
    //different scenes
    if (is_loaded == true)
    {
        return;
    };

    //TODO save/load whether its been completed or not
    // Tutorial* tutorial = Tutorial::getInstance();
    // auto scene = cocos2d::Director::getInstance()->getRunningScene()->getChildByName("HarvestScene");
    // tutorial->first_start(scene);

    //cheat to reset data to blank on load
    auto username = DataManager::get_string_from_data("username");
    if (username == "__resetdata")
    {
        DataManager::set_string_from_data("username", "");
        return;
    };

    std::string tutorial_key = "tutorial_v1_complete";
    bool tutorial_complete = DataManager::get_bool_from_data(tutorial_key);

    //add popup to message to fill out username
    if (username == "" && tutorial_complete) {
        auto popup = GameLogic::get_popup_panel();
        popup->set_string("Set a username, visible along the right side of the screen, and check out how you stack up on the leaderboard.");
        popup->animate_open();
    };

    CCLOG("loading game on startup, this should only happen once");
    GameLogic::load_all();


    bool show_welcome_modal = tutorial_complete == true;
    // bool show_welcome_modal = false;
    if (show_welcome_modal)
    {
        build_welcome_modal();
        auto scene = cocos2d::Director::getInstance()->getRunningScene()->getChildByName("HarvestScene");
        auto tutorial_sidebar_panel = scene->getChildByName("tutorial_sidebar_panel");
        tutorial_sidebar_panel->setVisible(false);
    }

    //set the last login time, set here and on save
    BEATUP->set_last_login();
    LOG(INFO) << "Game loaded!";

    this->is_loaded = true;

    if (tutorial_complete == false)
    {
        Tutorial* tutorial = Tutorial::getInstance();
        auto scene = cocos2d::Director::getInstance()->getRunningScene()->getChildByName("HarvestScene");
        tutorial->first_start(scene);
        auto tutorial_sidebar_panel = scene->getChildByName("tutorial_sidebar_panel");
        tutorial_sidebar_panel->setVisible(true);
    };

}

bool GameLogic::init()
{
    GameLogic* instance = GameLogic::getInstance();
    instance->beatup = Beatup::create();
    instance->beatup->setName("beatup");
    instance->beatup->retain();
    instance->buildup = instance->beatup->buildup;
    instance->equipment = std::unique_ptr<Equipment>(new Equipment());

    instance->_total_kills = 0.0; //set in serializer

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
            set_aliasing(player_gold_per_sec_lbl); //TODO move this somewhere where it's not called all the time

            const auto& total_coins = BEATUP->get_total_coins();
            res_count_t coin_diff = total_coins - BEATUP->_last_total_coins;
            std::string prefix = "+";
            if (coin_diff < 0.0) {
                prefix = "";
            }

            res_count_t max_storage = BEATUP->get_max_coin_storage();
            if (total_coins == max_storage ) {
                player_gold_per_sec_lbl->setString("Max coins stored");

                if (!this->has_learned_wallet_size) {
                    this->has_learned_wallet_size = true;
                    TextBlobModal* modal = TextBlobModal::create();
                    scene->addChild(modal);
                    modal->set_title("Upgrade coin storage!");
                    modal->set_body("You've hit the coin limit!\n\nScrap some weapons at The Marketplace, contributing to city investment. Then use that to upgrade the coin storage.\n\nAlternatively, deposit coins 10 at a time in the City Management screen.");
                }

            } else {
                std::string beautified_diff = prefix+beautify_double(coin_diff)+"/sec";
                player_gold_per_sec_lbl->setString(beautified_diff);
            };

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
        return NULL;
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

void GameLogic::add_city_investment(res_count_t value)
{
    auto achievement_manager = AchievementManager::getInstance();
    std::shared_ptr<BaseAchievement> achievement = achievement_manager->getAchievement(AchievementType::TotalCityInvestments);
    achievement->increment_by_n((res_count_t)value);
    this->_city_investment += value;
};

res_count_t GameLogic::get_city_investment()
{
    //NOTE returns actual double though
    return this->_city_investment;
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
    if (value > 0) {
        auto achievement_manager = AchievementManager::getInstance();
        std::shared_ptr<BaseAchievement> achievement = achievement_manager->getAchievement(AchievementType::TotalKills);
        achievement->increment_by_n((res_count_t)value);
    };

    this->_total_kills += value;
};

int GameLogic::get_total_kills()
{
    return this->_total_kills;
};

void GameLogic::save_all()
{
    LOG(INFO) << "Starting save";
    auto bldg_serializer = BuildingSerializer("test_building.json");
    for (spBuilding building : BUILDUP->city->buildings)
    {
        bldg_serializer.serialize(building);
    };

    auto item_serializer = ItemSerializer("alpha_items.json");
    item_serializer.serialize();

    auto equipment_serializer = EquipmentSerializer("alpha_equipment.json");
    equipment_serializer.serialize();

    auto ingredient_serializer = IngredientSerializer("alpha_ingredients.json");
    ingredient_serializer.serialize();

    auto achievement_serializer = AchievementSerializer("alpha_achievements.json");
    achievement_serializer.serialize();

    auto harvestable_manager_serializer = HarvestableManagerSerializer("alpha_harvestable_manager.json");
    harvestable_manager_serializer.serialize();

    auto bank_serializer = BankSerializer("alpha_bank.json");
    bank_serializer.serialize();

    //save coins
    DataManager::set_double_from_data(Beatup::total_coin_key, BEATUP->get_total_coins());
    DataManager::set_double_from_data("city_investment", GameLogic::getInstance()->get_city_investment());
    DataManager::set_double_from_data("total_kills", GameLogic::getInstance()->get_total_kills());

    //save last targeted building and tab
    DataManager::set_string_from_data("target_building", BUILDUP->get_target_building()->name);
    cocos2d::Scene* scene = cocos2d::Director::getInstance()->getRunningScene();
    HarvestScene* harvest_scene = dynamic_cast<HarvestScene*>(scene->getChildByName("HarvestScene"));
    auto sidebar = harvest_scene->sidebar;
    DataManager::set_int_from_data("active_tab", (int)sidebar->tabs.active_tab);

    //save vibration
    std::string vibrate_key = "can_vibrate";
    DataManager::set_bool_from_data(vibrate_key, GameLogic::getInstance()->get_can_vibrate());

    //been taught about wallet size
    DataManager::set_bool_from_data("has_learned_wallet_size", GameLogic::getInstance()->has_learned_wallet_size);

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

    auto bldg_serializer = BuildingSerializer("test_building.json");
    for (spBuilding building : BUILDUP->city->buildings)
    {
        bldg_serializer.load(building);
    };

    auto item_serializer = ItemSerializer("alpha_items.json");
    item_serializer.load();

    auto ingredient_serializer = IngredientSerializer("alpha_ingredients.json");
    ingredient_serializer.load();

    auto equipment_serializer = EquipmentSerializer("alpha_equipment.json");
    equipment_serializer.load();

    auto achievement_serializer = AchievementSerializer("alpha_achievements.json");
    achievement_serializer.load();

    auto harvestable_manager_serializer = HarvestableManagerSerializer("alpha_harvestable_manager.json");
    harvestable_manager_serializer.load();

    auto bank_serializer = BankSerializer("alpha_bank.json");
    bank_serializer.load();

    auto coins_from_data = DataManager::get_double_from_data(Beatup::total_coin_key);
    if (coins_from_data < 0.0)
    {
        LOG(INFO) << "Coins from data was less than zero. Should not be possible.";
        coins_from_data = 0.0;
    }
    BEATUP->_total_coins = coins_from_data;
    GameLogic::getInstance()->_city_investment = DataManager::get_double_from_data("city_investment");
    GameLogic::getInstance()->_total_kills = DataManager::get_double_from_data("total_kills");

    BUILDUP->city->name = DataManager::get_string_from_data("city_name", "");

    //load last targeted building and tab
    std::string target_building_name = DataManager::get_string_from_data("target_building", "The Farm");
    spBuilding building = BUILDUP->city->building_by_name(target_building_name);
    BUILDUP->set_target_building(building);

    cocos2d::Scene* scene = cocos2d::Director::getInstance()->getRunningScene();
    HarvestScene* harvest_scene = dynamic_cast<HarvestScene*>(scene->getChildByName("HarvestScene"));
    auto sidebar = harvest_scene->sidebar;
    TabManager::TabTypes tab_type = (TabManager::TabTypes)DataManager::get_int_from_data("active_tab", (int)TabManager::TabTypes::ShopTab);

    //TODO make add a toggle_buttons based on TabType to avoid this set of if statments
    if (tab_type == TabManager::TabTypes::ShopTab)
        sidebar->toggle_buttons(sidebar->tab_shop_btn, ui::Widget::TouchEventType::ENDED);
    else if (tab_type == TabManager::TabTypes::BuildingTab)
        sidebar->toggle_buttons(sidebar->tab_building_btn, ui::Widget::TouchEventType::ENDED);
    else if (tab_type == TabManager::TabTypes::DetailTab)
        sidebar->toggle_buttons(sidebar->tab_detail_btn, ui::Widget::TouchEventType::ENDED);
    else if (tab_type == TabManager::TabTypes::PowersTab)
        sidebar->toggle_buttons(sidebar->tab_powers_btn, ui::Widget::TouchEventType::ENDED);

    GameLogic::getInstance()->has_learned_wallet_size = DataManager::get_bool_from_data("has_learned_wallet_size", false);

};

void GameLogic::load_all_as_cheater()
{
    auto bldg_serializer = BuildingSerializer("cheat_building.json");
    for (spBuilding building : BUILDUP->city->buildings)
    {
        bldg_serializer.load(building);
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

void set_default_key_handler(Scene* scene)
{
    auto go_back_func = [](EventKeyboard::KeyCode keyCode, Event* event){
        if (keyCode == EventKeyboard::KeyCode::KEY_BACK || keyCode == EventKeyboard::KeyCode::KEY_ESCAPE)
        {
            do_vibrate(5);
            auto director = Director::getInstance();
            director->popScene();
            event->stopPropagation();
        }
    };
    auto pKeybackListener = cocos2d::EventListenerKeyboard::create();
    pKeybackListener->onKeyReleased = go_back_func;
    scene->getEventDispatcher()->addEventListenerWithSceneGraphPriority(pKeybackListener, scene);

};

void GameDirector::switch_to_city_menu()
{
    auto city_menu_scene_node = get_prebuilt_node_from_csb("editor/scenes/city_menu_scene.csb");

    city_menu_scene_node->removeFromParent();
    auto panel = city_menu_scene_node->getChildByName("panel");

    auto header = dynamic_cast<ui::Text*>(panel->getChildByName("title_lbl"));
    header->setTouchEnabled(true);
    auto toggle_display_stats = [](){
            auto director = Director::getInstance();
            director->setDisplayStats(!director->isDisplayStats());
    };
    bind_touch_ended(header, toggle_display_stats);

    auto scene = cocos2d::Scene::create();
    scene->setName("city_wrapper_scene");
    scene->addChild(city_menu_scene_node);

    set_default_key_handler(scene);

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
            std::string cleaned_city_name = clean_city_name(text);

            city_name_input->setString(cleaned_city_name);
            BUILDUP->city->name = cleaned_city_name;

            DataManager::set_string_from_data("city_name", cleaned_city_name);
        }
    };
    city_name_input->addEventListener(textfield_listener);

    //invest panel
    auto invest_panel = dynamic_cast<ui::Layout*>(panel->getChildByName("invest_panel"));
    auto invest_button = dynamic_cast<ui::Button*>(invest_panel->getChildByName("invest_button"));
    bind_touch_ended(invest_button, [](){ GameDirector::switch_to_scrap_item_menu(); });
    load_default_button_textures(invest_button);

    cocos2d::TTFConfig ttf_config = TTFConfig("pixelmix.ttf", 24, GlyphCollection::ASCII, NULL, false, 2);
    Label* invest_renderer = invest_button->getTitleRenderer();
    invest_renderer->setTTFConfig(ttf_config);

    auto invested_lbl = dynamic_cast<ui::Text*>(invest_panel->getChildByName("invested_lbl"));
    auto update_invested = [invested_lbl](float dt) {
        res_count_t invested = GameLogic::getInstance()->get_city_investment();
        invested_lbl->setString("CI "+beautify_double(invested));
    };
    invested_lbl->schedule(update_invested, AVERAGE_DELAY, "update_invested");
    update_invested(0);

    //appeasement panel
    auto appeasement_panel = dynamic_cast<ui::Layout*>(panel->getChildByName("appeasement_panel"));
    auto appeasement_button = dynamic_cast<ui::Button*>(appeasement_panel->getChildByName("appeasement_button"));
    auto add_appeasements_cb = []()
    {
        mistIngredient& city_ingredients = BUILDUP->get_all_ingredients();
        res_count_t _def = 0;
        IngredientSubType ing_type = IngredientSubType::Soul;
        res_count_t appease_count = 1.0;
        if (map_get(city_ingredients, ing_type, _def) >= appease_count)
        {
            GameLogic::getInstance()->add_appeasements(appease_count);
            BUILDUP->remove_shared_ingredients_from_all(ing_type, appease_count);
        }
    };
    bind_touch_ended(appeasement_button, add_appeasements_cb);
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
        appeased_lbl->setString("Souls "+beautify_double(appeasemented));
    };
    appeased_lbl->schedule(update_appeasemented, AVERAGE_DELAY, "update_appeasemented");
    update_appeasemented(0);


    auto back_btn = dynamic_cast<ui::Button*>(panel->getChildByName("back_btn"));
    prep_back_button(back_btn);

    auto items_scene_btn = dynamic_cast<ui::Button*>(panel->getChildByName("items_scene_btn"));
    prep_button(items_scene_btn);
    bind_touch_ended(items_scene_btn, [](){GameDirector::switch_to_items_menu();});

    auto miner_scene_btn = dynamic_cast<ui::Button*>(panel->getChildByName("miner_scene_btn"));
    prep_button(miner_scene_btn);
    bind_touch_ended(miner_scene_btn, [](){GameDirector::switch_to_miner_menu();});

    auto item_equip_scene_btn = dynamic_cast<ui::Button*>(panel->getChildByName("item_equip_scene_btn"));
    prep_button(item_equip_scene_btn);
    bind_touch_ended(item_equip_scene_btn, [](){GameDirector::switch_to_equipment_menu();});

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

            auto lay_rails = [miner, move_active_func, rails_count_lbl, info_panel]()
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
                    animate_flash_action(rails_count_lbl, 0.1f, 1.5f);
                    auto shake = FShake::actionWithDuration(0.1f, 2.5f, 2.5f);
                    info_panel->runAction(shake);
                }
            };
            bind_touch_ended(nav_button, lay_rails);

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

    auto scene = cocos2d::Scene::create();
    scene->setName("city_wrapper_scene");
    scene->addChild(miner_scene);

    set_default_key_handler(scene);

    auto back_btn = dynamic_cast<ui::Button*>(miner_scene->getChildByName("back_btn"));
    prep_button(back_btn); //dont prep_back_button because custom callback

    auto save_and_pop = [miner](){
        MinerSerializer serializer = MinerSerializer("alpha_tilemap.json", miner.get());
        serializer.serialize();

        auto director = Director::getInstance();
        director->popScene();
    };
    bind_touch_ended(back_btn, save_and_pop);


    //altar button
    auto explode_btn = dynamic_cast<ui::Button*>(miner_scene->getChildByName("explode_btn"));
    prep_button(explode_btn);

	auto altar_particle = MagicEmitter::create("Square");
	altar_particle->setPosition(explode_btn->getPosition());
	MP_Emitter_Cocos* altar_btn_emitter = altar_particle->GetEmitter();
	altar_btn_emitter->SetScale(0.5f);
	miner_scene->addChild(altar_particle);

    auto open_altar = [miner](){
        MinerSerializer serializer = MinerSerializer("alpha_tilemap.json", miner.get());
        serializer.serialize();

        GameDirector::switch_to_item_altar_menu();
        miner->reset();
    };
    bind_touch_ended(explode_btn, open_altar);

	auto check_altar_touching_cb = [explode_btn, miner, altar_btn_emitter](float dt){
		bool rail_connected = miner->rails_connect_a_resource(miner->altar_tile_pos);
		if (rail_connected)
		{
			explode_btn->setEnabled(true);
			altar_btn_emitter->SetState(MAGIC_STATE_UPDATE);
		}
		else
		{
			altar_btn_emitter->SetState(MAGIC_STATE_STOP);
			explode_btn->setEnabled(false);
		}
	};
    check_altar_touching_cb(0);
    explode_btn->schedule(check_altar_touching_cb, AVERAGE_DELAY, "check_altar_touching_cb");

    //chance button
    auto chance_btn = dynamic_cast<ui::Button*>(miner_scene->getChildByName("chance_btn"));
    prep_button(chance_btn);

	auto chance_particle = MagicEmitter::create("Square");
	chance_particle->setPosition(chance_btn->getPosition());
	MP_Emitter_Cocos* chance_btn_emitter = chance_particle->GetEmitter();
	chance_btn_emitter->SetScale(0.5f);
	miner_scene->addChild(chance_particle);

    auto open_chance = [miner](){
        MinerSerializer serializer = MinerSerializer("alpha_tilemap.json", miner.get());
        serializer.serialize();

        GameDirector::switch_to_item_chance_menu();
        miner->reset();
    };
    bind_touch_ended(chance_btn, open_chance);

	auto check_chance_touching_cb = [chance_btn, miner, chance_btn_emitter](float dt){
		bool rail_connected = miner->rails_connect_a_resource(miner->chance_tile_pos);
		if (rail_connected)
		{
			chance_btn->setEnabled(true);
			chance_btn_emitter->SetState(MAGIC_STATE_UPDATE);
		}
		else
		{
			chance_btn_emitter->SetState(MAGIC_STATE_STOP);
			chance_btn->setEnabled(false);
		}
	};
    check_chance_touching_cb(0);
    chance_btn->schedule(check_chance_touching_cb, AVERAGE_DELAY, "check_chance_touching_cb");

    //dig button
    auto dig_btn = dynamic_cast<ui::Button*>(miner_scene->getChildByName("dig_btn"));
    prep_button(dig_btn);

    auto go_deeper = [miner, cart_count_lbl, info_panel](){
            res_count_t num_carts = BUILDUP->count_ingredients(Ingredient::SubType::Minecart);
            if (num_carts > 0)
            {
                BUILDUP->remove_shared_ingredients_from_all(Ingredient::SubType::Minecart, 1);
                miner->reset();
                do_vibrate(10);

                MinerSerializer serializer = MinerSerializer("alpha_tilemap.json", miner.get());
                serializer.serialize();
            }
            else
            {
                animate_flash_action(cart_count_lbl, 0.1f, 1.5f);
                auto shake = FShake::actionWithDuration(0.1f, 2.5f, 2.5f);
                info_panel->runAction(shake);
            }
    };
    bind_touch_ended(dig_btn, go_deeper);

    auto check_carts_cb = [dig_btn, cart_count_lbl, miner](float dt){
        res_count_t num_carts = BUILDUP->count_ingredients(Ingredient::SubType::Minecart);
        dig_btn->setBright(num_carts > 0); //setBright because we want to trigger the label animation
    };
    check_carts_cb(0);
    dig_btn->schedule(check_carts_cb, AVERAGE_DELAY, "check_carts_cb");


    auto director = cocos2d::Director::getInstance();
    director->pushScene(scene);
};

void GameDirector::switch_to_building_detail_menu()
{
    auto target_building = BUILDUP->get_target_building();

	auto scene = BuildingDetailScene::createScene(target_building);
	scene->setName("building_detail_scene");
    set_default_key_handler(scene);

    auto director = cocos2d::Director::getInstance();
    director->pushScene(scene);
};

void GameDirector::switch_to_items_menu()
{
	auto scene = ItemScene::createScene();
	scene->setName("item_scene");
    set_default_key_handler(scene);

    auto director = cocos2d::Director::getInstance();
    director->pushScene(scene);
};

void GameDirector::switch_to_item_altar_menu()
{

    auto scene = AltarItemScene::createScene();
    scene->setName("altar_item_scene");
    set_default_key_handler(scene);

    auto director = cocos2d::Director::getInstance();
    director->pushScene(scene);
}

void GameDirector::switch_to_item_chance_menu()
{
    auto scene = ChanceItemScene::createScene();
    scene->setName("chance_item_scene");
    set_default_key_handler(scene);

    auto director = cocos2d::Director::getInstance();
    director->pushScene(scene);
};

void GameDirector::switch_to_item_equip_menu(ItemSlotType slot_type)
{

    auto scene = EquipItemScene::createScene(slot_type);
    scene->setName("equip_item_scene");
    set_default_key_handler(scene);

    auto director = cocos2d::Director::getInstance();
    director->pushScene(scene);
};

void GameDirector::switch_to_scrap_item_menu()
{
    auto scene = ScrapItemScene::createScene();
    scene->setName("scrap_item_scene");
    set_default_key_handler(scene);

    auto director = cocos2d::Director::getInstance();
    director->pushScene(scene);
};

void GameDirector::switch_to_equipment_menu()
{
    auto scene = cocos2d::Scene::create();
    scene->setName("equipment_scene");
    set_default_key_handler(scene);

    auto equipment_scene = get_prebuilt_node_from_csb("editor/scenes/equipment_scene.csb");
    auto panel = dynamic_cast<cocos2d::ui::Layout*>(equipment_scene->getChildByName("panel"));
    panel->removeFromParent();
    scene->addChild(panel);

    //builds the callback for the Equip Combat/Crafting/Mining button
    auto build_update_equipped_item = [](
            cocos2d::Node* panel,
            EquipmentSlot* slot, std::string default_message
        ){

        ui::Text* name_lbl = dynamic_cast<ui::Text*>(panel->getChildByName("name_lbl"));
        ui::ImageView* item_img = dynamic_cast<ui::ImageView*>(panel->getChildByName("item_img"));
        image_view_scale9_hack(item_img);

        std::function<void(float)> update_equipped_item = [name_lbl, item_img, slot, default_message](float dt){
            if (slot->has_item()) {
                spItem item = slot->get_item();
                name_lbl->setString(item->get_name());
                item_img->setVisible(true);
                item_img->loadTexture(item->img_path);
                set_aliasing(item_img);
            } else {
                name_lbl->setString(default_message);
                item_img->setVisible(false);
            }
        };
        update_equipped_item(0);
        panel->schedule(update_equipped_item, SHORT_DELAY, "update_equipped_item");
        return update_equipped_item;
    };

    auto weapon_panel = panel->getChildByName("combat_panel");
    auto mining_panel = panel->getChildByName("mining_panel");
    auto recipe_panel = panel->getChildByName("recipe_panel");

    auto& equipment = EQUIPMENT;
    build_update_equipped_item(weapon_panel, equipment->combat_slot.get(), "Equip a weapon");
    build_update_equipped_item(mining_panel, equipment->mining_slot.get(), "Equip a mining tool");
    build_update_equipped_item(recipe_panel, equipment->recipe_slot.get(), "Equip a crafting tool");

    /// combat
    ui::Button* equip_weapon_btn = dynamic_cast<ui::Button*>(weapon_panel->getChildByName("item_btn"));
    prep_button(equip_weapon_btn);
    bind_touch_ended(equip_weapon_btn, [](){
        GameDirector::switch_to_item_equip_menu(ItemSlotType::Combat);
    });

	/// mining
    ui::Button* equip_mining_btn = dynamic_cast<ui::Button*>(mining_panel->getChildByName("item_btn"));
    prep_button(equip_mining_btn);
    bind_touch_ended(equip_mining_btn,[](){
        GameDirector::switch_to_item_equip_menu(ItemSlotType::Mining);
    });

	///recipe
    ui::Button* equip_recipe_btn = dynamic_cast<ui::Button*>(recipe_panel->getChildByName("item_btn"));
    prep_button(equip_recipe_btn);
    bind_touch_ended(equip_recipe_btn, [](){
        GameDirector::switch_to_item_equip_menu(ItemSlotType::Recipe);
    });

    auto back_btn = dynamic_cast<ui::Button*>(panel->getChildByName("back_btn"));
    prep_back_button(back_btn);

    auto director = cocos2d::Director::getInstance();
    director->pushScene(scene);
};

void GameDirector::switch_to_reset_menu()
{
    auto scene = cocos2d::Scene::create();
    scene->setName("reset_scene");
    set_default_key_handler(scene);

    auto reset_scene = get_prebuilt_node_from_csb("editor/scenes/reset_scene.csb");
    auto panel = dynamic_cast<cocos2d::ui::Layout*>(reset_scene->getChildByName("panel"));
    panel->removeFromParent();
    scene->addChild(panel);

    auto back_btn = dynamic_cast<ui::Button*>(panel->getChildByName("back_btn"));
    prep_back_button(back_btn);

    cocos2d::ui::ListView* reset_pageview = dynamic_cast<cocos2d::ui::ListView*>(panel->getChildByName("reset_listview"));

    struct reset_config {
        std::string title;
        std::string description;
        std::function<void()> reset_callback;
    };

    std::vector<reset_config> configs {
        {
            "Total kills",
            "Resets total kills",
            [](){ DataManager::set_int_from_data("total_kills", 0); }
        },
        {
            "Total Coins",
            "Resets total coins",
            [](){ BEATUP->_total_coins = 0.0; }
        }
    };

    for (auto& config : configs)
    {
        auto nuitem = NuItem::create(reset_pageview);
        nuitem->set_title(config.title);
        nuitem->set_description(config.description);

        //reset size to width of container
        nuitem->button->setContentSize({ reset_pageview->getInnerContainerSize().width, nuitem->button->getContentSize().height });

        auto do_reset = [config](){
                do_vibrate(5);
                config.reset_callback();
        };
        bind_touch_ended(nuitem->button, do_reset);
    }

    auto director = cocos2d::Director::getInstance();
    director->pushScene(scene);
};

float withdraw_ticks = 0.0f;
void bank_withdraw_callback(float dt)
{
    //TODO actually withdraw from bank
};

float deposit_ticks = 0.0f;
void bank_deposit_callback(float dt)
{
    //special case to immediately do something
    if (deposit_ticks == 0.0f) {
        BANK->add_total_coins_banked(10);
    }

    //TODO take from pocket and put into bank
    deposit_ticks += dt;
    if (deposit_ticks > 10.75f) {
        BANK->add_total_coins_banked(deposit_ticks*10);
    }
    else if (deposit_ticks > 2.75f) {
        BANK->add_total_coins_banked(deposit_ticks);
    }
    else if (deposit_ticks > 0.75f) {
        BANK->add_total_coins_banked(deposit_ticks/10);
    }
};

void GameDirector::switch_to_bank_menu()
{
    auto scene = cocos2d::Scene::create();
    scene->setName("bank_scene");
    set_default_key_handler(scene);

    auto bank_scene = get_prebuilt_node_from_csb("editor/scenes/bank_scene.csb");
    auto panel = dynamic_cast<cocos2d::ui::Layout*>(bank_scene->getChildByName("panel"));
    panel->removeFromParent();
    scene->addChild(panel);

    auto back_btn = dynamic_cast<ui::Button*>(panel->getChildByName("back_btn"));
    prep_back_button(back_btn);

    {
        cocos2d::ui::Layout* total_coins_panel = dynamic_cast<cocos2d::ui::Layout*>(panel->getChildByName("total_coins_panel"));
        cocos2d::ui::Text* total_coins_count_lbl = dynamic_cast<cocos2d::ui::Text*>(total_coins_panel->getChildByName("total_coins_count_lbl"));

        auto update_total_coins = [total_coins_count_lbl](float dt) {
            std::string total_coins = beautify_double(BEATUP->get_total_coins());
            total_coins_count_lbl->setString(total_coins);
        };
        update_total_coins(0);
        total_coins_count_lbl->schedule(update_total_coins, SHORT_DELAY, "update_total_coins");
    }

    {
        cocos2d::ui::Layout* banked_coins_panel = dynamic_cast<cocos2d::ui::Layout*>(panel->getChildByName("banked_coins_panel"));
        cocos2d::ui::Text* banked_coins_count_lbl = dynamic_cast<cocos2d::ui::Text*>(banked_coins_panel->getChildByName("banked_coins_count_lbl"));

        auto update_banked_coins = [banked_coins_count_lbl](float dt) {
            std::string banked_coins = beautify_double(BANK->get_total_coins_banked());
            banked_coins_count_lbl->setString(banked_coins);
        };
        update_banked_coins(0);
        banked_coins_count_lbl->schedule(update_banked_coins, SHORT_DELAY, "update_banked_coins");
    }

    cocos2d::ui::ListView* bank_pageview = dynamic_cast<cocos2d::ui::ListView*>(panel->getChildByName("bank_listview"));

    struct bank_config {
        std::string title;
        std::string description;
        std::function<void(float)> bank_callback;
    };

    std::vector<bank_config> configs {
        {
            "Withdraw 10",
            "Withdraw 10 coins in the bank.",
            bank_withdraw_callback
        },
        {
            "Deposit 10",
            "Deposit 10 coins from the bank.",
            bank_deposit_callback
        }
    };


    for (auto& config : configs)
    {
        NuItem* nuitem = NuItem::create(bank_pageview);
        nuitem->set_title(config.title);
        nuitem->set_description(config.description);

        //bank size to width of container
        nuitem->button->setContentSize({ bank_pageview->getInnerContainerSize().width, nuitem->button->getContentSize().height });

        auto listener = cocos2d::EventListenerTouchOneByOne::create();
        listener->setSwallowTouches(false);
        listener->onTouchBegan = [config, nuitem, scene](Touch* touch, Event* event)
        {
            Rect bbox = nuitem->button->getBoundingBox();
            auto converted_touch = nuitem->button->getParent()->convertTouchToNodeSpace(touch);
            if (bbox.containsPoint(converted_touch))
            {
                CCLOG("started %s", config.title.c_str());
                do_vibrate(5);
                nuitem->button->schedule(config.bank_callback, SHORT_DELAY, "bank_callback");
                SoundLibrary::getInstance()->play_general_widget_touched();
                return true;

            }
            return false;
        };
        listener->onTouchEnded = [config, nuitem](Touch* touch, Event* event)
        {
            withdraw_ticks = 0.0f;
            deposit_ticks = 0.0f;

            nuitem->button->unschedule("bank_callback");
        };

        auto event_dispatcher = cocos2d::Director::getInstance()->getEventDispatcher();
        event_dispatcher->addEventListenerWithSceneGraphPriority(listener, nuitem->button);
    }

    auto director = cocos2d::Director::getInstance();
    director->pushScene(scene);
};

void GameDirector::switch_to_achievement_menu()
{
    auto scene = cocos2d::Scene::create();
    scene->setName("achievement_scene");
    set_default_key_handler(scene);

    auto achievement_scene = get_prebuilt_node_from_csb("editor/scenes/achievement_scene.csb");
    auto panel = dynamic_cast<cocos2d::ui::Layout*>(achievement_scene->getChildByName("panel"));
    panel->removeFromParent();
    scene->addChild(panel);

    auto back_btn = dynamic_cast<ui::Button*>(panel->getChildByName("back_btn"));
    prep_back_button(back_btn);

    cocos2d::ui::ListView* achievement_pageview = dynamic_cast<cocos2d::ui::ListView*>(panel->getChildByName("achievement_listview"));

    auto achievements = AchievementManager::getInstance()->getAchievements();
    auto order_by_completed = [](std::shared_ptr<BaseAchievement> left, std::shared_ptr<BaseAchievement> right)
    {
        //incomplete first
        return left->get_completed() < right->get_completed();
    };
    std::sort(achievements.begin(), achievements.end(), order_by_completed);

    auto is_completed = [](const std::shared_ptr<BaseAchievement>& achievement) { return achievement->get_completed(); };
    int completed = std::count_if(achievements.begin(), achievements.end(), is_completed);
    std::stringstream completed_ss;
    completed_ss << "Completed:\n" << completed;
    ((ui::Text*)panel->getChildByName("unlocked_lbl"))->setString(completed_ss.str());

    auto is_not_completed = [](const std::shared_ptr<BaseAchievement>& achievement) { return !achievement->get_completed(); };
    int remaining = std::count_if(achievements.begin(), achievements.end(), is_not_completed);
    std::stringstream not_completed_ss;
    not_completed_ss << "Remaining:\n" << remaining;
    ((ui::Text*)panel->getChildByName("remaining_lbl"))->setString(not_completed_ss.str());

    for (std::shared_ptr<BaseAchievement>& achievement : achievements)
    {
        auto nuitem = NuItem::create(achievement_pageview);
        nuitem->set_title(achievement->get_name());
        nuitem->set_description(achievement->get_description());

        //reset size to width of container
        nuitem->button->setContentSize({ achievement_pageview->getInnerContainerSize().width, nuitem->button->getContentSize().height });
        nuitem->set_progress_panel_visible(true);
        auto counted_achievement = dynamic_cast<CountAchievement*>(achievement.get());
        if (counted_achievement != NULL) {
            auto current = counted_achievement->get_current_count();
            nuitem->current_lbl->setString(beautify_double(current));
            auto target = counted_achievement->get_target_count();
            nuitem->total_lbl->setString(beautify_double(target));
        };

        if (achievement->get_completed()){
            nuitem->set_image("trophy14.png", ui::Widget::TextureResType::PLIST);
        } else {
            nuitem->set_image("trophy1.png", ui::Widget::TextureResType::PLIST);
        };

    }

    auto director = cocos2d::Director::getInstance();
    director->pushScene(scene);
};
