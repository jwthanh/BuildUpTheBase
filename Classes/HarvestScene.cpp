#include "HarvestScene.h"
#include "Harvestable.h"

#include "GameLogic.h"
#include <cocos2d/cocos/editor-support/cocostudio/ActionTimeline/CSLoader.h>
#include "NuMenu.h"
#include "Beatup.h"
#include "Util.h"

USING_NS_CC;

bool HarvestScene::init()
{
    FUNC_INIT(HarvestScene);

    auto inst = CSLoader::getInstance();
    Node* harvest_scene_editor = inst->createNode("editor/scenes/harvest_scene.csb");

    auto shop_button = ui::Button::create(
            "shop_banner.png",
            "shop_banner_hili.png",
            "shop_banner.png",
            ui::TextureResType::PLIST
            );
    auto text_shop = ui::ImageView::create(
        "text_shop.png",
        ui::TextureResType::PLIST
        );
    text_shop->setPosition(Vec2(28, 17));
    shop_button->addChild(text_shop);
    shop_button->setScale(4);
    
    Node* shop_pos_node = harvest_scene_editor->getChildByName("shop_pos");
    // shop_pos_node->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
    shop_button->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
    shop_button->setPosition(shop_pos_node->getPosition());
    

    shop_button->addTouchEventListener([this](Ref*, ui::Widget::TouchEventType evt)
    {
        if (evt == ui::Widget::TouchEventType::ENDED) {
            auto scene = Scene::create();
            BuyBuildingsNuMenu* building_menu = BuyBuildingsNuMenu::create(GameLogic::getInstance()->beatup);
            scene->addChild(building_menu);

            auto director = Director::getInstance();
            director->pushScene(scene);
        };
    });

    this->addChild(shop_button);

    this->add_harvestable();

    this->scheduleUpdate();

    return true;
};

void HarvestScene::add_harvestable()
{
    auto harvestable = Harvestable::create(GameLogic::getInstance()->beatup);
    harvestable->setPosition(this->get_center_pos());
    harvestable->setName("harvestable");
    this->addChild(harvestable);
};

void HarvestScene::update(float dt)
{
    GameLogic::getInstance()->update(dt);

    if (!this->getChildByName("harvestable")) {
        this->add_harvestable();
    };
};

void HarvestScene::onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event)
{
    if(keyCode == EventKeyboard::KeyCode::KEY_BACK || keyCode == EventKeyboard::KeyCode::KEY_ESCAPE) 
    {
        this->quit(NULL);
        event->stopPropagation();
    }
    else if (keyCode == EventKeyboard::KeyCode::KEY_F5)
    {
        GameLogic::getInstance()->beatup->reload_resources();
    }
    else if (keyCode == EventKeyboard::KeyCode::KEY_GRAVE)
    {
        Director::getInstance()->popScene();
    }
};

