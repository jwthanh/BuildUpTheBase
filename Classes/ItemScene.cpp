#include "ItemScene.h"

#include "base/CCEventDispatcher.h"
#include "base/CCEventListenerKeyboard.h"
#include "base/CCDirector.h"
#include "base/CCEventListenerTouch.h"

#include "ui/UIListView.h"
#include "ui/UIText.h"
#include "ui/UIButton.h"

#include "NodeBuilder.h"
#include "MiscUI.h"
#include "NuMenu.h"
#include "Item.h"
#include "GameLogic.h"
#include "HouseBuilding.h"
#include "Beatup.h"
#include "Util.h"

using namespace cocos2d;

cocos2d::ui::Layout* ItemScene::init_panel()
{
    auto items_scene_node = get_prebuilt_node_from_csb("editor/scenes/items_scene.csb");
    items_scene_node->removeFromParent();
    auto panel = dynamic_cast<cocos2d::ui::Layout*>(items_scene_node->getChildByName("panel"));
	panel->removeFromParent();
	this->addChild(panel);

	return panel;
};

void ItemScene::init_title_lbl(cocos2d::Node* panel, std::string title)
{
    //title
    auto title_lbl = dynamic_cast<ui::Text*>(panel->getChildByName("title_lbl"));
    set_aliasing(title_lbl);
    title_lbl->setString(title);

};

bool ItemScene::init()
{

#ifdef _WIN32
    FUNC_INIT_WIN32(ItemScene);
#else
    FUNC_INIT(ItemScene);
#endif

	auto panel = this->init_panel();

	this->init_title_lbl(panel, "Item Management");

    //items listview
    auto items_listview = dynamic_cast<ui::ListView*>(panel->getChildByName("items_listview"));

    auto item_detail_panel = dynamic_cast<ui::Layout*>(panel->getChildByName("item_detail"));
    auto item_name = dynamic_cast<ui::Text*>(item_detail_panel->getChildByName("item_name"));
    auto item_listview_description = dynamic_cast<ui::ListView*>(item_detail_panel->getChildByName("item_listview_description"));

    auto item_desc = dynamic_cast<ui::Text*>(item_listview_description->getChildByName("item_description"));
    item_desc->setTextAreaSize({ 375, 0 }); //hardcode width of textarea so that it wraps properly

    auto item_sell_btn = dynamic_cast<ui::Button*>(item_detail_panel->getChildByName("item_sell"));
    item_sell_btn->setVisible(false);
    load_default_button_textures(item_sell_btn);
    item_sell_btn->getTitleRenderer()->setTextColor(Color4B::WHITE);
    item_sell_btn->getTitleRenderer()->enableOutline(Color4B::BLACK, 2);
    set_aliasing(item_sell_btn);

    auto reset_item_detail_panel = [panel, item_name, item_desc, item_sell_btn, item_listview_description, items_listview](){
        item_name->setString("Item Details");
        const char* raw_description = R"foo(
Collect Items at The Dump with the help of Undead scavengers

This screen will show you more information about them.

You're able to sell them, and we're planning to have things like people who want items, appeasing gods, improving chances of getting better loot, and even equipping items in slots for new abilities.
)foo";
        auto default_description = raw_description;
        item_desc->setString(default_description);
        item_sell_btn->setVisible(false);

        items_listview->requestDoLayout();
    };

    reset_item_detail_panel();


    auto update_detail_panel_on_touch = [panel, item_name, item_desc, item_sell_btn, item_listview_description, reset_item_detail_panel, items_listview](NuItem* nuitem, spItem item) {
        item_name->setString(item->get_name());
        item_desc->setString(item->description);

        item_sell_btn->setVisible(true);
        item_sell_btn->addTouchEventListener([reset_item_detail_panel, nuitem, item, items_listview](Ref* sender, ui::Widget::TouchEventType type){
            if (type == ui::Widget::TouchEventType::ENDED)
            {
                items_listview->removeChild(nuitem->button);
                items_listview->removeChild(nuitem);
                BUILDUP->items.erase(std::remove(BUILDUP->items.begin(), BUILDUP->items.end(), item), BUILDUP->items.end());

                BEATUP->add_total_coin(item->get_effective_cost());

                reset_item_detail_panel();
                do_vibrate(16);
            }
        });

        //update listviews layout to account for different content height
        item_listview_description->requestDoLayout();
    };

    for (spItem item : BUILDUP->items) {
        auto nuitem = NuItem::create(items_listview);
        nuitem->set_title(item->get_name());
        nuitem->set_description(item->summary);

        nuitem->set_image(item->img_path, ui::Widget::TextureResType::LOCAL);

        res_count_t cost = item->get_effective_cost();
        std::string cost_str = beautify_double(cost);
        CCLOG("cost %f, beauty cost %s", cost, cost_str.c_str());
        nuitem->set_cost_lbl(cost_str);

        nuitem->button->addTouchEventListener([item, nuitem, update_detail_panel_on_touch](Ref* sender, ui::Widget::TouchEventType type){
            if (type == ui::Widget::TouchEventType::ENDED)
            {
                do_vibrate(16);
                update_detail_panel_on_touch(nuitem, item);
            }
        });
    };

    //back button
    auto back_btn = dynamic_cast<ui::Button*>(panel->getChildByName("back_btn"));
    Label* button_lbl = back_btn->getTitleRenderer();
    button_lbl->setTextColor(Color4B::WHITE);
    button_lbl->enableOutline(Color4B::BLACK, 2);

    back_btn->addTouchEventListener([](Ref* touch, ui::Widget::TouchEventType type){
        if (type == ui::Widget::TouchEventType::ENDED)
        {
            do_vibrate(16);
            auto director = Director::getInstance();
            director->popScene();
        }
    });
    load_default_button_textures(back_btn);

	return true;
};

cocos2d::Scene* ItemScene::createScene()
{
	auto scene = cocos2d::Scene::create();
	auto* layer = ItemScene::create();
	scene->addChild(layer);
	return scene;
};

bool AltarItemScene::init()
{

#ifdef _WIN32
    FUNC_INIT_WIN32(AltarItemScene);
#else
    FUNC_INIT(AltarItemScene);
#endif

	auto panel = this->init_panel();
	this->init_title_lbl(panel, "Item Altar");

    //items listview
    auto items_listview = dynamic_cast<ui::ListView*>(panel->getChildByName("items_listview"));

    auto item_detail_panel = dynamic_cast<ui::Layout*>(panel->getChildByName("item_detail"));
    auto item_name = dynamic_cast<ui::Text*>(item_detail_panel->getChildByName("item_name"));
    auto item_listview_description = dynamic_cast<ui::ListView*>(item_detail_panel->getChildByName("item_listview_description"));

    auto item_desc = dynamic_cast<ui::Text*>(item_listview_description->getChildByName("item_description"));
    item_desc->setTextAreaSize({ 375, 0 }); //hardcode width of textarea so that it wraps properly

    auto item_sell_btn = dynamic_cast<ui::Button*>(item_detail_panel->getChildByName("item_sell"));
    item_sell_btn->setVisible(false);
    load_default_button_textures(item_sell_btn);
    item_sell_btn->getTitleRenderer()->setTextColor(Color4B::WHITE);
    item_sell_btn->getTitleRenderer()->enableOutline(Color4B::BLACK, 2);
    set_aliasing(item_sell_btn);
    item_sell_btn->setTitleText("Place");

    auto reset_item_detail_panel = [panel, item_name, item_desc, item_sell_btn, item_listview_description, items_listview](){
        item_name->setString("Item");
        const char* raw_description = R"foo(
Place an item at the altar and the gods will see to that it gets strengthed.
)foo";
        auto default_description = raw_description;
        item_desc->setString(default_description);
        item_sell_btn->setVisible(false);

        items_listview->requestDoLayout();
    };

    reset_item_detail_panel();


    auto update_detail_panel_on_touch = [panel, item_name, item_desc, item_sell_btn, item_listview_description, reset_item_detail_panel, items_listview](NuItem* nuitem, spItem item) {
        item_name->setString(item->get_name());
        item_desc->setString(item->description);

        item_sell_btn->setVisible(true);
        item_sell_btn->addTouchEventListener([reset_item_detail_panel, nuitem, item, items_listview](Ref* sender, ui::Widget::TouchEventType type){
            if (type == ui::Widget::TouchEventType::ENDED)
            {
                item->level += 3;

                cocos2d::Director::getInstance()->popScene();
                do_vibrate(16);
            }
        });

        //update listviews layout to account for different content height
        item_listview_description->requestDoLayout();
    };

    for (spItem item : BUILDUP->items) {
        auto nuitem = NuItem::create(items_listview);
        nuitem->set_title(item->get_name());
        nuitem->set_description(item->summary);

        nuitem->set_image(item->img_path, ui::Widget::TextureResType::LOCAL);

        res_count_t cost = item->get_effective_cost();
        std::string cost_str = beautify_double(cost);
        CCLOG("cost %f, beauty cost %s", cost, cost_str.c_str());
        nuitem->set_cost_lbl(cost_str);

        nuitem->button->addTouchEventListener([item, nuitem, update_detail_panel_on_touch](Ref* sender, ui::Widget::TouchEventType type){
            if (type == ui::Widget::TouchEventType::ENDED)
            {
                do_vibrate(16);
                update_detail_panel_on_touch(nuitem, item);
            }
        });
    };

    //back button
    auto back_btn = dynamic_cast<ui::Button*>(panel->getChildByName("back_btn"));
    Label* button_lbl = back_btn->getTitleRenderer();
    button_lbl->setTextColor(Color4B::WHITE);
    button_lbl->enableOutline(Color4B::BLACK, 2);

    back_btn->addTouchEventListener([](Ref* touch, ui::Widget::TouchEventType type){
        if (type == ui::Widget::TouchEventType::ENDED)
        {
            do_vibrate(16);
            auto director = Director::getInstance();
            director->popScene();
        }
    });
    load_default_button_textures(back_btn);

	return true;
};

cocos2d::Scene* AltarItemScene::createScene()
{
	auto scene = cocos2d::Scene::create();
	auto* layer = AltarItemScene::create();
	scene->addChild(layer);
	return scene;
};

bool EquipItemScene::init()
{

#ifdef _WIN32
    FUNC_INIT_WIN32(EquipItemScene);
#else
    FUNC_INIT(EquipItemScene);
#endif

	auto panel = this->init_panel();

	this->init_title_lbl(panel, "Equip Item");

    //items listview
    auto items_listview = dynamic_cast<ui::ListView*>(panel->getChildByName("items_listview"));

    auto item_detail_panel = dynamic_cast<ui::Layout*>(panel->getChildByName("item_detail"));
    auto item_name = dynamic_cast<ui::Text*>(item_detail_panel->getChildByName("item_name"));
    auto item_listview_description = dynamic_cast<ui::ListView*>(item_detail_panel->getChildByName("item_listview_description"));

    auto item_desc = dynamic_cast<ui::Text*>(item_listview_description->getChildByName("item_description"));
    item_desc->setTextAreaSize({ 375, 0 }); //hardcode width of textarea so that it wraps properly

    auto item_equip_btn = dynamic_cast<ui::Button*>(item_detail_panel->getChildByName("item_sell"));
    item_equip_btn->setVisible(false);
    load_default_button_textures(item_equip_btn);
    item_equip_btn->getTitleRenderer()->setTextColor(Color4B::WHITE);
    item_equip_btn->getTitleRenderer()->enableOutline(Color4B::BLACK, 2);
    set_aliasing(item_equip_btn);
    item_equip_btn->setTitleText("Equip");

    auto reset_item_detail_panel = [panel, item_name, item_desc, item_equip_btn, item_listview_description, items_listview](){
        item_name->setString("Item");
        const char* raw_description = R"foo(
Equip and item and harness its power.
)foo";
        auto default_description = raw_description;
        item_desc->setString(default_description);
        item_equip_btn->setVisible(false);

        items_listview->requestDoLayout();
    };

    reset_item_detail_panel();


    auto update_detail_panel_on_touch = [panel, item_name, item_desc, item_equip_btn, item_listview_description, reset_item_detail_panel, items_listview](NuItem* nuitem, spItem item) {
        item_name->setString(item->get_name());
        item_desc->setString(item->description);

        item_equip_btn->setVisible(true);
        item_equip_btn->addTouchEventListener([reset_item_detail_panel, nuitem, item, items_listview](Ref* sender, ui::Widget::TouchEventType type){
            if (type == ui::Widget::TouchEventType::ENDED)
            {
                //item->level += 3;
				CCLOG("TODO equip item");

                //cocos2d::Director::getInstance()->popScene();
                do_vibrate(16);
            }
        });

        //update listviews layout to account for different content height
        item_listview_description->requestDoLayout();
    };

    for (spItem item : BUILDUP->items) {
        auto nuitem = NuItem::create(items_listview);
        nuitem->set_title(item->get_name());
        nuitem->set_description(item->summary);

        nuitem->set_image(item->img_path, ui::Widget::TextureResType::LOCAL);

        res_count_t cost = item->get_effective_cost();
        std::string cost_str = beautify_double(cost);
        CCLOG("cost %f, beauty cost %s", cost, cost_str.c_str());
        nuitem->set_cost_lbl(cost_str);

        nuitem->button->addTouchEventListener([item, nuitem, update_detail_panel_on_touch](Ref* sender, ui::Widget::TouchEventType type){
            if (type == ui::Widget::TouchEventType::ENDED)
            {
                do_vibrate(16);
                update_detail_panel_on_touch(nuitem, item);
            }
        });
    };

    //back button
    auto back_btn = dynamic_cast<ui::Button*>(panel->getChildByName("back_btn"));
    Label* button_lbl = back_btn->getTitleRenderer();
    button_lbl->setTextColor(Color4B::WHITE);
    button_lbl->enableOutline(Color4B::BLACK, 2);

    back_btn->addTouchEventListener([](Ref* touch, ui::Widget::TouchEventType type){
        if (type == ui::Widget::TouchEventType::ENDED)
        {
            do_vibrate(16);
            auto director = Director::getInstance();
            director->popScene();
        }
    });
    load_default_button_textures(back_btn);

	return true;
};

cocos2d::Scene* EquipItemScene::createScene()
{
	auto scene = cocos2d::Scene::create();
	auto* layer = EquipItemScene::create();
	scene->addChild(layer);
	return scene;
};
