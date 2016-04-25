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
#include "Animal.h"

class Animal;
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
    this->create_inventory_pageview();


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
}

void BaseScene::onEnter()
{
    GameLayer::onEnter();
    auto building_pageview = dynamic_cast<ui::PageView*>(this->getChildByName("building_pageview"));
    if (building_pageview)
    {
        vsBuilding buildings = BUILDUP->city->buildings;
        auto find_cb = [](spBuilding building) {
            return building == BUILDUP->get_target_building();
        };
        int index = std::find_if(buildings.begin(),
                buildings.end(),
                find_cb) - buildings.begin();

        if (index > (int)buildings.size())
        {
            CCLOG("no building found, not moving building pageview");
        } else
        {
            
            building_pageview->ListView::scrollToItem(index, { 0, 0 }, { 0, 0 }, 0.0f);
        }
    }
};

void BaseScene::create_info_panel()
{
    auto building = BUILDUP->get_target_building();

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
        building_name->setString(BUILDUP->get_target_building()->name);
    };
    this->schedule(update_building_name, update_delay, "building_name_update");
    update_building_name(0);

    auto ing_count = dynamic_cast<ui::Text*>(building_info_panel->getChildByName("ingredient_count"));
    auto update_ing_count = [create_count, ing_count](float dt)
    {
        spBuilding building = BUILDUP->get_target_building();
        ing_count->setString(create_count("ING", building->count_ingredients()));
    };
    this->schedule(update_ing_count, update_delay, "ing_count_update");
    update_ing_count(0);

    auto pro_count = dynamic_cast<ui::Text*>(building_info_panel->getChildByName("product_count"));
    auto update_pro_count = [create_count, pro_count](float dt)
    {
        spBuilding building = BUILDUP->get_target_building();
        pro_count->setString(create_count("PRO", building->count_products()));
    };
    this->schedule(update_pro_count, update_delay, "pro_count_update");
    update_pro_count(0);

    auto wst_count = dynamic_cast<ui::Text*>(building_info_panel->getChildByName("waste_count"));
    auto update_wst_count = [create_count, wst_count](float dt)
    {
        spBuilding building = BUILDUP->get_target_building();
        wst_count->setString(create_count("WST", building->count_wastes()));
    };
    this->schedule(update_wst_count, update_delay, "wst_count_update");
    update_wst_count(0);

    auto harvester_count = dynamic_cast<ui::Text*>(building_info_panel->getChildByName("harvester_count"));
    auto update_harvester_count = [harvester_count](float dt)
    {
        spBuilding building = BUILDUP->get_target_building();

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
    auto building = BUILDUP->get_target_building();

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
                BUILDUP->set_target_building(building);
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

        auto update_panel = [this, building_pageview, building_panel, building](float dt) {
            if (BUILDUP->get_target_building() == building) {
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

void BaseScene::create_inventory_pageview()
{
    auto inst = CSLoader::getInstance();
    Node* harvest_scene_editor = inst->createNode("editor/scenes/base_scene.csb");
    ui::ListView* inventory_listview = dynamic_cast<ui::ListView*>(harvest_scene_editor->getChildByName("inventory_listview"));
    // inventory_listview->setClippingEnabled(true);
    inventory_listview->removeFromParent();
    this->addChild(inventory_listview);

    //FIXME not the farm, so the name in the update_listview always hits the true condition
    inventory_listview->setUserData(static_cast<void*>(BUILDUP->city->building_by_name("The Arena").get()));

    auto update_listview = [this, inst, inventory_listview](float dt)
    {
        if (static_cast<Building*>(inventory_listview->getUserData())->name != BUILDUP->get_target_building()->name)
        //if (true) //FIXME this doesnt need to get fully rebuilt every 0.1s does it?
        {
            inventory_listview->removeAllChildrenWithCleanup(true);
            inventory_listview->setUserData(static_cast<void*>(BUILDUP->get_target_building().get()));


            auto raw_node = inst->createNode("editor/buttons/inventory_button.csb");
            auto orig_item_panel = dynamic_cast<ui::Layout*>(raw_node->getChildByName("item_panel"));
            orig_item_panel->removeFromParent();

            typedef std::pair<Ingredient::SubType, res_count_t> mt;
            auto order_by_count = [](mt left, mt right)
            {
                return left.second > right.second;
            };


            std::vector<mt> type_vec;

            for (auto ts : Ingredient::type_map)
            {
                type_vec.push_back({ ts.first, map_get(BUILDUP->get_target_building()->ingredients, ts.first, 0) });
            }

            auto b = type_vec.begin();
            auto e = type_vec.end();
            std::sort(b, e, order_by_count);

            CCLOG("rebuild");

            for (auto type_to_str : type_vec)
            {
                Ingredient::SubType ing_type = type_to_str.first;
                //res_count_t count = mist.second;

                auto new_item_panel = dynamic_cast<ui::Layout*>(orig_item_panel->clone());
                if (BUILDUP->get_target_building()->ingredients[ing_type] <= 0)
                {
                    new_item_panel->setVisible(false);
                    new_item_panel->setSizePercent({ 0.0f, 0.0f });
                    //continue;
                }
                else
                {
                    new_item_panel->setVisible(true);
                    new_item_panel->setContentSize(orig_item_panel->getContentSize());
                }


                auto cb = [ing_type, this, new_item_panel](Ref* ref, ui::Widget::TouchEventType type) {

                    if (type == ui::Widget::TouchEventType::ENDED) {
                        CCLOG("touched a panel %i", ing_type);
                        auto alert = this->create_detail_alert(ing_type);
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

                    res_count_t count = BUILDUP->get_target_building()->count_ingredients(ing_type);
                    ss << count << " " << type_str;
                    auto item_lbl = dynamic_cast<ui::Text*>(new_item_panel->getChildByName("item_lbl"));
                    item_lbl->setString(ss.str());

                    if (count >= 1)
                    {
                        new_item_panel->setVisible(true);
                    }
                    else
                    {
                        new_item_panel->setVisible(false);
                    }
                };
                update_lbl_cb(0); //fire once immediately
                new_item_panel->schedule(update_lbl_cb, update_delay, "item_lbl_update");

                inventory_listview->addChild(new_item_panel);
            };

        }
    };
    inventory_listview->schedule(update_listview, 0.1f, "update_listview");
};

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
           BuildingNuMenu* building_menu = BuildingNuMenu::create(BUILDUP->get_target_building());
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
    else if (BUILDUP->get_target_building() != harvestable->building) {
        harvestable->removeFromParent();
    };

}

ui::Widget* BaseScene::create_detail_alert(Ingredient::SubType ing_type)
{
    //make sure one doesn't already exist first
    if (this->getChildByName("inventory_detail_panel")) {
        this->getChildByName("inventory_detail_panel")->removeFromParent();
    };

    auto inst = CSLoader::getInstance();
    auto raw_node = inst->createNode("editor/details/inventory_detail.csb");
    auto alert_panel = dynamic_cast<ui::Layout*>(raw_node->getChildByName("Panel_1"));
    alert_panel->removeFromParent();

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

    alert_panel->schedule([count_lbl, ing_type](float) {
        int count = BUILDUP->get_target_building()->count_ingredients(ing_type);
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
            mistIngredient& ingredients = BUILDUP->get_target_building()->ingredients;

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
        mistIngredient& ingredients = BUILDUP->get_target_building()->ingredients;
        if (ingredients.empty()){
            sell_btn->setBright(false);
        }
        else if (BUILDUP->get_target_building()->count_ingredients(ing_type) == 0)
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
        mistIngredient& ingredients = BUILDUP->get_target_building()->ingredients;
        if (ingredients.empty()){
            move_btn->setBright(false);
        }
        else if (BUILDUP->get_target_building()->count_ingredients(ing_type) == 0)
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
            mistIngredient& ingredients = BUILDUP->get_target_building()->ingredients;
            if (ingredients.empty()){ return; }

            Animal animal("WorkshopWagon");
            animal.transfer_ingredients(
                BUILDUP->get_target_building(),
                BUILDUP->city->building_by_name("The Workshop"),
                ing_type,
                10);

        }
    });

    alert_panel->setPosition(this->get_center_pos());

    alert_panel->setName("inventory_detail_panel");

    return alert_panel;
};

void HarvestScene::add_harvestable()
{
    Harvestable* harvestable;
    this->target_recipe = NULL; //TODO move this somewhere else

    if (BUILDUP->get_target_building()->name == "The Mine") {
        harvestable = MiningHarvestable::create();
    } else if (BUILDUP->get_target_building()->name == "The Workshop") {
        this->target_recipe = BUILDUP->city->building_by_name("The Farm")->data->get_recipe("loaf_recipe");
        harvestable = CraftingHarvestable::create(this->target_recipe);
    } else if (BUILDUP->get_target_building()->name == "The Arena") {
        harvestable = FightingHarvestable::create();
    } else if (BUILDUP->get_target_building()->name == "The Underscape") {
        harvestable = UndeadHarvestable::create();
    } else if (BUILDUP->get_target_building()->name == "The Farm") {
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
