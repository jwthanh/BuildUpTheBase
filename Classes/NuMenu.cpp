#include "NuMenu.h"
#include <editor-support/cocostudio/ActionTimeline/CSLoader.h>
#include "Beatup.h"
#include "StaticData.h"

#include "HouseBuilding.h"
#include "MainMenu.h"

#include "BaseMenu.h"
#include "Util.h"
#include "Worker.h"

USING_NS_CC;


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

    this->set_title("");
    this->set_description("");
    this->set_cost_lbl("");
    // this->set_image("");

};

void NuItem::set_touch_ended_callback(std::function<void(void)> callback)
{
    this->button->addTouchEventListener([callback](Ref* sender, ui::Widget::TouchEventType type)
    {
        if (type == ui::Widget::TouchEventType::ENDED)
        {
            callback();
        }
    });

};

void NuItem::set_image(std::string path)
{
    this->item_icon->loadTexture(path, ui::TextureResType::PLIST);

    ui::Scale9Sprite* sprite = dynamic_cast<ui::Scale9Sprite*>(this->item_icon->getVirtualRenderer());
    if (sprite)
    {
        sprite->getSprite()->getTexture()->setAliasTexParameters();
    } else
    {
        CCLOG("cannot convert item_icon pointer to scale9");
    }
};

void NuItem::set_title(std::string title)
{
    this->title_lbl->setString(title);
};

void NuItem::set_description(std::string description)
{
    this->desc_lbl->setString(description);
};

void NuItem::set_cost_lbl(std::string cost)
{
    this->cost_lbl->setString(cost);
};

void ShopNuItem::my_init(Beatup* beatup, Node* parent, std::string id_key)
{
    NuItem::my_init(beatup, parent);

    this->id_key = id_key;
    this->_shop_cost = -1;
};

void ShopNuItem::update_func(float dt)
{
    //anytime the disk is checked, this slows down
    if (this->get_been_bought()) 
    {
        //Check if image has been updated
        ResourceData data = this->item_icon->getRenderFile();
        std::string checkbox_path = "grey_boxCheckmark.png";
        if (data.file != checkbox_path)
        {
            this->set_image(checkbox_path);
        }
    };

    int cost = this->get_cost();

    this->cost_lbl->setTextColor(Color4B::BLACK);
    if (this->beatup->get_total_coins() < cost)
    {
        this->button->setBright(false);
        this->button->setEnabled(false);



        Color3B color = { 254, 81, 81 };

        float tint = 0.9f;
        Color3B text_color = Color3B(
            color.r*tint,
            color.g*tint,
            color.b*tint
            );

        this->cost_lbl->setTextColor(Color4B(text_color));
        this->button->setColor(color);
    }

    if (this->get_been_bought())
    {
        this->set_cost_lbl("---");
        this->button->setBright(false);
        this->button->setEnabled(false);
        this->cost_lbl->setTextColor(Color4B::GRAY);
    };

};

void BuildingShopNuItem::my_init(spBuilding building, Node* parent)
{
    ShopNuItem::my_init(building->city->buildup->beatup, parent, building->id_key);

    this->set_image(building->data->get_img_large());
    this->set_title(building->name);
    this->set_description(building->data->get_description());

    this->set_cost_lbl(std::to_string(building->get_cost()));
    this->_shop_cost = building->get_cost();
};

bool NuMenu::init()
{

#ifdef _WIN32
    FUNC_INIT_WIN32(NuMenu);
#else
    FUNC_INIT(NuMenu);
#endif

    //setup title

    //the scrollable in the center
    this->scrollable = this->create_center_scrollview();

    //setup menu items
    this->init_items();

    //setup backbutton
    this->create_back_item(scrollable);

    //resizes scrollable based on size of contents TODO:actually measure sizes
    scrollable->resize_to_fit();

    return true;
};

void NuMenu::create_back_item(cocos2d::Node* parent)
{
    auto menu_item = NuItem::create();
    menu_item->my_init(this->beatup, parent);

    menu_item->set_title("Back");
    menu_item->set_description("Return to previous screen");

    menu_item->set_image("arrowLeft.png");

    auto pop_scene = [](){
        auto director = Director::getInstance();
        director->popScene();
    };
    menu_item->set_touch_ended_callback(pop_scene);

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
        scheduler->schedule(CC_SCHEDULE_SELECTOR(BuildingShopNuItem::update_func), menu_item, 0.5f, true); //TODO make this happen more optimally, reading disk is slow
        menu_item->update_func(0);

        auto buy_stuff = [this, building, menu_item](){
            //can afford
            auto cost = building->get_cost();
            auto total_coins = this->beatup->get_total_coins();
            if (cost <= total_coins)
            {
                CCLOG("buying stuff");
                building->set_been_bought(true);
                this->beatup->add_total_coin(-building->get_cost());

                menu_item->update_func(0);
            }
        };

        menu_item->set_touch_ended_callback(buy_stuff);

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
    menu_item->set_description("Buy Auto-Harvester");
    menu_item->set_touch_ended_callback([this]()
    {
        CCLOG("Buying auto havester");
        auto harvester = std::make_shared<Harvester>(this->building, "test worker");
        harvester->active_count += 1;
        this->building->harvesters.push_back(harvester);
    });

};

void BuildingNuMenu::create_inventory_item(cocos2d::Node* parent)
{
    auto inventory_item = NuItem::create();
    inventory_item->my_init(this->beatup, parent);

    inventory_item->set_title("Inventory");
    inventory_item->set_description("Check out what this building contains.");

    inventory_item->set_image("zoomIn.png");

    auto open_inventory = [this](){
        auto scene = Scene::create();
        InventoryMenu* building_menu = InventoryMenu::create(this->building);
        scene->addChild(building_menu);

        auto director = Director::getInstance();
        director->pushScene(scene);
    };

    inventory_item->set_touch_ended_callback(open_inventory);

};
