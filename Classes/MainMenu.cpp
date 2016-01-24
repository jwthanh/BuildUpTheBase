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
#include <cocos2d/cocos/editor-support/cocostudio/CCSGUIReader.h>
#include <cocos2d/cocos/editor-support/cocostudio/ActionTimeline/CSLoader.h>

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

    this->coins_lbl = Label::createWithTTF("Beat some people up!", TITLE_FONT, sx(this->menu_fontsize));
    this->coins_lbl->setVisible(false);
    this->coins_lbl->setPosition(Vec2(
        origin.x + visibleSize.width/2,
        origin.y + visibleSize.height 
    ));
    this->addChild(this->coins_lbl, 1);

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
        director->pushScene(TransitionZoomFlipAngular::create(0.25, MainMenu::beatup_scene));
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
    director->pushScene(TransitionZoomFlipAngular::create(0.25, scene));

    return true;
};

bool MainMenu::open_levelselect()
{
    auto scene = Scene::create();
    LevelSelectMenu* level_select = LevelSelectMenu::create();
    scene->addChild(level_select);

    auto director = Director::getInstance();
    director->pushScene(TransitionZoomFlipAngular::create(0.25, scene));
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
    director->pushScene(TransitionZoomFlipAngular::create(0.25, scene));
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

    this->coins_lbl = Label::createWithTTF("OPTIONS!", this->menu_font, this->menu_fontsize);
    this->coins_lbl->setPosition(Vec2(origin.x + visibleSize.width/2,
                origin.y + visibleSize.height - this->coins_lbl->getContentSize().height-sx(30)));
    this->addChild(this->coins_lbl, 1);

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
    director->pushScene(TransitionZoomFlipAngular::create(0.25, scene));
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

    this->coins_lbl = Label::createWithTTF(
        "Complete missions to unlock more levels",
        this->menu_font,
        this->menu_fontsize
    );
    this->coins_lbl->setPosition(
        Vec2(
            origin.x + visibleSize.width/2,
            origin.y + visibleSize.height - label->getContentSize().height-sx(30)
        )
    );
    this->coins_lbl->setTextColor(Color4B::WHITE);
    this->addChild(this->coins_lbl, 1);

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

    this->coins_lbl = Label::createWithTTF(
        "Rewards for completion!",
        this->menu_font,
        this->menu_fontsize
    );
    this->coins_lbl->setPosition(
        Vec2(
            origin.x + visibleSize.width/2,
            origin.y + visibleSize.height - this->coins_lbl->getContentSize().height-sx(30)
        )
    );
    this->coins_lbl->setTextColor(Color4B::BLUE);
    this->addChild(this->coins_lbl, 1);

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
        director->pushScene(TransitionZoomFlipAngular::create(0.25, MainMenu::beatup_scene));
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
        director->pushScene(TransitionZoomFlipAngular::create(0.25, MainMenu::beatup_scene));
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

    this->coins_lbl = Label::createWithTTF(
        "CAUTION: No way to undo this",
        this->menu_font,
        this->menu_fontsize
    );
    this->coins_lbl->setPosition(
        Vec2(
            origin.x + visibleSize.width/2,
            origin.y + visibleSize.height - this->coins_lbl->getContentSize().height-sx(30)
        )
    );
    this->coins_lbl->setTextColor(Color4B::WHITE);
    this->addChild(this->coins_lbl, 1);

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
        director->pushScene(TransitionZoomFlipAngular::create(0.25, MainMenu::beatup_scene));
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
    ss << "Buidling: " << this->building->name;
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

    this->coins_lbl = Label::createWithTTF(
        "Building Menu",
        this->menu_font,
        this->menu_fontsize
    );
    this->coins_lbl->setPosition(
        Vec2(
            origin.x + visibleSize.width/2,
            origin.y + visibleSize.height - this->coins_lbl->getContentSize().height-sx(30)
        )
    );
    this->coins_lbl->setTextColor(Color4B::WHITE);
    this->addChild(this->coins_lbl, 1);

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
        director->pushScene(TransitionZoomFlipAngular::create(0.25, scene));
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
        director->pushScene(TransitionZoomFlipAngular::create(0.25, MainMenu::beatup_scene));
    };
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
    ss << "Buidling: " << this->building->name;
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

    auto layout_param = ui::RelativeLayoutParameter::create();
    layout_param->setMargin(ui::Margin(0, 50.0f, 0, 0)); //supposed to be 50 units between each button, FIXME

    auto scroll = ui::ScrollView::create();
    scroll->setLayoutType(ui::Layout::Type::VERTICAL);

    scroll->setContentSize(Size(200, 400));
    scroll->setInnerContainerSize(Size(200, 800));
    scroll->setAnchorPoint(Vec2(0.5f, 0.5f));

    scroll->setPosition(this->get_center_pos());

    for (auto building : this->building->city->buildings)
    {
        auto button = ui::Button::create("shop_title.png", "", "", ui::TextureResType::PLIST);
        button->setLayoutParameter(layout_param->createCloneInstance());

        button->ignoreContentAdaptWithSize(false);
        button->setTitleText(building->name);
        button->setTitleFontSize(24);

        auto cb = [this, building](Ref*, ui::Widget::TouchEventType) {
            auto scene = Scene::create();
            BuildingMenu* building_menu = BuildingMenu::create(building);
            scene->addChild(building_menu);

            auto director = Director::getInstance();
            director->pushScene(TransitionZoomFlipAngular::create(0.25, scene));
        };
        button->addTouchEventListener(cb);

        resize_btn(button);
        scroll->addChild(button);
    }
    this->addChild(scroll);

    // add the label as a child to this layer
    this->addChild(label, 1);

    this->coins_lbl = Label::createWithTTF(
        "Building Menu",
        this->menu_font,
        this->menu_fontsize
    );
    this->coins_lbl->setPosition(
        Vec2(
            origin.x + visibleSize.width/2,
            origin.y + visibleSize.height - this->coins_lbl->getContentSize().height-sx(30)
        )
    );
    this->coins_lbl->setTextColor(Color4B::WHITE);
    this->addChild(this->coins_lbl, 1);
    return true;
};

void CityMenu::onKeyPressed(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event *pEvent)
{
    if(keyCode == EventKeyboard::KeyCode::KEY_SPACE 
            || keyCode == EventKeyboard::KeyCode::KEY_ENTER) 
    {
        auto director = Director::getInstance();
        director->pushScene(TransitionZoomFlipAngular::create(0.25, MainMenu::beatup_scene));
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
    ss << "Buidling: " << this->building->name;
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

    //create 9 sprites, three per row to go into a layout
    auto layout = ui::Layout::create();
    layout->setLayoutType(ui::Layout::Type::VERTICAL);
    layout->setContentSize(Size(300, 300));
    layout->setAnchorPoint(Vec2(0.5, 0.5));

    int index = 0;
    for (int i = 0; i < 3; i++) {
        auto inner_layout = ui::HBox::create();
        inner_layout->setContentSize(Size(300, 100));

        auto inst = CSLoader::getInstance();


        for (int j = 0; j < 3; j++) {
            index++;
            // auto scene = ui::Button::create("f_face_neutral.png", "", "", ui::TextureResType::PLIST);
            auto parent_scene = inst->createNode("editor/Node.csb");
            auto scene = parent_scene->getChildByName("Panel_1");
            scene->removeFromParent();
            CCLOG("scene width %f", scene->getContentSize().width);

            auto btn = scene->getChildByName("resource_btn");
            if (btn) {
                CCLOG("found button");

                auto casted = dynamic_cast<ui::Button*>(btn);
                if (casted) {
                    CCLOG("cast properly %i", index);
                    try {
                        casted->setTitleText(building->ingredients.at(index)->name);
                        CCLOG("set text properly ASDASDASD");
                    }
                    catch (std::out_of_range&) {
                        CCLOG("set text out of range error");
                    }
                }
            }

            inner_layout->addChild(scene);
        };

        layout->addChild(inner_layout);
    };
    layout->setPosition(this->get_center_pos());

    this->addChild(layout);

    // add the label as a child to this layer
    this->addChild(label, 1);

    this->coins_lbl = Label::createWithTTF(
        "Inventory Menu",
        this->menu_font,
        this->menu_fontsize
    );
    this->coins_lbl->setPosition(
        Vec2(
            origin.x + visibleSize.width/2,
            origin.y + visibleSize.height - this->coins_lbl->getContentSize().height-sx(30)
        )
    );
    this->coins_lbl->setTextColor(Color4B::WHITE);
    this->addChild(this->coins_lbl, 1);

    return true;
};

void InventoryMenu::onKeyPressed(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event *pEvent)
{
    if(keyCode == EventKeyboard::KeyCode::KEY_SPACE 
            || keyCode == EventKeyboard::KeyCode::KEY_ENTER) 
    {
        auto director = Director::getInstance();
        director->pushScene(TransitionZoomFlipAngular::create(0.25, MainMenu::beatup_scene));
    };
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

