#include "NuMenu.h"
#include <cocos2d/cocos/editor-support/cocostudio/ActionTimeline/CSLoader.h>
#include "Beatup.h"
#include "StaticData.h"

USING_NS_CC;

#include "BaseMenu.h"
#include "Util.h"

NuMenu* NuMenu::create(Beatup* beatup)
{
    NuMenu *menu = new(std::nothrow) NuMenu();
    menu->beatup = beatup; //this should be after init, cause i guess init should fail, but its fine for now.

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

bool NuMenu::init()
{

#ifdef _WIN32
    FUNC_INIT_WIN32(NuMenu);
#else
    FUNC_INIT(NuMenu);
#endif

    //setup title
    //setup backbutton

    //setup scrollable
    auto scrollview = this->create_center_scrollview();

    //setup menu items
    auto inst = cocos2d::CSLoader::getInstance();

    int gold_in_hand = 10;

    for (auto building : this->beatup->buildup->city->buildings) {
        cocos2d::ui::Button* menu_item = static_cast<cocos2d::ui::Button*>(inst->createNode("editor/buttons/menu_item.csb")->getChildByName("menu_item_btn"));
        menu_item->loadTextures("main_UI_export_10_x4.png", "main_UI_export_10_x4_pressed.png", "main_UI_export_10_x4_disabled.png", cocos2d::ui::TextureResType::PLIST);
        menu_item->removeFromParent();
        scrollview->addChild(menu_item);

        cocos2d::ui::Text* title_lbl = static_cast<cocos2d::ui::Text*>(menu_item->getChildByName("title_panel")->getChildByName("title_lbl"));
        title_lbl->setString(building->name);

        cocos2d::ui::Text* desc_lbl = static_cast<cocos2d::ui::Text*>(menu_item->getChildByName("description_panel")->getChildByName("description_lbl"));
        desc_lbl->setString(BaseStaticData::get_data("buildings", building->name, "description"));

        cocos2d::ui::Text* cost_lbl = static_cast<cocos2d::ui::Text*>(menu_item->getChildByName("cost_panel")->getChildByName("cost_lbl"));
        auto cost_str = BaseStaticData::get_data("buildings", building->name, "gold_cost");
        cost_lbl->setString(cost_str);

        //should be update func
        int cost = std::atoi(cost_str.c_str());
        if (gold_in_hand < cost)
        {
            menu_item->setBright(false);
        }

        if (building->get_been_bought())
        {
            menu_item->setColor(Color3B::GREEN);
            menu_item->setBright(false);
        };



        menu_item->addTouchEventListener([&](Ref* sender, ui::Widget::TouchEventType type)
        {
            if (type == ui::Widget::TouchEventType::ENDED)
            {
                //can afford
                //if (building->)
            }
        });
    };

    scrollview->resize_to_fit();

    return true;
};
