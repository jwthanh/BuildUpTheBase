#include "NuMenu.h"
#include <editor-support/cocostudio/ActionTimeline/CSLoader.h>
#include "Beatup.h"
#include "StaticData.h"

USING_NS_CC;

#include "BaseMenu.h"
#include "Util.h"

void NuItem::my_init(Beatup* beatup, cocos2d::Node* parent)
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

    parent->addChild(this); //FIXME hack to get the selector to run. ideally the button would be a child of this but then button doesnt go on the right spot
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

void ShopNuItem::my_init(Beatup* beatup, Node* parent, std::string id_key)
    
{
    NuItem::my_init(beatup, parent);
    this->id_key = id_key;
    this->_shop_cost = -1;

    CCLOG("id_key %s", id_key.c_str());
};

void ShopNuItem::update_func(float dt)
{
    if (this->get_been_bought()) 
    {
        //TODO make this not happen all the time, or check that its not the same image
        this->set_image("grey_boxCheckmark.png");
    };

    int cost = this->get_cost();

    if (this->beatup->get_total_coins() < cost)
    {
        this->button->setBright(false);
        this->button->setEnabled(false);
    }

    if (this->get_been_bought())
    {
        this->set_cost("---");
        this->button->setBright(false);
        this->button->setEnabled(false);
    };

};

void BuildingShopNuItem::my_init(spBuilding building, Node* parent)
{
    ShopNuItem::my_init(building->city->buildup->beatup, parent, building->id_key);
    this->set_image(building->data->get_img_large());
    this->set_title(building->name);
    this->set_description(building->data->get_description());
    this->set_cost(building->data->get_gold_cost());

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

    //setup menu items
    this->init_items();

    return true;
};

void NuMenu::init_items()
{
    auto scrollview = this->create_center_scrollview();

    //setup menu items
    auto inst = cocos2d::CSLoader::getInstance();

    for (auto building : this->beatup->buildup->city->buildings) {
        CCLOG("upper key %s", building->id_key.c_str());
        //auto menu_item = std::make_shared<BuildingShopNuItem>(building, scrollview);
        auto menu_item = BuildingShopNuItem::create();
        menu_item->my_init(building, scrollview);

        auto scheduler = Director::getInstance()->getScheduler();
        scheduler->schedule(CC_SCHEDULE_SELECTOR(BuildingShopNuItem::update_func), menu_item, 0.01f, true);

        menu_item->button->addTouchEventListener([building, this](Ref* sender, ui::Widget::TouchEventType type)
        {
            if (type == ui::Widget::TouchEventType::ENDED)
            {
                //can afford
                if (building->get_cost() <= this->beatup->get_total_coins())
                {
                    CCLOG("buying stuff");
                    building->set_been_bought(true);
                    this->beatup->add_total_coin(-building->get_cost());
                }
            }
        });
    };

    scrollview->resize_to_fit();
};
