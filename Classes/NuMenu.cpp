#include "NuMenu.h"
#include <editor-support/cocostudio/ActionTimeline/CSLoader.h>
#include "Beatup.h"
#include "StaticData.h"

#include "HouseBuilding.h"
#include "MainMenu.h"

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

bool NuMenu::init()
{

#ifdef _WIN32
    FUNC_INIT_WIN32(NuMenu);
#else
    FUNC_INIT(NuMenu);
#endif

    //setup title
    //setup backbutton

    this->scrollable = this->create_center_scrollview();

    //setup menu items
    this->init_items();

    this->create_back_item(scrollable);
    scrollable->resize_to_fit();

    return true;
};

void NuMenu::create_back_item(cocos2d::Node* parent)
{
    auto menu_item = NuItem::create();
    menu_item->my_init(this->beatup, parent);

    menu_item->set_title("Back");
    menu_item->set_description("Return to previous screen");

    menu_item->button->addTouchEventListener([](Ref* sender, ui::Widget::TouchEventType type)
    {
        if (type == ui::Widget::TouchEventType::ENDED)
        {
            auto director = Director::getInstance();
            director->popScene();
        }
    });
};

void BuyBuildingsNuMenu::init_items()
{
    auto scrollview = this->scrollable;

    //setup menu items
    auto inst = cocos2d::CSLoader::getInstance();

    for (auto building : this->beatup->buildup->city->buildings) {
        // CCLOG("upper key %s", building->id_key.c_str());
        //auto menu_item = std::make_shared<BuildingShopNuItem>(building, scrollable);
        auto menu_item = BuildingShopNuItem::create();
        menu_item->my_init(building, scrollview);

        auto scheduler = Director::getInstance()->getScheduler();
        scheduler->schedule(CC_SCHEDULE_SELECTOR(BuildingShopNuItem::update_func), menu_item, 0.01f, true);

        //touch handler
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

};

BuildingNuMenu* BuildingNuMenu::create(Beatup* beatup, std::shared_ptr<Building> building)
{
    BuildingNuMenu *menu = new(std::nothrow) BuildingNuMenu();
    menu->beatup = beatup; //this should be after init, cause i guess init should fail, but its fine for now.
    menu->building = building;

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

void BuildingNuMenu::init_items()
{
    auto scrollview = this->scrollable;
    this->create_inventory_item(scrollview);

    auto menu_item = NuItem::create();
    menu_item->my_init(this->beatup, scrollview);

    menu_item->set_title(building->name);
    menu_item->set_description(building->data->get_task_name());

};

void BuildingNuMenu::create_inventory_item(cocos2d::Node* parent)
{
    auto inventory_item = NuItem::create();
    inventory_item->my_init(this->beatup, parent);

    inventory_item->set_title("Inventory");
    inventory_item->set_description("Check out what this building contains.");

    inventory_item->button->addTouchEventListener([this](Ref* sender, ui::Widget::TouchEventType type)
    {
        if (type == ui::Widget::TouchEventType::ENDED)
        {
            auto scene = Scene::create();
            InventoryMenu* building_menu = InventoryMenu::create(this->building);
            scene->addChild(building_menu);

            auto director = Director::getInstance();
            director->pushScene(scene);
        }
    });
};
