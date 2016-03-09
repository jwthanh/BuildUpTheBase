#include "NuMenu.h"
#include <editor-support/cocostudio/ActionTimeline/CSLoader.h>
#include "Beatup.h"
#include "StaticData.h"

USING_NS_CC;

#include "BaseMenu.h"
#include "Util.h"

NuItem::NuItem(Beatup* beatup, cocos2d::Node* parent)
{
    this->beatup = beatup;

    auto inst = cocos2d::CSLoader::getInstance();

    this->button = static_cast<cocos2d::ui::Button*>(inst->createNode("editor/buttons/menu_item.csb")->getChildByName("menu_item_btn"));
    button->loadTextures(
            "main_UI_export_10_x4.png",
            "main_UI_export_10_x4_pressed.png",
            "main_UI_export_10_x4_disabled.png",
            cocos2d::ui::TextureResType::PLIST);
    button->removeFromParent();

    parent->addChild(button);

    this->item_icon = static_cast<cocos2d::ui::ImageView*>(button->getChildByName("item_icon"));
    this->title_lbl = static_cast<cocos2d::ui::Text*>(button->getChildByName("title_panel")->getChildByName("title_lbl"));
    this->desc_lbl = static_cast<cocos2d::ui::Text*>(button->getChildByName("description_panel")->getChildByName("description_lbl"));
    this->cost_lbl = static_cast<cocos2d::ui::Text*>(button->getChildByName("cost_panel")->getChildByName("cost_lbl"));

};

void NuItem::set_image(std::string path)
{
    this->item_icon->loadTexture(path, ui::TextureResType::PLIST);
};

void NuItem::set_title(std::string title)
{
    this->title_lbl->setString(title);
};

void NuItem::set_description(std::string description)
{
    this->desc_lbl->setString(description);
};

void NuItem::set_cost(std::string cost)
{
    this->cost_lbl->setString(cost);
};

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
        auto menu_item = std::make_shared<NuItem>(beatup, scrollview);

        menu_item->set_image(building->data->get_img_large());
        menu_item->set_title(building->name);
        menu_item->set_description(building->data->get_description());
        menu_item->set_cost(building->data->get_gold_cost());

        std::function<void(float)> update_func = [menu_item, this, building ](float dt) {
            if (building->get_been_bought()) 
            {
                //TODO make this not happen all the time, or check that its not the same image
                menu_item->set_image("grey_boxCheckmark.png");
            };

            int cost = std::atoi(building->data->get_gold_cost().c_str());

            if (this->beatup->get_total_coins() < cost)
            {
                menu_item->button->setBright(false);
                menu_item->button->setEnabled(false);
            }

            if (building->get_been_bought())
            {
                menu_item->set_cost("---");
                menu_item->button->setBright(false);
                menu_item->button->setEnabled(false);
            };
        };

        Director::getInstance()->getScheduler()->schedule(update_func, menu_item->button, 0.01f, true, "doesthismatter");

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
