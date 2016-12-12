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
#include "Equipment.h"

using namespace cocos2d;

void ItemScene::init_children()
{
    this->panel = this->init_panel();

    this->init_title_lbl(panel, this->get_scene_title());

    //items listview
    this->items_listview = dynamic_cast<ui::ListView*>(panel->getChildByName("items_listview"));

    this->item_detail_panel = dynamic_cast<ui::Layout*>(panel->getChildByName("item_detail"));
    this->item_name = dynamic_cast<ui::Text*>(item_detail_panel->getChildByName("item_name"));
    this->item_listview_description = dynamic_cast<ui::ListView*>(item_detail_panel->getChildByName("item_listview_description"));

    this->item_desc = dynamic_cast<ui::Text*>(item_listview_description->getChildByName("item_description"));
    item_desc->setTextAreaSize({ 375, 0 }); //hardcode width of textarea so that it wraps properly

    this->item_sell_btn = this->init_sell_btn(item_detail_panel);

    this->init_back_btn(this->panel);

    this->reset_detail_panel();

};

void ItemScene::init_callbacks()
{
    auto update_detail_panel_on_touch = [this](NuItem* nuitem, spItem item) {
        this->item_name->setString(item->get_name());
        this->item_desc->setString(item->description);

        this->item_sell_btn->setVisible(true);
        this->item_sell_btn->addTouchEventListener([this, nuitem, item](Ref* sender, ui::Widget::TouchEventType type){
            if (type == ui::Widget::TouchEventType::ENDED)
            {
                this->items_listview->removeChild(nuitem->button);
                this->items_listview->removeChild(nuitem);

                BUILDUP->items.erase(
                    std::remove(BUILDUP->items.begin(), BUILDUP->items.end(), item),
                    BUILDUP->items.end()
                );

                BEATUP->add_total_coin(item->get_effective_cost());

                this->reset_detail_panel();
                do_vibrate(16);
            }
        });

        //update listviews layout to account for different content height
        this->item_listview_description->requestDoLayout();
    };

    for (spItem item : this->get_items()) {
        NuItem* nuitem = NuItem::create(this->items_listview);
        nuitem->set_title(item->get_name());
        nuitem->set_description(item->summary);

        nuitem->set_image(item->img_path, ui::Widget::TextureResType::LOCAL);

        res_count_t cost = item->get_effective_cost();
        std::string cost_str = beautify_double(cost);
        nuitem->set_cost_lbl(cost_str);

        nuitem->button->addTouchEventListener([item, nuitem, update_detail_panel_on_touch](Ref* sender, ui::Widget::TouchEventType type){
            if (type == ui::Widget::TouchEventType::ENDED)
            {
                do_vibrate(16);
                update_detail_panel_on_touch(nuitem, item);
            }
        });
    };

};

void ItemScene::reset_detail_panel()
{
    this->item_name->setString(this->get_default_detail_panel_title());
    this->item_desc->setString(this->get_default_detail_panel_description());
    this->item_sell_btn->setVisible(false);

    this->items_listview->requestDoLayout();

    //TODO fix the detail panel not being scrollable after resetting
};

vsItem ItemScene::get_items()
{
    vsItem filtered_items;
    for (auto& item : BUILDUP->items)
    {
        if (item->slot_type == this->filtered_slot_type)
        {
            filtered_items.push_back(item);
        }
    }
    return filtered_items;
};

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

void ItemScene::init_back_btn(cocos2d::Node* panel)
{
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
};

cocos2d::ui::Button* ItemScene::init_sell_btn(cocos2d::Node* item_detail_panel)
{
    auto item_sell_btn = dynamic_cast<ui::Button*>(item_detail_panel->getChildByName("item_sell"));
    item_sell_btn->setVisible(false);
    load_default_button_textures(item_sell_btn);
    item_sell_btn->getTitleRenderer()->setTextColor(Color4B::WHITE);
    item_sell_btn->getTitleRenderer()->enableOutline(Color4B::BLACK, 2);
    set_aliasing(item_sell_btn);

    item_sell_btn->setTitleText(this->get_sell_btn_text());

    return item_sell_btn;
};

const std::string& ItemScene::get_scene_title()
{
    static std::string scene_title = "Item Management";
    return scene_title;
};

const std::string& ItemScene::get_default_detail_panel_title()
{
    static std::string default_title = "Item Detail";
    return default_title;
};

const std::string& ItemScene::get_default_detail_panel_description()
{
    static std::string default_desc = "Collect Items at The Dump with the help of Undead scavengers\n\nThis screen will show you more information about them.\n\nYou're able to sell them, and we're planning to have things like people who want items, appeasing gods, improving chances of getting better loot, and even equipping items in slots for new abilities.";
    return default_desc;
};

const std::string& ItemScene::get_sell_btn_text()
{
    static std::string default_desc = "Sell";
    return default_desc;
}

ItemScene* ItemScene::create(ItemSlotType slot_type)
{
    ItemScene* pRet = new(std::nothrow) ItemScene();
    if (pRet && pRet->init(slot_type))
    {
        pRet->autorelease();
        return pRet;
    }
    else
    {
        delete pRet;
        pRet = nullptr;
        return nullptr;
    }
};

bool ItemScene::init(ItemSlotType slot_type)
{
    this->filtered_slot_type = slot_type;

#ifdef _WIN32
    FUNC_INIT_WIN32(ItemScene);
#else
    FUNC_INIT(ItemScene);
#endif

    this->init_children();
    this->init_callbacks();

    return true;
};

cocos2d::Scene* ItemScene::createScene(ItemSlotType slot_type)
{
    auto scene = cocos2d::Scene::create();
    auto* layer = ItemScene::create(slot_type);
    scene->addChild(layer);
    return scene;
};

void AltarItemScene::init_callbacks()
{
    auto update_detail_panel_on_touch = [this](NuItem* nuitem, spItem item) {
        this->item_name->setString(item->get_name());
        this->item_desc->setString(item->description);

        this->item_sell_btn->setVisible(true);
        this->item_sell_btn->addTouchEventListener([item](Ref* sender, ui::Widget::TouchEventType type){
            if (type == ui::Widget::TouchEventType::ENDED)
            {
                item->level += 3;

                cocos2d::Director::getInstance()->popScene();
                do_vibrate(16);
            }
        });

        //update listviews layout to account for different content height
        this->item_listview_description->requestDoLayout();
    };

    for (spItem item : this->get_items()) {
        auto nuitem = NuItem::create(items_listview);
        nuitem->set_title(item->get_name());
        nuitem->set_description(item->summary);

        nuitem->set_image(item->img_path, ui::Widget::TextureResType::LOCAL);

        res_count_t cost = item->get_effective_cost();
        std::string cost_str = beautify_double(cost);
        nuitem->set_cost_lbl(cost_str);

        nuitem->button->addTouchEventListener([item, nuitem, update_detail_panel_on_touch](Ref* sender, ui::Widget::TouchEventType type){
            if (type == ui::Widget::TouchEventType::ENDED)
            {
                do_vibrate(16);
                update_detail_panel_on_touch(nuitem, item);
            }
        });
    };

    this->init_back_btn(panel);
};
const std::string& AltarItemScene::get_scene_title()
{
    static std::string scene_title = "Item Altar";
    return scene_title;
};

const std::string& AltarItemScene::get_default_detail_panel_title()
{
    static std::string default_title = "Item";
    return default_title;
};

const std::string& AltarItemScene::get_default_detail_panel_description()
{
    static std::string default_desc = "Place an item at the altar and the gods will see to that it gets strengthed.";
    return default_desc;
};

const std::string& AltarItemScene::get_sell_btn_text()
{
    static std::string default_desc = "Place";
    return default_desc;
}

AltarItemScene* AltarItemScene::create(ItemSlotType slot_type)
{
    AltarItemScene* pRet = new(std::nothrow) AltarItemScene();
    if (pRet && pRet->init(slot_type))
    {
        pRet->autorelease();
        return pRet;
    }
    else
    {
        delete pRet;
        pRet = nullptr;
        return nullptr;
    }
};

bool AltarItemScene::init(ItemSlotType slot_type)
{
    this->filtered_slot_type = slot_type;

#ifdef _WIN32
    FUNC_INIT_WIN32(AltarItemScene);
#else
    FUNC_INIT(AltarItemScene);
#endif

    this->init_children();
    this->init_callbacks();

    return true;
};

cocos2d::Scene* AltarItemScene::createScene(ItemSlotType slot_type)
{
    auto scene = cocos2d::Scene::create();
    auto* layer = AltarItemScene::create(slot_type);
    scene->addChild(layer);
    return scene;
};

const std::string& EquipItemScene::get_scene_title()
{
    static std::string scene_title = "Equip Item";
    return scene_title;
};

const std::string& EquipItemScene::get_default_detail_panel_title()
{
    static std::string default_title = "Item";
    return default_title;
};

const std::string& EquipItemScene::get_default_detail_panel_description()
{
    static std::string default_desc = "Equip and item and harness its power.";
    return default_desc;
};

const std::string& EquipItemScene::get_sell_btn_text()
{
    static std::string default_desc = "Equip";
    return default_desc;
}

EquipItemScene* EquipItemScene::create(ItemSlotType slot_type)
{
    EquipItemScene* pRet = new(std::nothrow) EquipItemScene();
    if (pRet && pRet->init(slot_type))
    {
        pRet->autorelease();
        return pRet;
    }
    else
    {
        delete pRet;
        pRet = nullptr;
        return nullptr;
    }
};

void EquipItemScene::init_callbacks()
{

    auto update_detail_panel_on_touch = [this](NuItem* nuitem, spItem item) {
        this->item_name->setString(item->get_name());
        this->item_desc->setString(item->description);

        this->item_sell_btn->setVisible(true);
        this->item_sell_btn->addTouchEventListener([this, item](Ref* sender, ui::Widget::TouchEventType type){
            if (type == ui::Widget::TouchEventType::ENDED)
            {
                do_vibrate(16);
                GameLogic::getInstance()->equipment->get_slot_by_type(this->filtered_slot_type)->set_item(item);
                cocos2d::Director::getInstance()->popScene();
            }
        });

        //update listviews layout to account for different content height
        this->item_listview_description->requestDoLayout();
    };

    for (spItem item : this->get_items()) {
        auto nuitem = NuItem::create(items_listview);
        nuitem->set_title(item->get_name());
        nuitem->set_description(item->summary);

        nuitem->set_image(item->img_path, ui::Widget::TextureResType::LOCAL);

        res_count_t cost = item->get_effective_cost();
        std::string cost_str = beautify_double(cost);
        CCLOG("cost %f, beauty cost %s", cost, cost_str.c_str());
        nuitem->set_cost_lbl(cost_str);

        nuitem->button->addTouchEventListener([update_detail_panel_on_touch, item, nuitem](Ref* sender, ui::Widget::TouchEventType type){
            if (type == ui::Widget::TouchEventType::ENDED)
            {
                do_vibrate(16);
                update_detail_panel_on_touch(nuitem, item);
            }
        });
    };
};

bool EquipItemScene::init(ItemSlotType slot_type)
{
    this->filtered_slot_type = slot_type;

#ifdef _WIN32
    FUNC_INIT_WIN32(EquipItemScene);
#else
    FUNC_INIT(EquipItemScene);
#endif

    this->init_children();
    this->init_callbacks();


    return true;
};

cocos2d::Scene* EquipItemScene::createScene(ItemSlotType slot_type)
{
    auto scene = cocos2d::Scene::create();
    auto* layer = EquipItemScene::create(slot_type);
    scene->addChild(layer);
    return scene;
};
