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
#include "FShake.h"

USING_NS_CC;

cocos2d::ui::Button* NuItem::orig_button = NULL;

void NuItem::my_init(cocos2d::Node* parent)
{
    auto inst = cocos2d::CSLoader::getInstance();


    if (NuItem::orig_button == NULL)
    {
        CCLOG("reload");
        NuItem::orig_button = static_cast<cocos2d::ui::Button*>(inst->createNode("editor/buttons/menu_item.csb")->getChildByName("menu_item_btn"));
        load_default_button_textures(NuItem::orig_button);
        NuItem::orig_button->retain();
        ((ui::Text*)NuItem::orig_button->getChildByName("title_panel")->getChildByName("title_lbl"))->setString("");
        ((ui::Text*)NuItem::orig_button->getChildByName("description_panel")->getChildByName("description_lbl"))->setString("");
        ((ui::Text*)NuItem::orig_button->getChildByName("cost_panel")->getChildByName("cost_lbl"))->setString("");
        ((ui::Text*)NuItem::orig_button->getChildByName("cost_panel")->getChildByName("count_lbl"))->setString("");

    }

    //clone orig button, background is still messed though
    this->button = (ui::Button*)NuItem::orig_button->clone();
    this->button->setCascadeColorEnabled(true);
    for (auto child : this->button->getChildren())
    {
        child->setCascadeColorEnabled(true);
    }

    load_default_button_textures(this->button); //TODO figure out why this is still needed, why the clone doesnt do this for me

    button->removeFromParent();

    parent->addChild(this); //FIXME hack to get the selector to run. ideally the button would be a child of this but then button doesnt go on the right spot
    parent->addChild(button);

    this->item_icon = static_cast<cocos2d::ui::ImageView*>(button->getChildByName("item_icon"));
    this->title_lbl = static_cast<cocos2d::ui::Text*>(button->getChildByName("title_panel")->getChildByName("title_lbl"));
    this->desc_lbl = static_cast<cocos2d::ui::Text*>(button->getChildByName("description_panel")->getChildByName("description_lbl"));
    this->cost_lbl = static_cast<cocos2d::ui::Text*>(button->getChildByName("cost_panel")->getChildByName("cost_lbl"));
    this->count_lbl = static_cast<cocos2d::ui::Text*>(button->getChildByName("cost_panel")->getChildByName("count_lbl"));

    this->set_cost_lbl("");

    this->schedule(CC_SCHEDULE_SELECTOR(NuItem::update_func));

    auto reposition_labels = [this](float dt)
    {
        //defaults
        this->cost_lbl->setPosition(NuItem::orig_button->getChildByName("cost_panel")->getChildByName("cost_lbl")->getPosition());
        this->count_lbl->setPosition(NuItem::orig_button->getChildByName("cost_panel")->getChildByName("count_lbl")->getPosition());
        this->desc_lbl->setPosition(NuItem::orig_button->getChildByName("description_panel")->getChildByName("description_lbl")->getPosition());
        this->desc_lbl->setContentSize(NuItem::orig_button->getChildByName("description_panel")->getChildByName("description_lbl")->getContentSize());

        //if theres no cost, move count up
        if (this->cost_lbl->getStringLength() == 0) {
            this->count_lbl->setPosition(this->cost_lbl->getPosition());
            this->desc_lbl->setContentSize({ 300.0f, 44.0f });
        };

        //if theres no count in place, widen description
        if (this->count_lbl->getStringLength() == 0)
        {
            this->desc_lbl->setContentSize({ 300.0f, 44.0f });
        }
    };
    this->schedule(reposition_labels, 0.1f, "reposition_labels");
    reposition_labels(0);

};

void NuItem::set_touch_ended_callback(std::function<void(void)> callback)
{
    this->button->addTouchEventListener([this, callback](Ref* sender, ui::Widget::TouchEventType type)
    {
        if (type == ui::Widget::TouchEventType::ENDED)
        {
            callback();

            this->button->runAction(FShake::actionWithDuration(0.1f, 5.0f));
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
    if (cost != ""){ //cost has content
        this->cost_lbl->setString("$"+cost);
    } else { //cost is blank, widen description TODO use orig_buttons positions so this isnt hardcoded
        this->cost_lbl->setString("");
    }
};

void NuItem::set_count_lbl(res_count_t count)
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

    res_count_t cost = this->get_cost();
    res_count_t total_coins = BEATUP->get_total_coins();

    this->cost_lbl->setTextColor(Color4B::WHITE);

    if (this->get_been_bought())
    {
        this->set_cost_lbl("---");
        try_set_enable(false);
        this->cost_lbl->setTextColor(Color4B::GRAY);
    } 
    else if (total_coins < cost)
    {
        try_set_enable(false);
        this->set_cost_lbl(beautify_double(std::round(this->get_cost())));

        Color3B color = { 243, 162, 173 };
        float tint = 0.4f;
        Color3B text_color = Color3B(
            color.r*tint,
            color.g*tint,
            color.b*tint
            );

        this->cost_lbl->setTextColor(Color4B(Color3B::RED));
        this->button->setColor(color);
    }
    else
    {
        try_set_enable(true);
        this->set_cost_lbl(beautify_double((double)std::round(this->get_cost())));

        this->button->setColor(Color3B::WHITE);
    };


};

void BuildingNuItem::my_init(std::shared_ptr<Building> building, Node* parent)
{
    NuItem::my_init(parent);
    this->building = building;
};

void RecipeNuItem::other_init(spRecipe recipe)
{
    this->recipe = recipe;
    this->set_touch_ended_callback([this]() {
        CCLOG("trying to consume %s recipe", this->recipe->name.c_str());
        this->building->consume_recipe(this->recipe.get());
    });

    if (recipe->_callback == NULL)
    {
        CCLOG("no callback set, using default, recipe %s", this->recipe->name.c_str());
        recipe->_callback = [this]() {
            for (auto pair : this->recipe->outputs) {
                Ingredient::SubType ing_type = pair.first;
                res_count_t count = pair.second;
                this->building->create_ingredients(ing_type, count);
            };
        };
    }
    else
    {
        CCLOG("callback set, using existing callback, recipe %s", this->recipe->name.c_str());
    }
}

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
    res_count_t result_count = 0;
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

bool UpgradeBuildingShopNuItem::get_been_bought()
{
    return this->_been_bought;
}

void UpgradeBuildingShopNuItem::set_been_bought(bool val)
{
    this->_been_bought = val;
}

void UpgradeBuildingShopNuItem::my_init(spBuilding building, Node* parent, int building_level)
{
    ShopNuItem::my_init(parent, "upgrade_building");

    this->building = building;
    this->building_level = building_level;

    this->set_title(building->name);
    this->set_description("Upgrade this building, increasing its storage");

    this->set_cost_lbl(std::to_string(100*this->building_level));
    this->_shop_cost = 100*this->building_level;

    auto update_func = [this](float dt) {
        this->set_been_bought(this->building->building_level >= this->building_level);
    };
    this->schedule(update_func, 0.1f, "set_enabled");
    update_func(0);

    auto touch_ended_cb = [this](){
        res_count_t cost = this->get_cost();
        double total_coins = BEATUP->get_total_coins();

        if (cost <= total_coins)
        {
            CCLOG("Bought level %i", this->building_level);

            BEATUP->add_total_coin(-((double)(cost)));

            auto building = BUILDUP->get_target_building();
            building->building_level = this->building_level;

            this->update_func(0);
        }
    };
    this->set_touch_ended_callback(touch_ended_cb);
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

    this->set_title("Buy "+harvester_name);
}

void HarvesterShopNuItem::my_init(Node* parent, WorkerSubType harv_type, Ingredient::SubType ing_type)
{
    ShopNuItem::my_init(parent, "harvester_buy");

    this->harv_type = harv_type;
    this->ing_type = ing_type;

    this->_shop_cost = 25;
    this->set_cost_lbl("25");

    this->my_init_title();
    this->my_init_sprite();
    this->my_init_touch_ended_callback();
    this->my_init_update_callback();

    this->update_func(0);
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

    std::string set_path;
    auto set_paths = gen_paths("set", 4);
    if (this->harv_type == WorkerSubType::One) { set_path = set_paths.at(0); }
    else if (this->harv_type == WorkerSubType::Two) { set_path = set_paths.at(1); }
    else if (this->harv_type == WorkerSubType::Three) { set_path = set_paths.at(2); }
    else if (this->harv_type == WorkerSubType::Four) { set_path = set_paths.at(3); }
    else { set_path = "set_0.png"; }

    std::vector<std::string> sprites = {
        set_path,
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
        res_count_t cost = this->get_cost();
        double total_coins = BEATUP->get_total_coins();

        if (cost <= total_coins)
        {
            CCLOG("HarvesterShopNuItem bought a harvester");
            BEATUP->add_total_coin(-((double)(cost)));
            auto building = BUILDUP->get_target_building();

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
        res_count_t harvesters_owned = map_get(building->harvesters, {this->harv_type, building->punched_sub_type}, 0);
        this->set_count_lbl(harvesters_owned);
        this->_shop_cost = Harvester::get_base_shop_cost(this->harv_type) * std::pow(1.15f, std::max((long double)0.0, harvesters_owned));

        std::stringstream ss;
        auto harvested_count = Harvester::get_to_harvest_count(this->harv_type);
        ss << "Adds " << harvested_count << " " << Ingredient::type_to_string(building->punched_sub_type) << " per sec";
        this->set_description(ss.str());
    };
    this->schedule(update_harvesters_cb, 0.1f, "harvester_count");
    update_harvesters_cb(0);

};

void SalesmanShopNuItem::my_init_touch_ended_callback()
{
    this->set_touch_ended_callback([this]()
    {
        res_count_t cost = this->get_cost();
        double total_coins = BEATUP->get_total_coins();

        if (cost <= total_coins)
        {
            CCLOG("SalesmanShopNuItem bought a salesman");
            BEATUP->add_total_coin(-((double)(cost)));
            auto building = BUILDUP->get_target_building();

            auto harvester_count = map_get(building->salesmen, { harv_type, ing_type }, 0);
            harvester_count++;
            building->salesmen[{ harv_type, ing_type }] = harvester_count;
            this->update_func(0);
        }
    });

};

void SalesmanShopNuItem::my_init_update_callback()
{
    auto update_harvesters_cb = [this](float dt) {
        auto building = BUILDUP->get_target_building();
        res_count_t harvesters_owned = map_get(building->salesmen, {this->harv_type, building->punched_sub_type}, 0);
        this->set_count_lbl(harvesters_owned);
        this->_shop_cost = Salesman::get_base_shop_cost(this->harv_type) * std::pow(1.15f, std::max((long double)0.0, harvesters_owned));

        std::stringstream ss;
        auto sold_count = Salesman::get_to_sell_count(this->harv_type);
        ss << "Sells " << sold_count << " " << Ingredient::type_to_string(building->punched_sub_type) << " per sec";
        this->set_description(ss.str());
    };
    this->schedule(update_harvesters_cb, 0.1f, "harvester_count");
    update_harvesters_cb(0);

};

void SalesmanShopNuItem::my_init_title()
{
    std::vector<std::string> names = {
        "Samson", "Sonny", "Smokey", "Sinead", "Sebastian", "Saul", "Stan",
        "Scott", "Spencer", "Sam", "Seth", "Stefan", "Jenson", "Shiloh",
        "Sequoia", "Sergeo", "Seren", "Seamus", "Spartacus", "Spike", 
        "Siegfried", "Sylvain"
    };
    std::string harvester_name = pick_one(names);

    this->set_title("Buy "+harvester_name);
}

void SalesmanShopNuItem::my_init_sprite()
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

    std::string set_path;
    auto set_paths = gen_paths("set", 4);
    if (this->harv_type == WorkerSubType::One) { set_path = set_paths.at(0); }
    else if (this->harv_type == WorkerSubType::Two) { set_path = set_paths.at(1); }
    else if (this->harv_type == WorkerSubType::Three) { set_path = set_paths.at(2); }
    else if (this->harv_type == WorkerSubType::Four) { set_path = set_paths.at(3); }
    else { set_path = "set_0.png"; }

    std::vector<std::string> sprites = {
        set_path,
        pick_one(gen_paths("body", 49)),
        pick_one(gen_paths("headwear", 49)),
        pick_one(gen_paths("legs", 22)),
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
                BEATUP->add_total_coin(-(double)building->get_cost());

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

    for (auto recipe : building->data->get_all_recipes())
    {
        auto convert_item = RecipeNuItem::create();

        convert_item->my_init(this->building, scrollview);
        convert_item->other_init(recipe);
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
