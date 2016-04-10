#include "HarvestScene.h"
#include "Harvestable.h"

#include "GameLogic.h"
#include "cocostudio/ActionTimeline/CSLoader.h"
#include "NuMenu.h"
#include "Beatup.h"
#include "Util.h"
#include "MainMenu.h"
#include "Network.h"

USING_NS_CC;

bool HarvestScene::init()
{
    FUNC_INIT(HarvestScene);

    ui::Button* shop_button = this->create_shop_button();
    this->addChild(shop_button);

    ui::Button* city_button = this->create_city_button();
    this->addChild(city_button);

    ui::Button* detail_button = this->create_detail_button();
    this->addChild(detail_button);

    ui::Layout* info_panel = this->create_info_panel();
    this->addChild(info_panel);

    ui::Layout* player_info_panel = this->create_player_info_panel();
    this->addChild(player_info_panel);

    this->add_harvestable();

    // this->scheduleUpdate(); //something else is already calling
    // HarvestScene::update so dont need to do it manually

    return true;
};

void HarvestScene::add_harvestable()
{
    Harvestable* harvestable;
    if (BUILDUP->target_building->name == "The Mine") {
        harvestable = MiningHarvestable::create();
    } else if (BUILDUP->target_building->name == "The Workshop") {
        harvestable = CraftingHarvestable::create();
    } else {
        harvestable = Harvestable::create();
    };

    harvestable->setPosition(this->get_center_pos());
    harvestable->setName("harvestable");

    harvestable->setScale(harvestable->getScale()*4); //4*4 scale now

    this->addChild(harvestable);
};

void HarvestScene::update(float dt)
{
    GameLogic::getInstance()->update(dt);

    auto harvestable = dynamic_cast<Harvestable*>(this->getChildByName("harvestable"));
    if (!harvestable) {
        this->add_harvestable();
    } else if (BUILDUP->target_building != harvestable->building) {
        harvestable->removeFromParent();
    };
};

void HarvestScene::onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event)
{
    if(keyCode == EventKeyboard::KeyCode::KEY_BACK || keyCode == EventKeyboard::KeyCode::KEY_ESCAPE) 
    {
        this->quit(NULL);
        event->stopPropagation();
    }
    else if(keyCode == EventKeyboard::KeyCode::KEY_F1) 
    {
        auto glView = Director::getInstance()->getOpenGLView();
        glView->setFrameSize(1920, 1080);
        glView->setDesignResolutionSize(1920, 1080, ResolutionPolicy::SHOW_ALL);
    }
    else if(keyCode == EventKeyboard::KeyCode::KEY_F2) 
    {
        auto glView = Director::getInstance()->getOpenGLView();
        glView->setFrameSize(960, 640);
        glView->setDesignResolutionSize(960, 640, ResolutionPolicy::SHOW_ALL);
    }
    else if (keyCode == EventKeyboard::KeyCode::KEY_F5)
    {
        BEATUP->reload_resources();
    }
    else if (keyCode == EventKeyboard::KeyCode::KEY_GRAVE)
    {
        Director::getInstance()->popScene();
    }
    else if (keyCode == EventKeyboard::KeyCode::KEY_SPACE)
    {
        // NetworkConsole::set_position(this->getChildByName("harvestable"));

        auto harvestable = (Harvestable*)this->getChildByName("harvestable");

    }
};

ui::Layout* HarvestScene::create_info_panel()
{
    auto building = BUILDUP->target_building;

    auto inst = CSLoader::getInstance();
    Node* harvest_scene_editor = inst->createNode("editor/scenes/harvest_scene.csb");

    ui::Layout* building_info_panel = dynamic_cast<ui::Layout*>(harvest_scene_editor->getChildByName("building_info_panel"));
    building_info_panel->removeFromParent();

    auto create_count = [](std::string prefix, int count) {
        std::stringstream ss;
        ss << prefix << ": " << count;
        return ss.str();
    };

    const float update_delay = 0.1f;

    auto building_name = dynamic_cast<ui::Text*>(building_info_panel->getChildByName("building_name"));
    auto update_building_name = [building_name](float dt){
        building_name->setString(BUILDUP->target_building->name);
    };
    this->schedule(update_building_name, update_delay, "building_name_update");
    update_building_name(0);

    auto ing_count = dynamic_cast<ui::Text*>(building_info_panel->getChildByName("ingredient_count"));
    auto update_ing_count = [create_count, ing_count](float dt)
    {
        spBuilding building = BUILDUP->target_building;
        ing_count->setString(create_count("ING", building->ingredients.size()));
    };
    this->schedule(update_ing_count, update_delay, "ing_count_update");
    update_ing_count(0);

    auto pro_count = dynamic_cast<ui::Text*>(building_info_panel->getChildByName("product_count"));
    auto update_pro_count = [create_count, pro_count](float dt)
    {
        spBuilding building = BUILDUP->target_building;
        pro_count->setString(create_count("PRO", building->products.size()));
    };
    this->schedule(update_pro_count, update_delay, "pro_count_update");
    update_pro_count(0);

    auto wst_count = dynamic_cast<ui::Text*>(building_info_panel->getChildByName("waste_count"));
    auto update_wst_count = [create_count, wst_count](float dt)
    {
        spBuilding building = BUILDUP->target_building;
        wst_count->setString(create_count("WST", building->wastes.size()));
    };
    this->schedule(update_wst_count, update_delay, "wst_count_update");
    update_wst_count(0);

    auto harvester_count = dynamic_cast<ui::Text*>(building_info_panel->getChildByName("harvester_count"));
    auto update_harvester_count = [harvester_count](float dt)
    {
        spBuilding building = BUILDUP->target_building;

        std::stringstream ss;
        ss << "Robo-harvesters: " << building->harvesters.size();
        harvester_count->setString(ss.str());
    };
    this->schedule(update_harvester_count, update_delay, "harvester_count_update");
    update_harvester_count(0);

    building_info_panel->addTouchEventListener([](Ref*, ui::Widget::TouchEventType evt_type)
    {
        if (evt_type == ui::Widget::TouchEventType::ENDED)
        {
            GameDirector::switch_to_inventory_menu();
        }
    });


    return building_info_panel;
};

ui::Layout* HarvestScene::create_player_info_panel()
{
    auto building = BUILDUP->target_building;

    auto inst = CSLoader::getInstance();
    Node* harvest_scene_editor = inst->createNode("editor/scenes/harvest_scene.csb");

    ui::Layout* player_info_panel = dynamic_cast<ui::Layout*>(harvest_scene_editor->getChildByName("player_info_panel"));
    player_info_panel->removeFromParent();

    auto create_count = [](std::string prefix, int count) {
        std::stringstream ss;
        ss << prefix << ": " << count;
        return ss.str();
    };

    const float update_delay = 0.1f;

    auto player_gold_lbl = dynamic_cast<ui::Text*>(player_info_panel->getChildByName("player_gold_lbl"));
    auto update_gold_lbl = [player_gold_lbl](float dt){
        std::stringstream coin_ss;
        auto gold = BEATUP->get_total_coins();
        coin_ss << "You have " << gold << " coins";
        std::string coin_msg = coin_ss.str();
        player_gold_lbl->setString(coin_msg);
    };
    this->schedule(update_gold_lbl, update_delay, "player_gold_lbl_update");
    update_gold_lbl(0);

    return player_info_panel;
};

ui::Button* HarvestScene::create_shop_button()
{

    auto inst = CSLoader::getInstance();
    Node* harvest_scene_editor = inst->createNode("editor/scenes/harvest_scene.csb");

    auto shop_button = ui::Button::create(
        "shop_banner.png",
        "shop_banner_hili.png",
        "shop_banner.png",
        ui::TextureResType::PLIST
    );

    auto shop_text_img = ui::ImageView::create(
        "text_shop.png",
        ui::TextureResType::PLIST
    );

    shop_text_img->setPosition(Vec2(28, 17));
    shop_button->addChild(shop_text_img);
    shop_button->setScale(4);

    Node* shop_pos_node = harvest_scene_editor->getChildByName("shop_pos");
    shop_button->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
    shop_button->setPosition(shop_pos_node->getPosition());

    shop_button->addTouchEventListener([](Ref*, ui::Widget::TouchEventType evt)
    {
        if (evt == ui::Widget::TouchEventType::ENDED) {
            auto scene = Scene::create();
            BuyBuildingsNuMenu* building_menu = BuyBuildingsNuMenu::create();
            scene->addChild(building_menu);

            auto director = Director::getInstance();
            director->pushScene(scene);
        };
    });

    return shop_button;
};

ui::Button* HarvestScene::create_city_button()
{

    auto inst = CSLoader::getInstance();
    Node* harvest_scene_editor = inst->createNode("editor/scenes/harvest_scene.csb");

    auto city_button = ui::Button::create(
        "shop_banner.png",
        "shop_banner_hili.png",
        "shop_banner.png",
        ui::TextureResType::PLIST
    );

    auto shop_text_img = ui::ImageView::create(
        "text_city.png",
        ui::TextureResType::PLIST
    );

    shop_text_img->setPosition(Vec2(28, 17));
    city_button->addChild(shop_text_img);
    city_button->setScale(4);

    Node* shop_pos_node = harvest_scene_editor->getChildByName("city_pos");
    city_button->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
    city_button->setPosition(shop_pos_node->getPosition());

    city_button->addTouchEventListener([](Ref*, ui::Widget::TouchEventType evt)
    {
        if (evt == ui::Widget::TouchEventType::ENDED) {
            auto scene = Scene::create();
            CityMenu* building_menu = CityMenu::create();
            scene->addChild(building_menu);

            auto director = Director::getInstance();
            director->pushScene(scene);
        };
    });

    return city_button;
};

ui::Button* HarvestScene::create_detail_button()
{

    auto inst = CSLoader::getInstance();
    Node* harvest_scene_editor = inst->createNode("editor/scenes/harvest_scene.csb");

    auto detail_button = ui::Button::create(
        "shop_banner.png",
        "shop_banner_hili.png",
        "shop_banner.png",
        ui::TextureResType::PLIST
    );

    auto shop_text_img = ui::Text::create(
        "detail",
        "Arial",
        10.0f
    );

    shop_text_img->setPosition(Vec2(28, 17));
    detail_button->addChild(shop_text_img);
    detail_button->setScale(4);

    Node* shop_pos_node = harvest_scene_editor->getChildByName("detail_pos");
    detail_button->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
    detail_button->setPosition(shop_pos_node->getPosition());

    detail_button->addTouchEventListener([](Ref*, ui::Widget::TouchEventType evt)
    {
        if (evt == ui::Widget::TouchEventType::ENDED) {
           auto scene = Scene::create();
           BuildingNuMenu* building_menu = BuildingNuMenu::create(BUILDUP->target_building);
           scene->addChild(building_menu);

           auto director = Director::getInstance();
           director->pushScene(scene);
        };
    });

    return detail_button;
};
