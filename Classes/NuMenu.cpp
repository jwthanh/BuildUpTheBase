#include "NuMenu.h"
#include <editor-support/cocostudio/ActionTimeline/CSLoader.h>
#include "Beatup.h"
#include "StaticData.h"

#include "HouseBuilding.h"
#include "MainMenu.h"

#include "BaseMenu.h"
#include "Util.h"
#include "Worker.h"
#include "GameLogic.h"
#include "Recipe.h"
#include "MiscUI.h"
#include <numeric>

USING_NS_CC;


void NuItem::my_init(cocos2d::Node* parent)
{
    auto inst = cocos2d::CSLoader::getInstance();

    this->button = static_cast<cocos2d::ui::Button*>(inst->createNode("editor/buttons/menu_item.csb")->getChildByName("menu_item_btn"));
    load_default_button_textures(this->button);
    button->removeFromParent();

    parent->addChild(this); //FIXME hack to get the selector to run. ideally the button would be a child of this but then button doesnt go on the right spot
    parent->addChild(button);

    this->item_icon = static_cast<cocos2d::ui::ImageView*>(button->getChildByName("item_icon"));
    this->title_lbl = static_cast<cocos2d::ui::Text*>(button->getChildByName("title_panel")->getChildByName("title_lbl"));
    this->desc_lbl = static_cast<cocos2d::ui::Text*>(button->getChildByName("description_panel")->getChildByName("description_lbl"));
    this->cost_lbl = static_cast<cocos2d::ui::Text*>(button->getChildByName("cost_panel")->getChildByName("cost_lbl"));
    this->count_lbl = static_cast<cocos2d::ui::Text*>(button->getChildByName("cost_panel")->getChildByName("count_lbl"));

    this->set_title("");
    this->set_description("");
    this->set_cost_lbl("");
    this->set_count_lbl(0);
    // this->set_image("");

    this->schedule(CC_SCHEDULE_SELECTOR(NuItem::update_func));

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
    //same loaded image, do nothing
    if (this->item_icon->getRenderFile().file == path)
    {
        return;
    };

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

void NuItem::set_count_lbl(int count)
{
    if (count == 0) {
        this->count_lbl->setString("");
        return;
    };

    std::stringstream count_ss;
    count_ss << "[x" << count << "]";
    this->count_lbl->setString(count_ss.str());
};

void NuItem::try_set_enable(bool is_enable)
{
    if (this->button->isEnabled() != is_enable)
    {
        this->button->setEnabled(is_enable);
    }
};


void ShopNuItem::my_init(Node* parent, std::string id_key)
{
    NuItem::my_init(parent);

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
    int total_coins = BEATUP->get_total_coins();

    this->cost_lbl->setTextColor(Color4B::BLACK);

    if (this->get_been_bought())
    {
        this->set_cost_lbl("---");
        try_set_enable(false);
        this->cost_lbl->setTextColor(Color4B::GRAY);
    } 
    else if (total_coins < cost)
    {
        try_set_enable(false);

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
    else
    {
        try_set_enable(true);
        //NOTE get_cost returns int
        this->set_cost_lbl(beautify_double((double)this->get_cost()));
        this->button->setColor(Color3B::WHITE);
    };


};

void RecipeNuItem::my_init(spRecipe recipe, std::shared_ptr<Building> building, Node* parent)
{
    NuItem::my_init(parent);

    this->recipe = recipe;
    this->building = building;

    this->set_touch_ended_callback([this]() {
        CCLOG("trying to consume %s recipe", this->recipe->name.c_str());
        this->building->consume_recipe(this->recipe.get());
    });

};

void RecipeNuItem::update_func(float dt)
{
    //FIXME, this lets someone press a button early i think
    // and avoid having the button disabled. Not sure.
    if (this->button->isHighlighted())
    {
        return;
    };

    if (this->recipe->is_satisfied(this->building->ingredients)){
        this->try_set_enable(true);
    } else {
        this->try_set_enable(false);
    }

    //TODO this'll get fucky when recipes output more than one type,
    // you make one recipe, the count'll go up by two
    int result_count = 0;
    for (auto output_ing : this->recipe->outputs) {
        auto out_type = output_ing.first;
        result_count += this->building->count_ingredients(out_type);
    };
    this->set_count_lbl(result_count);
};

void BuildingShopNuItem::my_init(spBuilding building, Node* parent)
{
    ShopNuItem::my_init(parent, building->id_key);

    this->set_image(building->data->get_img_large());
    this->set_title(building->name);
    this->set_description(building->data->get_description());

    this->set_cost_lbl(std::to_string(building->get_cost()));
    this->_shop_cost = building->get_cost();
}

void HarvesterShopNuItem::my_init_title()
{
    std::vector<std::string> names = {
        "Jamal", "Josh", "James", "Jimmy", "Jonathan", "Javert", "John",
        "Jackson", "Jax", "Jimothy", "Jasper", "Joe", "Jenson", "Jack",
        "Justin", "Jaleel", "Jamar", "Jesse", "Jaromir", "Jebediah", 
        "Johan", "Jericho"
    };
    std::string harvester_name = pick_one(names);

    this->set_title(harvester_name);
}

void HarvesterShopNuItem::my_init(Node* parent, spBuilding building)
{
    ShopNuItem::my_init(parent, "harvester_buy");
    this->desc_lbl->setFontSize(18);
    this->_shop_cost = 25;
    this->set_cost_lbl("25");

    this->my_init_title();
    this->my_init_sprite();
    this->my_init_touch_ended_callback();
    this->my_init_update_callback();
};

void HarvesterShopNuItem::my_init_sprite()
{
    auto gen_paths = [](std::string base_path, int max_num)
    {
        std::vector<int> nums(max_num);
        std::iota(nums.begin(), nums.end(), 0);

        std::vector<std::string> output;
        for (auto num : nums)
        {
            output.push_back(base_path + "_" + std::to_string(num)+".png");
        }

        return  output;
    };

    auto base_node = Node::create();
    auto sprites = {
        pick_one(gen_paths("set", 4)),
        pick_one(gen_paths("body", 49)),
        pick_one(gen_paths("headwear", 49)),
        pick_one(gen_paths("legs", 22)),
        pick_one(gen_paths("shield", 49)),
        pick_one(gen_paths("weapon", 49))
    };
    for (auto path : sprites)
    {
        base_node->addChild(Sprite::createWithSpriteFrameName(path));
    }

    base_node->setPosition(8,8);
    base_node->setScaleY(-1.0f);

    auto rt = RenderTexture::create(16, 16);
    rt->retain();
    rt->begin();
    base_node->visit();
    rt->end();

    ui::Scale9Sprite* vr = (ui::Scale9Sprite*)this->item_icon->getVirtualRenderer();
    vr->setSpriteFrame(rt->getSprite()->getSpriteFrame());
};

void HarvesterShopNuItem::my_init_touch_ended_callback()
{
    this->set_touch_ended_callback([this]()
    {
        auto cost = this->get_cost();
        auto total_coins = BEATUP->get_total_coins();

        if (cost <= total_coins)
        {
            CCLOG("buying a harvester");
            BEATUP->add_total_coin(-cost);
            auto building = BUILDUP->get_target_building();

            auto ing_type = Ingredient::string_to_type(building->punched_sub_type);
            auto harv_type = Harvester::SubType::One;

            auto harvester_count = map_get(building->harvesters, { harv_type, ing_type }, 0);
            harvester_count++;
            building->harvesters[{ harv_type, ing_type }] = harvester_count;
            this->update_func(0);
        }
    });

};

void HarvesterShopNuItem::my_init_update_callback()
{
    auto update_harvesters_cb = [this](float dt) {
        auto building = BUILDUP->get_target_building();
        auto harvesters_owned = map_get(building->harvesters, {Harvester::SubType::One, Ingredient::string_to_type(building->punched_sub_type)}, 0);
        this->set_count_lbl(harvesters_owned);
        this->_shop_cost = 25 * std::pow(1.15f, std::max(0, (int)harvesters_owned));

        std::stringstream ss;
        ss << "Buy Auto-Harvester\nAdds 1 " << building->punched_sub_type << " per sec";
        this->set_description(ss.str());
    };
    this->schedule(update_harvesters_cb, 0.1f, "harvester_count");
    update_harvesters_cb(0);

};

void NuMenu::onEnter()
{
    GameLayer::onEnter();
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

    auto back_btn = ui::Button::create();
    back_btn->addTouchEventListener([](Ref* touch, ui::Widget::TouchEventType type){
        if (type == ui::Widget::TouchEventType::ENDED)
        {
            auto director = Director::getInstance();
            director->popScene();
        }
    });
    load_default_button_textures(back_btn);
    back_btn->setPosition(Vec2(90, 540));
    back_btn->ignoreContentAdaptWithSize(false);
    back_btn->setContentSize(Size(150, 60));
    back_btn->setTitleText("Back");
    back_btn->setTitleFontSize(24);
    back_btn->setTitleColor(Color3B::BLACK);

    this->addChild(back_btn);

};

void BuyBuildingsNuMenu::init_items()
{
    auto scrollview = this->scrollable;

    //setup menu items
    auto inst = cocos2d::CSLoader::getInstance();

    for (auto building : BUILDUP->city->buildings) {
        auto menu_item = BuildingShopNuItem::create();
        menu_item->my_init(building, scrollview);

        auto scheduler = Director::getInstance()->getScheduler();
        scheduler->schedule(CC_SCHEDULE_SELECTOR(BuildingShopNuItem::update_func), menu_item, 0.5f, true); //TODO make this happen more optimally, reading disk is slow
        menu_item->update_func(0);

        auto buy_stuff = [this, building, menu_item](){
            //can afford
            auto cost = building->get_cost();
            auto total_coins = BEATUP->get_total_coins();
            if (cost <= total_coins)
            {
                CCLOG("buying stuff");
                building->set_been_bought(true);
                BEATUP->add_total_coin(-building->get_cost());

                menu_item->update_func(0);
            }
        };

        menu_item->set_touch_ended_callback(buy_stuff);

    };

};

BuildingNuMenu* BuildingNuMenu::create(std::shared_ptr<Building> building)
{
    BuildingNuMenu *menu = new(std::nothrow) BuildingNuMenu();
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
    float update_delay = 0.1f;

    auto scrollview = this->scrollable;
    this->create_inventory_item(scrollview);

    //buy an auto harvester button
    auto menu_item = ShopNuItem::create();
    menu_item->my_init(scrollview, "harvester_buy");
    menu_item->_shop_cost = 25;
    menu_item->set_cost_lbl("25");

    std::vector<std::string> names = {
        "Jamal", "Josh", "James", "Jimmy", "Jonathan", "Javert", "John",
        "Jackson", "Jax", "Jimothy", "Jasper", "Joe", "Jenson", "Jack",
        "Justin", "Jaleel", "Jamar", "Jesse", "Jaromir", "Jebediah", 
        "Johan", "Jericho"
    };
    std::string name = pick_one(names);

    menu_item->set_title(name);
    menu_item->set_description("Buy Auto-Harvester");
    menu_item->set_image("harvester.png");
    menu_item->set_touch_ended_callback([this, menu_item]()
    {
        auto cost = menu_item->get_cost();
        auto total_coins = BEATUP->get_total_coins();

        if (cost <= total_coins)
        {
            CCLOG("buying a harvester");
            BEATUP->add_total_coin(-cost);
            auto ing_type = Ingredient::string_to_type(this->building->punched_sub_type);
            auto harv_type = Harvester::SubType::One;

            auto harvester_count = map_get(this->building->harvesters, { harv_type, ing_type }, 0);
            harvester_count++;
            this->building->harvesters.at({ harv_type, ing_type }) = harvester_count;

            menu_item->update_func(0);
        }
    });
    auto update_harvesters_cb = [this, menu_item](float dt) {
        auto harvesters_owned = this->building->harvesters.size();
        menu_item->set_count_lbl(harvesters_owned);
        menu_item->_shop_cost = 25 * std::pow(1.15f, std::max(0, (int)harvesters_owned));
    };
    menu_item->schedule(update_harvesters_cb, update_delay, "harvester_count");
    update_harvesters_cb(0);

    for (auto recipe : building->data->get_all_recipes())
    {
        auto convert_item = RecipeNuItem::create();

        recipe->_callback = [this, recipe]() {
            for (auto pair : recipe->outputs) {
                Ingredient::SubType ing_type = pair.first;
                int count = pair.second;
                this->building->create_ingredients(ing_type, count);
            };
        };

        convert_item->my_init(recipe, this->building, scrollview);
        convert_item->set_title(recipe->name);
        convert_item->set_description(recipe->description);

    }

};

void BuildingNuMenu::create_inventory_item(cocos2d::Node* parent)
{
    auto inventory_item = NuItem::create();
    inventory_item->my_init(parent);

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
