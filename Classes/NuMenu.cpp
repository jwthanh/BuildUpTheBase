#include "NuMenu.h"
#include <editor-support/cocostudio/ActionTimeline/CSLoader.h>
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

    for (auto building : this->beatup->buildup->city->buildings) {
        cocos2d::ui::Button* menu_item = static_cast<cocos2d::ui::Button*>(inst->createNode("editor/buttons/menu_item.csb")->getChildByName("menu_item_btn"));
        menu_item->loadTextures("main_UI_export_10_x4.png", "main_UI_export_10_x4_pressed.png", "main_UI_export_10_x4_disabled.png", cocos2d::ui::TextureResType::PLIST);
        menu_item->removeFromParent();
        scrollview->addChild(menu_item);

        cocos2d::ui::ImageView* item_icon = static_cast<cocos2d::ui::ImageView*>(menu_item->getChildByName("item_icon"));
        std::string img_large = building->data->get_img_large();
        item_icon->loadTexture(img_large, ui::TextureResType::PLIST);

        cocos2d::ui::Text* title_lbl = static_cast<cocos2d::ui::Text*>(menu_item->getChildByName("title_panel")->getChildByName("title_lbl"));
        title_lbl->setString(building->name);

        cocos2d::ui::Text* desc_lbl = static_cast<cocos2d::ui::Text*>(menu_item->getChildByName("description_panel")->getChildByName("description_lbl"));
        desc_lbl->setString(building->data->get_description());

        cocos2d::ui::Text* cost_lbl = static_cast<cocos2d::ui::Text*>(menu_item->getChildByName("cost_panel")->getChildByName("cost_lbl"));
        auto cost_str = building->data->get_gold_cost();
        cost_lbl->setString(cost_str);


        std::function<void(float)> update_func = [item_icon, cost_lbl, cost_str, menu_item, this, building ](float dt) {
            if (building->get_been_bought()) 
            {
                //TODO make this not happen all the time, or check that its not the same image
                item_icon->loadTexture("grey_boxCheckmark.png", ui::TextureResType::PLIST);
            };

            int cost = std::atoi(cost_str.c_str());

            if (this->beatup->get_total_coins() < cost)
            {
                menu_item->setBright(false);
                menu_item->setEnabled(false);
            }

            if (building->get_been_bought())
            {
                cost_lbl->setString("---");
                menu_item->setBright(false);
                menu_item->setEnabled(false);
            };
        };

        Director::getInstance()->getScheduler()->schedule(update_func, menu_item, 0.01f, true, "doesthismatter");

        menu_item->addTouchEventListener([building, this](Ref* sender, ui::Widget::TouchEventType type)
        {
            if (type == ui::Widget::TouchEventType::ENDED)
            {
                //can afford
                if (building->get_cost() <= this->beatup->get_total_coins())
                {
                    building->set_been_bought(true);
                    this->beatup->add_total_coin(-building->get_cost());
                }
            }
        });
    };

    scrollview->resize_to_fit();

    return true;
};
