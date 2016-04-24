#include "HarvestScene.h"
#include "Harvestable.h"

#include "GameLogic.h"
#include "cocostudio/ActionTimeline/CSLoader.h"
#include "NuMenu.h"
#include "Beatup.h"
#include "Util.h"
#include "MainMenu.h"
#include "Network.h"
#include "StaticData.h"
#include "Recipe.h"
#include "attribute.h"
#include "FShake.h"
#include "MiscUI.h"

USING_NS_CC;

void BaseScene::create_side_buttons()
{
    this->create_shop_button();
    this->create_city_button();
    this->create_detail_button();
}

bool BaseScene::init()
{
    FUNC_INIT(BaseScene);


    this->create_side_buttons();

    this->create_info_panel();
    this->create_player_info_panel();

    this->create_building_scrollview();

    return true;
};

void BaseScene::update(float dt)
{
    GameLayer::update(dt);

    GameLogic::getInstance()->update(dt);

};

void BaseScene::onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event)
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
        auto harvestable = (Harvestable*)this->getChildByName("harvestable");
        harvestable->shatter();

    }
};

void BaseScene::create_info_panel()
{
    auto building = BUILDUP->target_building;

    auto inst = CSLoader::getInstance();
    Node* harvest_scene_editor = inst->createNode("editor/scenes/base_scene.csb");

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
        ing_count->setString(create_count("ING", building->count_ingredients()));
    };
    this->schedule(update_ing_count, update_delay, "ing_count_update");
    update_ing_count(0);

    auto pro_count = dynamic_cast<ui::Text*>(building_info_panel->getChildByName("product_count"));
    auto update_pro_count = [create_count, pro_count](float dt)
    {
        spBuilding building = BUILDUP->target_building;
        pro_count->setString(create_count("PRO", building->count_products()));
    };
    this->schedule(update_pro_count, update_delay, "pro_count_update");
    update_pro_count(0);

    auto wst_count = dynamic_cast<ui::Text*>(building_info_panel->getChildByName("waste_count"));
    auto update_wst_count = [create_count, wst_count](float dt)
    {
        spBuilding building = BUILDUP->target_building;
        wst_count->setString(create_count("WST", building->count_wastes()));
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


    this->addChild(building_info_panel);
};

void BaseScene::create_player_info_panel()
{
    auto building = BUILDUP->target_building;

    auto inst = CSLoader::getInstance();
    Node* harvest_scene_editor = inst->createNode("editor/scenes/base_scene.csb");

    ui::Layout* player_info_panel = dynamic_cast<ui::Layout*>(harvest_scene_editor->getChildByName("player_info_panel"));
    player_info_panel->removeFromParent();

    player_info_panel->addTouchEventListener([player_info_panel, this](Ref* target, ui::Widget::TouchEventType type)
    {
        if (type == ui::Widget::TouchEventType::ENDED)
        {
            auto gold = BEATUP->get_total_coins();
            int heal_cost = 10;
            if (gold > heal_cost)
            {
                auto health = BUILDUP->fighter->attrs->health;
                if (health->is_full() == false)
                {
                    health->add_to_current_val(5);
                    BEATUP->add_total_coin(-heal_cost);
                }
            }
            else
            {
                auto player_gold_lbl = dynamic_cast<ui::Text*>(player_info_panel->getChildByName("player_gold_lbl"));
                player_gold_lbl->runAction(FShake::actionWithDuration(0.05f, 10));
            }
        }
    });

    auto create_count = [](std::string prefix, int count) {
        std::stringstream ss;
        ss << prefix << ": " << count;
        return ss.str();
    };

    const float update_delay = 0.1f;

    auto player_gold_lbl = dynamic_cast<ui::Text*>(player_info_panel->getChildByName("player_gold_lbl"));
    auto player_hp_lbl = dynamic_cast<ui::Text*>(player_info_panel->getChildByName("player_hp_lbl"));
    auto update_gold_lbl = [player_gold_lbl, player_hp_lbl](float dt){
        //set gold
        std::stringstream coin_ss;
        auto gold = BEATUP->get_total_coins();
        coin_ss << "You have " << beautify_double(gold) << " coins";
        std::string coin_msg = coin_ss.str();
        player_gold_lbl->setString(coin_msg);

        //set hp
        std::stringstream hp_ss;
        auto hp = BUILDUP->fighter->attrs->health;
        hp_ss << "HP: " << hp->current_val << "/" << hp->max_val;
        player_hp_lbl->setString(hp_ss.str());

        if (hp->is_empty()) {
            player_hp_lbl->setTextColor(Color4B::RED);
        } else {
            player_hp_lbl->setTextColor(Color4B::WHITE);
        }

    };
    this->schedule(update_gold_lbl, update_delay, "player_gold_lbl_update");
    update_gold_lbl(0);

    this->addChild(player_info_panel);
};

void BaseScene::create_building_scrollview()
{
    auto inst = CSLoader::getInstance();
    Node* harvest_scene_editor = inst->createNode("editor/scenes/base_scene.csb");

    ui::PageView* building_pageview = dynamic_cast<ui::PageView*>(harvest_scene_editor->getChildByName("building_pageview"));
    building_pageview->setClippingEnabled(true);
    building_pageview->removeFromParent();

    auto city_scene = inst->createNode("editor/scenes/city_scene.csb");

    float update_delay = 0.1f;

    auto create_count = [](std::string prefix, int count) {
        std::stringstream ss;
        ss << prefix << ": " << count;
        return ss.str();
    };

    for (auto building : BUILDUP->city->buildings)
    {

        auto node = city_scene->getChildByName(building->name);
        auto building_panel = dynamic_cast<ui::Layout*>(node->getChildByName("building_panel"));
        building_panel->removeFromParent();
        building_pageview->addPage(building_panel);
        int page_index = building_pageview->getItems().size() - 1;

        auto change_target_page = [this, building_pageview, building, page_index](float dt)
        {
            if (building_pageview->getCurrentPageIndex() == page_index)
            {
                BUILDUP->target_building = building;
            }
        };
        building_panel->schedule(change_target_page, 0.5f, "update_target");

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
                   auto scene = Scene::create();
                   BuildingNuMenu* building_menu = BuildingNuMenu::create(building);
                   scene->addChild(building_menu);
                   Director::getInstance()->pushScene(scene);
            };
        };
        building_panel->addTouchEventListener(cb);

        auto update_panel = [this, building_panel, building](float dt) {
            if (BUILDUP->target_building == building) {
                building_panel->setBackGroundColor(Color3B::BLUE);
            }
            else {
                building_panel->setBackGroundColor(Color3B(150, 200, 255));
            }
        };
        update_panel(0);
        building_panel->schedule(update_panel, update_delay, "update_panel");

    }


    this->addChild(building_pageview);
}

void BaseScene::create_shop_button()
{

    auto inst = CSLoader::getInstance();
    Node* harvest_scene_editor = inst->createNode("editor/scenes/base_scene.csb");

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

    this->addChild(shop_button);
};

void BaseScene::create_city_button()
{

    auto inst = CSLoader::getInstance();
    Node* harvest_scene_editor = inst->createNode("editor/scenes/base_scene.csb");

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

    this->addChild(city_button);
};

void BaseScene::create_detail_button()
{

    auto inst = CSLoader::getInstance();
    Node* harvest_scene_editor = inst->createNode("editor/scenes/base_scene.csb");

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

    this->addChild(detail_button);
};

bool HarvestScene::init()
{
    BaseScene::init();

    this->target_recipe = NULL;
    this->create_recipe_lbl();

    this->add_harvestable();

    return true;

}

void HarvestScene::update(float dt)
{
    BaseScene::update(dt);

    if (this->target_recipe != NULL)
    {
        this->recipe_lbl->setString("Current recipe: " + this->target_recipe->name);
    }
    else
    {
        this->recipe_lbl->setString("");
    }

    auto harvestable = dynamic_cast<Harvestable*>(this->getChildByName("harvestable"));
    if (!harvestable) {
        this->add_harvestable();
    }
    else if (BUILDUP->target_building != harvestable->building) {
        harvestable->removeFromParent();
    };

}

void HarvestScene::add_harvestable()
{
    Harvestable* harvestable;
    this->target_recipe = NULL; //TODO move this somewhere else

    if (BUILDUP->target_building->name == "The Mine") {
        harvestable = MiningHarvestable::create();
    } else if (BUILDUP->target_building->name == "The Workshop") {
        this->target_recipe = BUILDUP->city->building_by_name("The Farm")->data->get_recipe("loaf_recipe");
        harvestable = CraftingHarvestable::create(this->target_recipe);
    } else if (BUILDUP->target_building->name == "The Arena") {
        harvestable = FightingHarvestable::create();
    } else if (BUILDUP->target_building->name == "The Underscape") {
        harvestable = UndeadHarvestable::create();
    } else if (BUILDUP->target_building->name == "The Farm") {
        harvestable = FarmingHarvestable::create();
    } else {
        harvestable = Harvestable::create();
    };

    this->addChild(harvestable);
};

void HarvestScene::create_recipe_lbl()
{
    auto inst = CSLoader::getInstance();
    Node* harvest_scene_editor = inst->createNode("editor/scenes/base_scene.csb");
    this->recipe_lbl = dynamic_cast<ui::Text*>(harvest_scene_editor->getChildByName("recipe_lbl"));
    this->recipe_lbl->removeFromParent();
    this->recipe_lbl->setString("");
    this->addChild(recipe_lbl);
}
