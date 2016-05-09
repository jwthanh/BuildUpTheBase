#include "MainMenu.h"

#include <algorithm>

#include "BaseMenu.h"
#include "Beatup.h"
#include "ui/CocosGUI.h"
#include "SoundEngine.h"
#include "DataManager.h"
#include "Util.h"
#include <editor-support/cocostudio/CCSGUIReader.h>
#include <editor-support/cocostudio/ActionTimeline/CSLoader.h>
#include "attribute.h"
#include "NuMenu.h"
#include "GameLogic.h"
#include "StaticData.h"
#include "MiscUI.h"
#include "Fighter.h"
#include "attribute_container.h"

#include "HouseBuilding.h"
#include "combat.h"

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
    }
    else if(keyCode == EventKeyboard::KeyCode::KEY_L ) 
    {
    }
    else if(keyCode == EventKeyboard::KeyCode::KEY_O ) 
    {
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
        if (this->menu_state == ChangeTarget) { target_btn->setTitleText("Open"); }
        else { target_btn->setTitleText("Target"); }
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


       auto cb = [this, building](Ref* target, ui::Widget::TouchEventType event) {
           if (event == ui::Widget::TouchEventType::ENDED)
           {
               auto director = Director::getInstance();
               if (this->menu_state == OpenMenu) {
                   auto scene = Scene::create();
                   BuildingNuMenu* building_menu = BuildingNuMenu::create(building);
                   scene->addChild(building_menu);
                   director->pushScene(scene);
               } else {
                   //change target building and exit menu
                   BUILDUP->set_target_building(building);
                   this->menu_state = OpenMenu;
                   director->popScene();
               };
           };
       };
       building_panel->addTouchEventListener(cb);

       auto update_panel = [this, building_panel, building](float dt) {
           if (BUILDUP->get_target_building() == building) {
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
