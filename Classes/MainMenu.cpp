#include "MainMenu.h"

#include <algorithm>

#include "ShopScene.h"
#include "Beatup.h"
#include "ui/CocosGUI.h"
#include "SoundEngine.h"
#include "Weapons.h"
#include "DataManager.h"
#include "Combo.h"
#include "Quest.h"
#include "Util.h"
#include "Level.h"
#include <editor-support/cocostudio/CCSGUIReader.h>
#include <editor-support/cocostudio/ActionTimeline/CSLoader.h>
#include "attribute.h"

USING_NS_CC;


Scene* MainMenu::beatup_scene = NULL;

bool MainMenu::init()
{
#ifdef _WIN32
    FUNC_INIT_WIN32(MainMenu);
#else
    FUNC_INIT(MainMenu);
#endif

    SoundEngine::play_music("music\\menu.mp3");

    MainMenu::menu_font = DEFAULT_FONT;
    MainMenu::menu_fontsize = 40;

    this->shop_items = new std::vector<ShopItem*>();

    MainMenu::menu_heightdiff = -1;
    this->last_pos = Vec2(-1, -1);

    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    this->main_lbl = Label::createWithTTF("Beat some people up!", TITLE_FONT, sx(this->menu_fontsize));
    this->main_lbl->setVisible(false);
    this->main_lbl->setPosition(Vec2(
        origin.x + visibleSize.width/2,
        origin.y + visibleSize.height 
    ));
    this->addChild(this->main_lbl, 1);

    this->combo_menu = Menu::create();
    this->addChild(this->combo_menu);

    auto scroll = this->create_center_scrollview();

    BoolFuncNoArgs quit_cb = [this](){
        this->quit(NULL);
        return false;
    };

    std::vector<ItemData> item_data = {
        {"default", "Fight!", MainMenu::quick_start_game, false},
        {"resume_fight", "Resume", MainMenu::resume_game, true},
        {"default", "Level Select", MainMenu::open_levelselect, false},
        {"default", "Options", MainMenu::open_options, false},
        {"default", "Quit", quit_cb, false},
    };

    this->init_menu_from_data(scroll, item_data);

    this->resize_scroll_inner(scroll);

    return true;
};

void MainMenu::onKeyPressed(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event *pEvent)
{
    if(keyCode == EventKeyboard::KeyCode::KEY_BACK || keyCode == EventKeyboard::KeyCode::KEY_ESCAPE) 
    {
        printj("MENU ENDED")
        Director::getInstance()->end();
    }
    else if(keyCode == EventKeyboard::KeyCode::KEY_SPACE || keyCode == EventKeyboard::KeyCode::KEY_ENTER) 
    {
        MainMenu::quick_start_game();
    }
    else if(keyCode == EventKeyboard::KeyCode::KEY_L ) 
    {
        MainMenu::open_levelselect();
    }
    else if(keyCode == EventKeyboard::KeyCode::KEY_O ) 
    {
        MainMenu::open_options();
    }
};

bool MainMenu::quick_start_game()
{
    int level = Level::get_generic_complete() + 1;
    level = std::min(level, 20); //TODO make it so it knows its limit dynamically
    MainMenu::cleanup_beatup_scene();
    MainMenu::beatup_scene = Beatup::createScene(level);
    MainMenu::beatup_scene->retain();

    SoundEngine::stop_all_sound_and_music();
    if (SoundEngine::is_music_enabled())
    {
        // SoundEngine::bg_music_id = SoundEngine::play_sound("music\\start_fight.mp3");
    };

    //use level 1
    MainMenu::resume_game();
    // MainMenu::open_objective();

    return true;
};

bool MainMenu::resume_game()
{
    if (MainMenu::beatup_scene != NULL)
    {
        CCLOG("Resuming fight\n");

        MainMenu::beatup_scene->retain();
        auto director = Director::getInstance();
        director->pushScene(MainMenu::beatup_scene);
    }
    else
    {
        CCLOG("No fight\n");
    };

    return true;
};

bool MainMenu::open_options()
{
    auto scene = Scene::create();
    OptionsMenu* options_menu = OptionsMenu::create();
    scene->addChild(options_menu);

    auto director = Director::getInstance();
    director->pushScene(scene);

    return true;
};

bool MainMenu::open_levelselect()
{
    auto scene = Scene::create();
    LevelSelectMenu* level_select = LevelSelectMenu::create();
    scene->addChild(level_select);

    auto director = Director::getInstance();
    director->pushScene(scene);
    return true;
};

//TODO figure if this needs to exist or not
bool MainMenu::open_objective()
{
    //create a level one if it doesn't already exist
    if (MainMenu::beatup_scene == NULL)
    {
        MainMenu::beatup_scene = Beatup::createScene(1);
        MainMenu::beatup_scene->retain();
    };

    auto scene = Scene::create();
    ObjectiveMenu* objective_menu = ObjectiveMenu::create();
    scene->addChild(objective_menu);

    auto director = Director::getInstance();
    director->pushScene(scene);
    return true;
};

void MainMenu::cleanup_beatup_scene()
{
    if (MainMenu::beatup_scene != NULL)
    {
        MainMenu::beatup_scene->release();
        MainMenu::beatup_scene = NULL;
    };
};

bool OptionsMenu::init()
{
#ifdef _WIN32
    FUNC_INIT_WIN32(OptionsMenu);
#else
    FUNC_INIT(OptionsMenu);
#endif

    menu_font = DEFAULT_FONT;
    menu_fontsize = 40;

    this->shop_items = new std::vector<ShopItem*>();

    menu_heightdiff = -1;
    this->last_pos = Vec2(-1, -1);

    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    auto label = Label::createWithTTF("Welcome to the OptionsMenu", TITLE_FONT, sx(24));

    label->setPosition(Vec2(origin.x + visibleSize.width/2,
                origin.y + visibleSize.height - label->getContentSize().height));

    this->addChild(label, 1);

    this->main_lbl = Label::createWithTTF("OPTIONS!", this->menu_font, this->menu_fontsize);
    this->main_lbl->setPosition(Vec2(origin.x + visibleSize.width/2,
                origin.y + visibleSize.height - this->main_lbl->getContentSize().height-sx(30)));
    this->addChild(this->main_lbl, 1);

    this->combo_menu = Menu::create();
    this->addChild(this->combo_menu);

    auto scroll = this->create_center_scrollview();

    auto go_back_cb = []() {
        auto director = Director::getInstance();
        director->popScene();
        return false;
    };


    auto create_state = [](std::string id_key, std::string noun)
    {

        auto vibration_state = new OptionsMenu::ButtonState;
        vibration_state->current = DataManager::get_bool_from_data(id_key, true);
        auto options_cb = [noun](ui::Button* btn, bool& current)
        {
            if (current)
            {
                std::stringstream ss;
                ss << "[ ] " << noun << " enabled"; //checkmark
                std::string text = ss.str();
                btn->setTitleText(text);
            }
            else
            {
                std::stringstream ss;
                ss << "[X] " << noun << " disabled";
                std::string text = ss.str();
                btn->setTitleText(text);
            };
        };
        vibration_state->swap_state = options_cb;

        return vibration_state;
    };

    auto vibration_state = create_state("vibration_enabled", "Vibration");
    auto tutorial_state = create_state(Beatup::tutorial_id_key, "Tutorials");
    auto sound_state = create_state(SoundEngine::id_string, "Sounds");
    auto music_state = create_state(SoundEngine::music_id_string, "Music");

    std::vector<ItemData> item_data = {
        {"default", "Toggle Sound", OptionsMenu::toggle_sound, false, sound_state},
        {"default", "Toggle Music", OptionsMenu::toggle_music, false, music_state},
        {"default", "Toggle Tutorials", OptionsMenu::toggle_tutorials, false, tutorial_state},
        {"default", "Toggle Vibration", OptionsMenu::toggle_vibration, false, vibration_state},

        {"default", "Reset...", OptionsMenu::open_reset, false},

        {"default", "Back", go_back_cb, false},
    };

    this->init_menu_from_data(scroll, item_data);

    this->resize_scroll_inner(scroll);

    return true;
};

bool OptionsMenu::toggle_sound()
{
    SoundEngine::set_sound_enabled(!SoundEngine::is_sound_enabled());
    return true;
};

bool OptionsMenu::toggle_music()
{
    SoundEngine::set_music_enabled(!SoundEngine::is_music_enabled());
    if (SoundEngine::is_music_enabled())
    {
        SoundEngine::play_music("music\\menu.mp3");
    };
    return true;
};

bool OptionsMenu::toggle_vibration()
{
    std::string vibrate_key = "vibration_enabled";
    bool current_vibration = DataManager::get_bool_from_data(vibrate_key, true);
    DataManager::set_bool_from_data(vibrate_key, !current_vibration);
    return true;
};

bool OptionsMenu::toggle_tutorials()
{
    std::string vibrate_key = Beatup::tutorial_id_key;
    bool current_vibration = DataManager::get_bool_from_data(vibrate_key, true);
    DataManager::set_bool_from_data(vibrate_key, !current_vibration);
    return true;
};

bool OptionsMenu::open_reset()
{
    auto scene = Scene::create();
    ResetMenu* reset_layer = ResetMenu::create();
    scene->addChild(reset_layer);

    auto director = Director::getInstance();
    director->pushScene(scene);
    return true;
};

void OptionsMenu::onKeyPressed(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event *pEvent)
{
    if(keyCode == EventKeyboard::KeyCode::KEY_BACK || keyCode == EventKeyboard::KeyCode::KEY_ESCAPE) 
    {
        pop_scene(NULL);
    }
};

bool LevelSelectMenu::init()
{
#ifdef _WIN32
    FUNC_INIT_WIN32(LevelSelectMenu);
#else
    FUNC_INIT(LevelSelectMenu);
#endif

    menu_font = DEFAULT_FONT;
    menu_fontsize = 40;

    this->shop_items = new std::vector<ShopItem*>();

    menu_heightdiff = -1;
    this->last_pos = Vec2(-1, -1);

    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    // add a label shows "Hello World" create and initialize a label
    auto label = Label::createWithTTF("Opponent Select", menu_font, sx(24));

    // position the label on the center of the screen
    label->setPosition(
        Vec2(
            origin.x + visibleSize.width/2,
            origin.y + visibleSize.height - label->getContentSize().height
        )
    );

    // add the label as a child to this layer
    this->addChild(label, 1);

    this->main_lbl = Label::createWithTTF(
        "Complete missions to unlock more levels",
        this->menu_font,
        this->menu_fontsize
    );
    this->main_lbl->setPosition(
        Vec2(
            origin.x + visibleSize.width/2,
            origin.y + visibleSize.height - label->getContentSize().height-sx(30)
        )
    );
    this->main_lbl->setTextColor(Color4B::WHITE);
    this->addChild(this->main_lbl, 1);

    auto scroll = this->create_center_scrollview();

    this->combo_menu = Menu::create();
    this->addChild(this->combo_menu);

    int last_level = DataManager::get_int_from_data(Level::generic_id_key);
    CCLOG("current level %d", last_level);

    for (int i = 1; i <= last_level + 1; i++)
    {
        Level* level = Level::create_level(i);
        if (level != NULL)
        {
            std::string lvl_lbl = level->scene_detail.name;
            std::stringstream ss;
            ss << i << ") " << lvl_lbl;
            auto btn = create_button(ss.str(), start_new_game(level->order));
            scroll->addChild(btn);
        }
        else
        {
            CCLOG("stopping level creation at %i", i);
            break;
        };
    }

    //back button
    auto btn = create_button("Back", []() 
    {
        auto director = Director::getInstance();
        director->popScene();
        return false;
    });
    scroll->addChild(btn);

    this->resize_scroll_inner(scroll);
    return true;
};

BoolFuncNoArgs LevelSelectMenu::start_new_game(int level)
{

    BoolFuncNoArgs cb = [level]()
    {
        MainMenu::cleanup_beatup_scene();

        SoundEngine::stop_all_sound_and_music();
        if (SoundEngine::is_music_enabled())
        {
            // SoundEngine::bg_music_id = SoundEngine::play_sound("music\\start_fight.mp3");
        };

        MainMenu::beatup_scene = Beatup::createScene(level);
        MainMenu::beatup_scene->retain();

        MainMenu::resume_game();

        return false;
    };
    return cb;
};


void LevelSelectMenu::onKeyPressed(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event *pEvent)
{
    if(keyCode == EventKeyboard::KeyCode::KEY_BACK || keyCode == EventKeyboard::KeyCode::KEY_ESCAPE) 
    {
        pop_scene(NULL);
    }
};

bool ObjectiveMenu::init()
{
#ifdef _WIN32
    FUNC_INIT_WIN32(ObjectiveMenu);
#else
    FUNC_INIT(ObjectiveMenu);
#endif

    menu_font = DEFAULT_FONT;
    menu_fontsize = 40;

    this->shop_items = new std::vector<ShopItem*>();

    menu_heightdiff = -1;
    this->last_pos = Vec2(-1, -1);

    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    // add a label shows "Hello World" create and initialize a label
    auto label = Label::createWithTTF("Mission Screen", menu_font, sx(24));

    // position the label on the center of the screen
    label->setPosition(
        Vec2(
            origin.x + visibleSize.width/2,
            origin.y + visibleSize.height - label->getContentSize().height
        )
    );

    // add the label as a child to this layer
    this->addChild(label, 1);

    this->main_lbl = Label::createWithTTF(
        "Rewards for completion!",
        this->menu_font,
        this->menu_fontsize
    );
    this->main_lbl->setPosition(
        Vec2(
            origin.x + visibleSize.width/2,
            origin.y + visibleSize.height - this->main_lbl->getContentSize().height-sx(30)
        )
    );
    this->main_lbl->setTextColor(Color4B::BLUE);
    this->addChild(this->main_lbl, 1);

    auto scroll = this->create_center_scrollview();

    this->combo_menu = Menu::create();
    this->addChild(this->combo_menu);

    auto raw = MainMenu::beatup_scene->getChildByName("beatup");
    Beatup* beatup = dynamic_cast<Beatup*>(raw);
    Quest* quest = beatup->quest;

    auto quest_name = create_button(quest->name, [](){ return true;}, true);
    scroll->addChild(quest_name);

    for (Objective* obj : *quest->objectives)
    {
        auto obj_btn = create_button(obj->name, [](){ return true;});
        scroll->addChild(obj_btn);
    };

    //back button
    auto btn = create_button("Start", []() 
    {
        auto director = Director::getInstance();
        director->pushScene(MainMenu::beatup_scene);
        return false;
    });
    scroll->addChild(btn);

    this->resize_scroll_inner(scroll);
    return true;
};

void ObjectiveMenu::onKeyPressed(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event *pEvent)
{
    if(keyCode == EventKeyboard::KeyCode::KEY_BACK || keyCode == EventKeyboard::KeyCode::KEY_ESCAPE) 
    {
        pop_scene(NULL);
    }
    else if(keyCode == EventKeyboard::KeyCode::KEY_SPACE 
            || keyCode == EventKeyboard::KeyCode::KEY_Q 
            || keyCode == EventKeyboard::KeyCode::KEY_E 
            || keyCode == EventKeyboard::KeyCode::KEY_ENTER) 
    {
        auto director = Director::getInstance();
        director->pushScene(MainMenu::beatup_scene);
    };
};

bool ResetMenu::init()
{
#ifdef _WIN32
    FUNC_INIT_WIN32(ResetMenu);
#else
    FUNC_INIT(ResetMenu);
#endif

    menu_font = DEFAULT_FONT;
    menu_fontsize = 40;

    this->shop_items = new std::vector<ShopItem*>();

    menu_heightdiff = -1;
    this->last_pos = Vec2(-1, -1);

    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    // add a label shows "Hello World" create and initialize a label
    auto label = Label::createWithTTF("Reset your save data", menu_font, sx(24));

    // position the label on the center of the screen
    label->setPosition(
        Vec2(
            origin.x + visibleSize.width/2,
            origin.y + visibleSize.height - label->getContentSize().height
        )
    );

    // add the label as a child to this layer
    this->addChild(label, 1);

    this->main_lbl = Label::createWithTTF(
        "CAUTION: No way to undo this",
        this->menu_font,
        this->menu_fontsize
    );
    this->main_lbl->setPosition(
        Vec2(
            origin.x + visibleSize.width/2,
            origin.y + visibleSize.height - this->main_lbl->getContentSize().height-sx(30)
        )
    );
    this->main_lbl->setTextColor(Color4B::WHITE);
    this->addChild(this->main_lbl, 1);

    auto scroll = this->create_center_scrollview();

    this->combo_menu = Menu::create();
    this->addChild(this->combo_menu);
    auto go_back_cb = []() {
       auto director = Director::getInstance();
       director->popScene();
       return false;
    };



    std::vector<ItemData> item_data = {
        {"default", "Reset All", ResetMenu::reset_all, false},
        {"default", "Reset Coins", ResetMenu::reset_total_coin_stat, false},
        {"default", "Reset Levels", ResetMenu::reset_levels, false},
        {"default", "Reset Combos", ResetMenu::reset_all_combos, false},
        {"default", "Reset Total Hits", ResetMenu::reset_total_hit_stat, false},
        {"default", "Reset Fists", ResetMenu::reset_all_fist_weapons, false},
        {"default", "Back", go_back_cb, false},
    };

    this->init_menu_from_data(scroll, item_data);



    this->resize_scroll_inner(scroll);
    return true;
};

void ResetMenu::onKeyPressed(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event *pEvent)
{
    if(keyCode == EventKeyboard::KeyCode::KEY_BACK || keyCode == EventKeyboard::KeyCode::KEY_ESCAPE) 
    {
        pop_scene(NULL);
    }
    else if(keyCode == EventKeyboard::KeyCode::KEY_SPACE 
            || keyCode == EventKeyboard::KeyCode::KEY_Q 
            || keyCode == EventKeyboard::KeyCode::KEY_E 
            || keyCode == EventKeyboard::KeyCode::KEY_ENTER) 
    {
        auto director = Director::getInstance();
        director->pushScene(MainMenu::beatup_scene);
    };
};

bool ResetMenu::reset_all()
{
    ResetMenu::reset_total_hit_stat();
    ResetMenu::reset_total_coin_stat();
    ResetMenu::reset_all_combos();
    ResetMenu::reset_all_fist_weapons();
    ResetMenu::reset_levels();

    return true;
};

bool ResetMenu::reset_total_hit_stat()
{
    int default_stat = 0;
    UserDefault* ud = UserDefault::getInstance();
    ud->setIntegerForKey(Beatup::total_hit_key.c_str(), default_stat);
    log("reset hit stats");
    return true;
};

bool ResetMenu::reset_all_combos()
{
    Beatup* bad_beatup = (Beatup*)Beatup::create();
    for (Combo* combo : *bad_beatup->combos)
    {
        combo->set_been_bought(false);
    };
    log("reset all combos");
    return true;
};

bool ResetMenu::reset_all_fist_weapons()
{
    Beatup* bad_beatup = (Beatup*)Beatup::create();
    bad_beatup->fist_flame->set_been_bought(false);
    bad_beatup->fist_psionic->set_been_bought(false);
    bad_beatup->fist_frost->set_been_bought(false);

    DataManager::set_bool_from_data("left_fist_charge_boost", false);
    DataManager::set_bool_from_data("right_fist_charge_boost", false);
    DataManager::set_bool_from_data("charging_enabled", false);

    log("reset all fist weapons");
    return true;
};

bool ResetMenu::reset_total_coin_stat()
{
    int default_stat = 0;
    UserDefault* ud = UserDefault::getInstance();
    ud->setIntegerForKey(Beatup::total_coin_key.c_str(), default_stat);
    log("reset coin stats");
    return true;
};

bool ResetMenu::reset_levels()
{
    int default_stat = 0;
    UserDefault* ud = UserDefault::getInstance();
    ud->setIntegerForKey(Level::generic_id_key.c_str(), default_stat);
    log("reset level");
    return true;
};

BuildingMenu* BuildingMenu::create(spBuilding building)
{
    BuildingMenu *pRet = new(std::nothrow) BuildingMenu();
    pRet->building = building; //this should be after init, cause i guess init should fail, but its fine for now.

    if (pRet && pRet->init()) {
        pRet->autorelease();
        return pRet;
    }
    else
    {
        delete pRet;
        pRet = nullptr; 
        return pRet;
    }
};

bool BuildingMenu::init()
{
#ifdef _WIN32
    FUNC_INIT_WIN32(BuildingMenu);
#else
    FUNC_INIT(BuildingMenu);
#endif

    menu_font = DEFAULT_FONT;
    menu_fontsize = 40;

    this->shop_items = new std::vector<ShopItem*>();

    menu_heightdiff = -1;
    this->last_pos = Vec2(-1, -1);

    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    // add a label shows "Hello World" create and initialize a label
    std::stringstream ss;
    ss << "Building: " << this->building->name;
    auto label = Label::createWithTTF(ss.str(), menu_font, sx(24));

    // position the label on the center of the screen
    label->setPosition(
        Vec2(
            origin.x + visibleSize.width/2,
            origin.y + visibleSize.height - label->getContentSize().height
        )
    );

    // add the label as a child to this layer
    this->addChild(label, 1);

    this->main_lbl = Label::createWithTTF(
        "Building Menu",
        this->menu_font,
        this->menu_fontsize
    );
    this->main_lbl->setPosition(
        Vec2(
            origin.x + visibleSize.width/2,
            origin.y + visibleSize.height - this->main_lbl->getContentSize().height-sx(30)
        )
    );
    this->main_lbl->setTextColor(Color4B::WHITE);
    this->addChild(this->main_lbl, 1);

    auto scroll = this->create_center_scrollview();

    this->combo_menu = Menu::create();
    this->addChild(this->combo_menu);
    auto go_back_cb = []() {
       auto director = Director::getInstance();
       director->popScene();
       return false;
    };

    auto open_bldg_inventory = [this]() {

        auto scene = Scene::create();
        InventoryMenu* building_menu = InventoryMenu::create(this->building);
        scene->addChild(building_menu);

        auto director = Director::getInstance();
        director->pushScene(scene);
        return false;
    };

    std::vector<ItemData> item_data = {
        {"default", "Inventory", open_bldg_inventory, false},
        {"default", "Back", go_back_cb, false},
    };
    item_data.insert(
        item_data.begin(),
        this->building->menu_items.begin(),
        this->building->menu_items.end()
    );

    this->init_menu_from_data(scroll, item_data);



    this->resize_scroll_inner(scroll);
    return true;
};

void BuildingMenu::onKeyPressed(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event *pEvent)
{
    if(keyCode == EventKeyboard::KeyCode::KEY_SPACE 
            || keyCode == EventKeyboard::KeyCode::KEY_ENTER) 
    {
        auto director = Director::getInstance();
        director->popScene();
    };
};

void BuildingMenu::onKeyReleased(EventKeyboard::KeyCode keyCode, Event *pEvent)
{
    if(keyCode == EventKeyboard::KeyCode::KEY_BACK || keyCode == EventKeyboard::KeyCode::KEY_ESCAPE) 
    {
        auto director = Director::getInstance();
        director->popScene();
    }
};

CityMenu* CityMenu::create(spBuilding building)
{
    CityMenu *pRet = new(std::nothrow) CityMenu();
    pRet->building = building; //this should be after init, cause i guess init should fail, but its fine for now.

    if (pRet && pRet->init()) {
        pRet->autorelease();
        return pRet;
    }
    else
    {
        delete pRet;
        pRet = nullptr; 
        return pRet;
    }
};

bool CityMenu::init()
{
#ifdef _WIN32
    FUNC_INIT_WIN32(CityMenu);
#else
    FUNC_INIT(CityMenu);
#endif

    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    // add a label shows "Hello World" create and initialize a label
    std::stringstream ss;
    ss << "City Menu";
    auto label = Label::createWithTTF(ss.str(), menu_font, sx(24));
    this->addChild(label, 1);

    // position the label on the center of the screen
    label->setPosition(
        Vec2(
            origin.x + visibleSize.width/2,
            origin.y + visibleSize.height - label->getContentSize().height
        )
    );

    auto inst = CSLoader::getInstance();
    auto city_scene = inst->createNode("editor/scenes/city_scene.csb");
    city_scene->setPosition(this->get_center_pos());
    city_scene->setAnchorPoint(Vec2(0.5,0.5));

    auto create_count = [](std::string prefix, int count) {
        std::stringstream ss;
        ss << prefix << ": " << count;
        return ss.str();
    };

    for (auto building : this->building->city->buildings)
    {

       auto node = city_scene->getChildByName(building->name);
       auto building_panel = dynamic_cast<ui::Layout*>(node->getChildByName("building_panel"));

       auto building_name_lbl = dynamic_cast<ui::Text*>(building_panel->getChildByName("building_name")); 
       building_name_lbl->setString(building->name);

       auto ing_count = dynamic_cast<ui::Text*>(building_panel->getChildByName("ingredient_count"));
       ing_count->setString(create_count("ING", building->ingredients.size()));

       auto pro_count = dynamic_cast<ui::Text*>(building_panel->getChildByName("product_count"));
       pro_count->setString(create_count("PRO", building->products.size()));

       auto wst_count = dynamic_cast<ui::Text*>(building_panel->getChildByName("waste_count"));
       wst_count->setString(create_count("WST", building->wastes.size()));

       auto cb = [this, building](Ref*, ui::Widget::TouchEventType) {
           auto scene = Scene::create();
           BuildingMenu* building_menu = BuildingMenu::create(building);
           scene->addChild(building_menu);

           auto director = Director::getInstance();
           director->pushScene(scene);
       };
       building_panel->addTouchEventListener(cb);

    }
    this->addChild(city_scene);


    // this->main_lbl = Label::createWithTTF(
    //     "",
    //     this->menu_font,
    //     this->menu_fontsize
    // );
    // this->main_lbl->setPosition(
    //     Vec2(
    //         origin.x + visibleSize.width/2,
    //         origin.y + visibleSize.height - this->main_lbl->getContentSize().height-sx(30)
    //     )
    // );
    // this->main_lbl->setTextColor(Color4B::WHITE);
    // this->addChild(this->main_lbl, 1);

    return true;
};

void CityMenu::onKeyPressed(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event *pEvent)
{
    if(keyCode == EventKeyboard::KeyCode::KEY_SPACE 
            || keyCode == EventKeyboard::KeyCode::KEY_ENTER) 
    {
        auto director = Director::getInstance();
        director->popScene();
    };
};

BuildingNode::BuildingNode()
{
    float width = 50;
    float height = 50;

    this->bg_layer = LayerColor::create(Color4B::GREEN, width, height);
    this->addChild(this->bg_layer);

    this->building_label = Label::createWithTTF("unset bldg", TITLE_FONT, 12);
    this->addChild(this->building_label);


    this->setContentSize(Size(width, height));
};

void BuildingNode::set_building(spBuilding bldg)
{
    this->building = bldg;
};

bool InventoryMenu::init()
{
#ifdef _WIN32
    FUNC_INIT_WIN32(InventoryMenu);
#else
    FUNC_INIT(InventoryMenu);
#endif

    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    // add a label shows "Hello World" create and initialize a label
    std::stringstream ss;
    ss << "Building: " << this->building->name;
    auto label = Label::createWithTTF(ss.str(), menu_font, sx(24));

    // position the label on the center of the screen
    label->setPosition(
        Vec2(
            origin.x + visibleSize.width/2,
            origin.y + visibleSize.height - label->getContentSize().height
        )
    );

    // auto bldg_node = BuildingNode::create();
    // bldg_node->setPosition(this->get_center_pos());
    // this->addChild(bldg_node);
    auto resize_btn = [](ui::Button* button) {
        auto lbl_size = button->getTitleRenderer()->getContentSize();

        button->setContentSize(
                Size(
                    lbl_size.width * 1.1f,
                    lbl_size.height * 1.1f
                    )
                );
    };

    auto inst = CSLoader::getInstance();

    int ingredient_count = building->ingredients.size();
    int num_cols = 5;

    //load dummy node to get size
    //and pull the panel out of node because node's not a widget and has no size
    auto raw_node = inst->createNode("editor/buttons/inventory_button.csb");
    auto original_panel = dynamic_cast<ui::Widget*>(raw_node->getChildByName("Panel_1"));
    original_panel->removeFromParent();
    auto cat = dynamic_cast<Sprite*>(original_panel->getChildByName("cat_1"));

    float panel_width = original_panel->getContentSize().width;

    //prep param to apply to rows and cols
    auto param = ui::LinearLayoutParameter::create();
    auto margin = ui::Margin(0, 10, 10, 0);
    param->setMargin(margin);


    float width = (panel_width+margin.left+margin.right)*num_cols;

    //set base layout TODO variable height
    auto layout = ui::Layout::create();
    layout->setLayoutType(ui::Layout::Type::VERTICAL);
    layout->setContentSize(Size(width, 500)); 
    layout->setAnchorPoint(Vec2(0.5, 0.5));

    auto map_type = building->get_ingredient_count();
    auto it = map_type.begin();
    auto end_it = map_type.end();

    for (int i = 0; i < 99; i++) {
        auto inner_layout = ui::HBox::create();
        inner_layout->setContentSize(Size(width, 100));
        inner_layout->setLayoutParameter(param);

        for (int j = 0; j < num_cols; j++) {
            if (it == end_it){
                break;
            };

            if (it != end_it) {
                Ingredient::IngredientType ing_type = it->first;
                int count = it->second;

                if (count == 0) {
                    std::advance(it, 1);
                    j--;
                    continue;
                };

                auto panel = original_panel->clone();
                panel->setLayoutParameter(param);

                auto raw_butn = panel->getChildByName("resource_btn");
                auto btn = dynamic_cast<ui::Button*>(raw_butn);

                auto cb = [ing_type, this](Ref*, ui::Widget::TouchEventType type) {
                    if (type == ui::Widget::TouchEventType::ENDED) {
                        auto alert = InventoryMenu::create_detail_alert(building, ing_type);
                        this->addChild(alert);
                    };
                };
                btn->addTouchEventListener(cb);

                auto type_str = Ingredient::type_to_string(ing_type);

                std::stringstream ss;
                ss << count << std::endl << type_str;
                btn->setTitleText(ss.str());

                inner_layout->addChild(panel);

            }

            std::advance(it, 1);

        };

        layout->addChild(inner_layout);

        if (it == end_it){
            break;
        };
    };
    layout->setPosition(this->get_center_pos());

    this->addChild(layout);

    // add the label as a child to this layer
    this->addChild(label, 1);

    this->main_lbl = Label::createWithTTF(
        "Inventory Menu",
        this->menu_font,
        this->menu_fontsize
    );
    this->main_lbl->setPosition(
        Vec2(
            origin.x + visibleSize.width/2,
            origin.y + visibleSize.height - this->main_lbl->getContentSize().height-sx(30)
        )
    );
    this->main_lbl->setTextColor(Color4B::WHITE);
    this->addChild(this->main_lbl, 1);

    return true;
};

void InventoryMenu::onKeyPressed(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event *pEvent)
{
    if(keyCode == EventKeyboard::KeyCode::KEY_SPACE 
            || keyCode == EventKeyboard::KeyCode::KEY_ENTER) 
    {
        auto director = Director::getInstance();
        director->popScene();
    }
    else  if (keyCode == EventKeyboard::KeyCode::KEY_Q)
    {
        auto original_panel = InventoryMenu::create_detail_alert(this->building, Ingredient::IngredientType::Grain);
        this->addChild(original_panel);
    }
};

InventoryMenu* InventoryMenu::create(spBuilding building)
{
    InventoryMenu *menu = new(std::nothrow) InventoryMenu();
    menu->building = building; //this should be after init, cause i guess init should fail, but its fine for now.

    if (menu && menu->init()) {
        menu->autorelease();
        return menu;
    }
    else
    {
        delete menu;
        menu = nullptr; 
        return menu;
    }
};

ui::Widget* InventoryMenu::create_detail_alert(spBuilding building, Ingredient::IngredientType type)
{
    auto inst = CSLoader::getInstance();
    auto raw_node = inst->createNode("editor/details/inventory_detail.csb");
    auto original_panel = dynamic_cast<ui::Widget*>(raw_node->getChildByName("Panel_1"));
    original_panel->removeFromParent();

    auto cb = [original_panel](Ref*, ui::Widget::TouchEventType type) {
        if (type == ui::Widget::TouchEventType::ENDED)
        {
            original_panel->removeFromParent();
        };
    };
    original_panel->addTouchEventListener(cb);

    auto resource_name = dynamic_cast<ui::Text*>(original_panel->getChildByName("resource_name"));
    std::string res_name = Ingredient::type_to_string(type);
    resource_name->setString(res_name);

    auto resource_description = dynamic_cast<ui::Text*>(original_panel->getChildByName("resource_description"));
    resource_description->setString("Grain is good to eat\nits a lot of fun to touch\ni could go for some right now");

    auto count_desc = dynamic_cast<ui::Text*>(original_panel->getChildByName("count_desc"));
    auto count_lbl = dynamic_cast<ui::Text*>(original_panel->getChildByName("count_lbl"));
    int count = building->get_ingredient_count()[type];
    count_lbl->setString(std::to_string(count));

    original_panel->setPosition(this->get_center_pos());

    return original_panel;
};

bool CharacterMenu::init()
{
#ifdef _WIN32
    FUNC_INIT_WIN32(CharacterMenu);
#else
    FUNC_INIT(CharacterMenu);
#endif

    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    // add a label shows "Hello World" create and initialize a label
    std::stringstream ss;
    ss << "Fighter detail: " << this->fighter->name;
    auto label = Label::createWithTTF(ss.str(), menu_font, sx(24));

    // position the label on the center of the screen
    label->setPosition(
        Vec2(
            origin.x + visibleSize.width/2,
            origin.y + visibleSize.height - label->getContentSize().height
        )
    );

    // auto bldg_node = FighterNode::create();
    // bldg_node->setPosition(this->get_center_pos());
    // this->addChild(bldg_node);
    auto resize_btn = [](ui::Button* button) {
        auto lbl_size = button->getTitleRenderer()->getContentSize();

        button->setContentSize(
                Size(
                    lbl_size.width * 1.1f,
                    lbl_size.height * 1.1f
                    )
                );
    };

    auto inst = CSLoader::getInstance();

    int num_cols = 5;

    auto raw_node = inst->createNode("editor/details/character_detail.csb");
    auto original_panel = dynamic_cast<ui::Widget*>(raw_node->getChildByName("Panel_1"));
    original_panel->removeFromParent();

    float panel_width = original_panel->getContentSize().width;
    auto param = ui::LinearLayoutParameter::create();
    auto margin = ui::Margin(0, 10, 10, 0);
    param->setMargin(margin);
    float width = (panel_width+margin.left+margin.right);

    auto character_name = dynamic_cast<ui::Text*>(original_panel->getChildByName("character_name"));
    character_name->setString(fighter->name);

    auto character_art = dynamic_cast<ui::ImageView*>(original_panel->getChildByName("character_art"));
    character_art->loadTexture(fighter->sprite_name, ui::TextureResType::PLIST);

    auto hp_val = dynamic_cast<ui::Text*>(original_panel->getChildByName("hp_val"));
    hp_val->setString(std::to_string((int)fighter->attrs->health->current_val));

    auto xp_val = dynamic_cast<ui::Text*>(original_panel->getChildByName("xp_val"));
    xp_val->setString(std::to_string((int)fighter->xp->total));

    auto dmg_val = dynamic_cast<ui::Text*>(original_panel->getChildByName("dmg_val"));
    dmg_val->setString(std::to_string((int)fighter->attrs->damage->current_val));

    auto bns_val = dynamic_cast<ui::Text*>(original_panel->getChildByName("bns_val"));
    std::string bns_msg = "x1";
    if (this->fighter->has_sword) { bns_msg = "x2"; };
    bns_val->setString(bns_msg);

    auto layout = ui::Layout::create();
    layout->setLayoutType(ui::Layout::Type::VERTICAL);
    layout->setContentSize(Size(width, 500)); 
    layout->setAnchorPoint(Vec2(0.5, 0.5));

    layout->setPosition(this->get_center_pos());

    layout->addChild(original_panel);

    this->addChild(layout);

    // add the label as a child to this layer
    this->addChild(label, 1);

    this->main_lbl = Label::createWithTTF(
        "Character menu",
        this->menu_font,
        this->menu_fontsize
    );
    this->main_lbl->setPosition(
        Vec2(
            origin.x + visibleSize.width/2,
            origin.y + visibleSize.height - this->main_lbl->getContentSize().height-sx(30)
        )
    );
    this->main_lbl->setTextColor(Color4B::WHITE);
    this->addChild(this->main_lbl, 1);

    return true;
};

void CharacterMenu::onKeyPressed(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event *pEvent)
{
    if(keyCode == EventKeyboard::KeyCode::KEY_SPACE 
            || keyCode == EventKeyboard::KeyCode::KEY_ENTER) 
    {
        auto director = Director::getInstance();
        director->popScene();
    }
    else  if (keyCode == EventKeyboard::KeyCode::KEY_Q)
    {
    }
};

CharacterMenu* CharacterMenu::create(spFighter fighter)
{
    CharacterMenu *menu = new(std::nothrow) CharacterMenu();
    menu->fighter = fighter; //this should be after init, cause i guess init should fail, but its fine for now.

    if (menu && menu->init()) {
        menu->autorelease();
        return menu;
    }
    else
    {
        delete menu;
        menu = nullptr; 
        return menu;
    }
};


