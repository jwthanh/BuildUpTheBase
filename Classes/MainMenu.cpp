#include "MainMenu.h"

#include <algorithm>

#include "BaseMenu.h"
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
#include "NuMenu.h"
#include "GameLogic.h"
#include "StaticData.h"
#include "Animal.h"
#include "MiscUI.h"

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

    scroll->resize_to_fit();

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

void MainMenu::onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event)
{
    if(keyCode == EventKeyboard::KeyCode::KEY_BACK || keyCode == EventKeyboard::KeyCode::KEY_ESCAPE) 
    {
        this->quit(NULL);
        event->stopPropagation();
    }
};

bool MainMenu::quick_start_game()
{
    int level = Level::get_generic_complete() + 1;
    level = std::min(level, 20); //TODO make it so it knows its limit dynamically
    MainMenu::cleanup_beatup_scene();
    MainMenu::beatup_scene = Beatup::createScene(level);
    MainMenu::beatup_scene->retain(); //TODO make sure this gets released at some point

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

    scroll->resize_to_fit();

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

    scroll->resize_to_fit();
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

    scroll->resize_to_fit();
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
        {"default", "Reset Total Hits", ResetMenu::reset_total_hit_stat, false},
        {"default", "Back", go_back_cb, false},
    };

    this->init_menu_from_data(scroll, item_data);



    scroll->resize_to_fit();
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

bool CityMenu::init()
{
#ifdef _WIN32
    FUNC_INIT_WIN32(CityMenu);
#else
    FUNC_INIT(CityMenu);
#endif

    float update_delay = 0.1f;

    this->menu_state = OpenMenu;

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

    auto back_btn = dynamic_cast<ui::Button*>(city_scene->getChildByName("back_btn"));
    back_btn->addTouchEventListener([](Ref* touch, ui::Widget::TouchEventType type){
        if (type == ui::Widget::TouchEventType::ENDED)
        {
            auto director = Director::getInstance();
            director->popScene();
        }
    });
    load_default_button_textures(back_btn);

    auto target_btn = dynamic_cast<ui::Button*>(city_scene->getChildByName("target_btn"));
    target_btn->addTouchEventListener([this](Ref* touch, ui::Widget::TouchEventType type){
        if (type == ui::Widget::TouchEventType::ENDED)
        {
            if (this->menu_state == OpenMenu) {
                this->menu_state = ChangeTarget;
            } else {
                this->menu_state = OpenMenu;
            };

        }
    });

    auto update_target_btn = [target_btn, this](float dt)
    {
        if (this->menu_state == OpenMenu) { target_btn->setTitleText("Menu mode"); }
        else { target_btn->setTitleText("Target mode"); }
    };
    update_target_btn(0);
    target_btn->schedule(update_target_btn, update_delay, "update_target_btn");

    load_default_button_textures(target_btn);

    auto create_count = [](std::string prefix, int count) {
        std::stringstream ss;
        ss << prefix << ": " << count;
        return ss.str();
    };

    for (auto building : BUILDUP->city->buildings)
    {

       auto node = city_scene->getChildByName(building->name);
       auto building_panel = dynamic_cast<ui::Layout*>(node->getChildByName("building_panel"));

        if (building->get_been_bought() == false) {
            node->setVisible(false);
            continue;
        };

       auto building_name_lbl = dynamic_cast<ui::Text*>(building_panel->getChildByName("building_name")); 
       building_name_lbl->setString(building->name);

       auto building_image = dynamic_cast<ui::ImageView*>(building_panel->getChildByName("building_image")); 
       building_image->loadTexture(building->data->get_img_large(), ui::TextureResType::PLIST);

       auto ing_count = dynamic_cast<ui::Text*>(building_panel->getChildByName("ingredient_count"));
       ing_count->setString(create_count("ING", building->ingredients.size()));

       auto pro_count = dynamic_cast<ui::Text*>(building_panel->getChildByName("product_count"));
       pro_count->setString(create_count("PRO", building->products.size()));

       auto wst_count = dynamic_cast<ui::Text*>(building_panel->getChildByName("waste_count"));
       wst_count->setString(create_count("WST", building->wastes.size()));

       auto cb = [this, building](Ref* target, ui::Widget::TouchEventType event) {
           if (event == ui::Widget::TouchEventType::ENDED)
           {
               auto director = Director::getInstance();
               if (this->menu_state == OpenMenu) {
                   auto building_panel = dynamic_cast<ui::Layout*>(target);
                   auto scene = Scene::create();
                   BuildingNuMenu* building_menu = BuildingNuMenu::create(building);
                   scene->addChild(building_menu);

                   director->pushScene(scene);
               } else {
                   //change target building and exit menu
                   BUILDUP->target_building = building;
                   this->menu_state = OpenMenu;
                   director->popScene();
               };
           };
       };
       building_panel->addTouchEventListener(cb);

       auto update_panel = [this, building_panel, building](float dt) {
           if (BUILDUP->target_building == building) {
               building_panel->setBackGroundColor(Color3B::BLUE);
           } else {
               building_panel->setBackGroundColor(Color3B(150, 200, 255));
           }
       };
       update_panel(0);
       building_panel->schedule(update_panel, update_delay, "update_panel");

    }

    this->addChild(city_scene);

    return true;
};

void CityMenu::pop_scene(cocos2d::Ref* pSender)
{
    GameLayer::pop_scene(pSender);
}

void CityMenu::onEnter()
{
    GameLayer::onEnter();
}

void CityMenu::onKeyPressed(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event *pEvent)
{
    if(keyCode == EventKeyboard::KeyCode::KEY_SPACE 
            || keyCode == EventKeyboard::KeyCode::KEY_ENTER) 
    {
        auto director = Director::getInstance();
        director->popScene();
        pEvent->stopPropagation();
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

    auto back_btn = ui::Button::create();
    back_btn->addTouchEventListener([](Ref* touch, ui::Widget::TouchEventType type){
        if (type == ui::Widget::TouchEventType::ENDED)
        {
            auto director = Director::getInstance();
            director->popScene();
        }
    });
    load_default_button_textures(back_btn);
    back_btn->setPosition(Vec2(90, 540));
    back_btn->ignoreContentAdaptWithSize(false);
    back_btn->setContentSize(Size(125, 55));
    back_btn->setTitleText("Back");
    back_btn->setTitleFontSize(24);
    back_btn->setTitleColor(Color3B::BLACK);

    this->addChild(back_btn);

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
    auto orig_item_panel = dynamic_cast<ui::Widget*>(raw_node->getChildByName("item_panel"));
    orig_item_panel->removeFromParent();

    float panel_width = orig_item_panel->getContentSize().width;

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

    auto ingredients = building->ingredients;
    auto it = ingredients.begin();
    auto end_it = ingredients.end();

    for (int i = 0; i < 99; i++) {
        auto horiz_layout = ui::HBox::create();
        horiz_layout->setContentSize(Size(width, 100));
        horiz_layout->setLayoutParameter(param);

        for (int j = 0; j < num_cols; j++) {
            if (it == end_it){
                break;
            };

            if (it != end_it) {
                Ingredient::SubType ing_type = it->first;
                int count = it->second;

                if (count == 0) {
                    std::advance(it, 1);
                    j--;
                    continue;
                };

                auto new_item_panel = orig_item_panel->clone();
                new_item_panel->setLayoutParameter(param);

                auto cb = [ing_type, this, new_item_panel](Ref* ref, ui::Widget::TouchEventType type) {

                    if (type == ui::Widget::TouchEventType::ENDED) {

                        //make sure one doesn't already exist first
                        if (this->getChildByName("inventory_detail_panel")) {
                            this->getChildByName("inventory_detail_panel")->removeFromParent();
                        };

                        auto alert = InventoryMenu::create_detail_alert(this->building, ing_type);
                        this->addChild(alert);

                        //animate
                        Vec2 start_pos = new_item_panel->getTouchEndPosition();
                        alert->setPosition(start_pos);

                        alert->setScale(0);

                        float duration = 0.25f;
                        auto scale = ScaleTo::create(duration, 1.0f, 1.0f);

                        Vec2 end_pos = this->get_center_pos();
                        auto move = MoveTo::create(duration, end_pos);

                        Sequence* seq = Sequence::create(Spawn::createWithTwoActions(move, scale), NULL);
                        alert->runAction(seq);
                    };
                };
                new_item_panel->addTouchEventListener(cb);

                float update_delay = 0.1f;
                auto update_lbl_cb = [new_item_panel, this, ing_type](float)
                {
                    auto type_str = Ingredient::type_to_string(ing_type);
                    std::stringstream ss;
                    ss << this->building->count_ingredients(ing_type) << " " << type_str;
                    auto item_lbl = dynamic_cast<ui::Text*>(new_item_panel->getChildByName("item_lbl"));
                    item_lbl->setString(ss.str());
                };
                update_lbl_cb(0); //fire once immediately
                new_item_panel->schedule(update_lbl_cb, update_delay, "item_lbl_update");

                horiz_layout->addChild(new_item_panel);

            }

            std::advance(it, 1);

        };

        layout->addChild(horiz_layout);

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
        pEvent->stopPropagation();
    }
    else  if (keyCode == EventKeyboard::KeyCode::KEY_Q)
    {
        auto original_panel = InventoryMenu::create_detail_alert(this->building, Ingredient::SubType::Grain);
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

ui::Widget* InventoryMenu::create_detail_alert(spBuilding building, Ingredient::SubType ing_type)
{
    auto inst = CSLoader::getInstance();
    auto raw_node = inst->createNode("editor/details/inventory_detail.csb");
    auto alert_panel = dynamic_cast<ui::Layout*>(raw_node->getChildByName("Panel_1"));
    alert_panel->removeFromParent();

    //alert_panel->setBackGroundImage("main_UI_export_10_x4.png", ui::TextureResType::PLIST);
    //alert_panel->setBackGroundImageScale9Enabled(true);

    auto cb = [alert_panel](Ref*, ui::Widget::TouchEventType type) {
        if (type == ui::Widget::TouchEventType::ENDED)
        {
            alert_panel->removeFromParent();
        };
    };
    alert_panel->addTouchEventListener(cb);

    auto resource_name = dynamic_cast<ui::Text*>(alert_panel->getChildByName("resource_name"));
    std::string res_name = Ingredient::type_to_string(ing_type);
    resource_name->setString(res_name);

    auto resource_description = dynamic_cast<ui::Text*>(alert_panel->getChildByName("resource_description"));
    //TODO: load resource desc from json
    resource_description->setString("Grain is good to eat\nits a lot of fun to touch\ni could go for some right now");

    auto count_desc = dynamic_cast<ui::Text*>(alert_panel->getChildByName("count_desc"));
    auto count_lbl = dynamic_cast<ui::Text*>(alert_panel->getChildByName("count_lbl"));

    auto update_delay = 0.1f;

    alert_panel->schedule([count_lbl, building, ing_type](float) {
        int count = building->count_ingredients(ing_type);
        count_lbl->setString(std::to_string(count));
    }, update_delay, "alert_count_update");

    auto sell_btn = dynamic_cast<ui::Button*>(alert_panel->getChildByName("sell_btn"));
    load_default_button_textures(sell_btn);

    int coins_gained = 10;
    std::stringstream cost_ss;
    cost_ss << "Sell for " << coins_gained;
    sell_btn->setTitleText(cost_ss.str());
    sell_btn->addTouchEventListener([this, ing_type, coins_gained](Ref* touch, ui::Widget::TouchEventType type){
        if (type == ui::Widget::TouchEventType::ENDED)
        {
            mistIngredient& ingredients = this->building->ingredients;

            int num_sellable = map_get(ingredients, ing_type, 0);
            if (num_sellable != 0)
            {
                --ingredients[ing_type];
                BEATUP->add_total_coin(coins_gained);
                CCLOG("SELLING STUFF");
            }
        }
    });
    sell_btn->schedule([sell_btn, this, ing_type](float){
        mistIngredient& ingredients = this->building->ingredients;
        if (ingredients.empty()){
            sell_btn->setBright(false);
        }
        else if (this->building->count_ingredients(ing_type) == 0)
        {
            sell_btn->setBright(false);
        }
        else
        {
            sell_btn->setEnabled(true);
        }
    }, update_delay, "sell_btn_state_cb");


    auto move_btn = dynamic_cast<ui::Button*>(alert_panel->getChildByName("move_btn"));
    load_default_button_textures(move_btn);
    move_btn->schedule([move_btn, this, ing_type](float){
        mistIngredient& ingredients = this->building->ingredients;
        if (ingredients.empty()){
            move_btn->setBright(false);
        }
        else if (this->building->count_ingredients(ing_type) == 0)
        {
            move_btn->setBright(false);
        }
        else
        {
            move_btn->setEnabled(true);
        }
    }, update_delay, "move_btn_state_cb");

    move_btn->addTouchEventListener([this, ing_type](Ref* touch, ui::Widget::TouchEventType type){
        if (type == ui::Widget::TouchEventType::ENDED)
        {
            mistIngredient& ingredients = this->building->ingredients;
            if (ingredients.empty()){ return; }

            Animal animal("WorkshopWagon");
            animal.transfer_ingredients(
                this->building,
                BUILDUP->city->building_by_name("The Workshop"),
                ing_type,
                10);

        }
    });

    alert_panel->setPosition(this->get_center_pos());

    alert_panel->setName("inventory_detail_panel");

    return alert_panel;
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
        pEvent->stopPropagation();
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

void CharacterMenu::onKeyReleased(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event *pEvent)
{
    Director::getInstance()->popScene();
    pEvent->stopPropagation();
};

void CharacterMenu::onTouchEnded(Touch *touch, Event *event)
{
    GameLayer::onTouchEnded(touch, event);
    Director::getInstance()->popScene();
    event->stopPropagation();
};
