#include "NuMenu.h"
#include <numeric>

#include <editor-support/cocostudio/ActionTimeline/CSLoader.h>

#include "Util.h"
#include "MiscUI.h"
#include "FShake.h"

#include "HouseBuilding.h"
#include "Beatup.h"
#include "StaticData.h"
#include "Worker.h"
#include "GameLogic.h"
#include "Recipe.h"
#include "Technology.h"

#include <random>

USING_NS_CC;

cocos2d::ui::Button* NuItem::orig_button = NULL;
cocos2d::TTFConfig NuItem::ttf_config = TTFConfig("pixelmix.ttf", 16, GlyphCollection::CUSTOM, "\"!#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~¡¢£¤¥¦§¨©ª«¬­®¯°±²³´µ¶·¸¹º»¼½¾¿ÀÁÂÃÄÅÆÇÈÉÊËÌÍÎÏÐÑÒÓÔÕÖ×ØÙÚÛÜÝÞßàáâãäåæçèéêëìíîïðñòóôõö÷øùúûüýþ \n", false, 2);

NuItem* NuItem::create(cocos2d::Node* parent)
{

    NuItem* pRet = new(std::nothrow) NuItem();
    if (pRet && pRet->init(parent))
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
}

bool NuItem::init(cocos2d::Node* parent)
{
    ui::Widget::init();

    //TTFConfig ttf_config = TTFConfig("pixelmix.ttf", 16, GlyphCollection::ASCII, nullptr, false, 2);
    auto set_renderer = [/*ttf_config*/](ui::Text* text_node){
        Label* renderer = (Label*)text_node->getVirtualRenderer();
        renderer->setTTFConfig(NuItem::ttf_config);
    };


    auto setup_text_node = [set_renderer](ui::Text* text_node)
    {
        Label* renderer = (Label*)text_node->getVirtualRenderer();
        set_aliasing(text_node, true);
        renderer->setOverflow(Label::Overflow::SHRINK);
        set_renderer(text_node);
    };

    if (NuItem::orig_button == NULL)
    {
        CCLOG("creating NuItem::orig_button, should only happen once");
        auto inst = cocos2d::CSLoader::getInstance();
        NuItem::orig_button = static_cast<cocos2d::ui::Button*>(inst->createNode("editor/buttons/menu_item.csb")->getChildByName("menu_item_btn"));
        load_default_button_textures(NuItem::orig_button);
        NuItem::orig_button->retain();

        ((ui::Text*)NuItem::orig_button->getChildByName("title_panel")->getChildByName("title_lbl"))->setString(std::string("", 200));
        ((ui::Text*)NuItem::orig_button->getChildByName("description_panel")->getChildByName("description_lbl"))->setString(std::string("", 200));
        ((ui::Text*)NuItem::orig_button->getChildByName("cost_panel")->getChildByName("cost_lbl"))->setString(std::string("", 200));
        ((ui::Text*)NuItem::orig_button->getChildByName("cost_panel")->getChildByName("count_lbl"))->setString(std::string("", 200));

    };

    //clone orig button, background is still messed though
    this->button = (ui::Button*)NuItem::orig_button->clone();
    this->button->setCascadeColorEnabled(true);
    for (auto child : this->button->getChildren())
    {
        child->setCascadeColorEnabled(true);
    }

    load_default_button_textures(this->button); //TODO figure out why this is still needed, why the clone doesnt do this for me

    this->button->removeFromParent();


    parent->addChild(this); //FIXME hack to get the selector to run. ideally the button would be a child of this but then button doesnt go on the right spot
    parent->addChild(button);

    this->item_icon = static_cast<cocos2d::ui::ImageView*>(button->getChildByName("item_icon"));
    this->title_lbl = static_cast<cocos2d::ui::Text*>(button->getChildByName("title_panel")->getChildByName("title_lbl"));
    setup_text_node(this->title_lbl);
    this->desc_lbl = static_cast<cocos2d::ui::Text*>(button->getChildByName("description_panel")->getChildByName("description_lbl"));
    setup_text_node(this->desc_lbl);
    this->cost_lbl = static_cast<cocos2d::ui::Text*>(button->getChildByName("cost_panel")->getChildByName("cost_lbl"));
    setup_text_node(this->cost_lbl);
    this->count_lbl = static_cast<cocos2d::ui::Text*>(button->getChildByName("cost_panel")->getChildByName("count_lbl"));
    setup_text_node(this->count_lbl);

    this->schedule(CC_SCHEDULE_SELECTOR(NuItem::update_func));

    auto reposition_labels = [this](float dt)
    {
        //defaults
        this->cost_lbl->setPosition(NuItem::orig_button->getChildByName("cost_panel")->getChildByName("cost_lbl")->getPosition());
        this->count_lbl->setPosition(NuItem::orig_button->getChildByName("cost_panel")->getChildByName("count_lbl")->getPosition());

        auto orig_desc_pos = NuItem::orig_button->getChildByName("description_panel")->getChildByName("description_lbl")->getPosition();
        this->desc_lbl->setPosition(orig_desc_pos);
        auto orig_desc_size = NuItem::orig_button->getChildByName("description_panel")->getChildByName("description_lbl")->getContentSize();
        this->desc_lbl->setContentSize(orig_desc_size);

        //if theres no cost, move count up, widen description
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

    return true;

};

void NuItem::set_touch_ended_callback(std::function<void(void)> callback)
{
    this->button->addTouchEventListener([this, callback](Ref* sender, ui::Widget::TouchEventType type)
    {
        if (type == ui::Widget::TouchEventType::ENDED)
        {
            callback();

            do_vibrate(32); //vibrate for two frames
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
    //CCLOG(title.c_str());
    this->title_lbl->setString(title);
};

void NuItem::set_description(std::string description)
{
    //CCLOG(description.c_str());
    this->desc_lbl->setString(description);
};

void NuItem::set_cost_lbl(std::string cost)
{
    if (cost != ""){ //cost has content
        //CCLOG(cost.c_str());
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


bool ShopNuItem::init(Node* parent, std::string id_key)
{
    NuItem::init(parent);

    this->id_key = id_key;
    this->_shop_cost = -1;

    return true;
}

bool ShopNuItem::custom_status_check(float dt)
{
    return true;
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
    res_count_t rounded_cost = std::round(this->get_cost());

    if (this->get_been_bought())
    {
        this->set_cost_lbl("---");
        try_set_enable(false);
        try_set_text_color(this->cost_lbl, Color4B::GRAY);
        try_set_node_color(this->button, Color3B::WHITE);
    } 
    else if (total_coins < cost || !this->custom_status_check(dt))
    {
        try_set_enable(false);
        this->set_cost_lbl(beautify_double(rounded_cost));

        Color3B color = { 243, 162, 173 };
        try_set_text_color(this->cost_lbl, Color4B::RED);
        try_set_node_color(this->button, color);
    }
    else
    {
        try_set_enable(true);
        this->set_cost_lbl(beautify_double(rounded_cost));

        try_set_node_color(this->button, Color3B::WHITE);
    };



}

BuildingShopNuItem* BuildingShopNuItem::create(cocos2d::Node* parent, spBuilding building)
{
    BuildingShopNuItem* pRet = new(std::nothrow) BuildingShopNuItem();
    if (pRet && pRet->init(parent, building))
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

BuildingNuItem* BuildingNuItem::create(Node* parent, std::shared_ptr<Building> building)
{
    BuildingNuItem* pRet = new(std::nothrow) BuildingNuItem();
    if (pRet && pRet->init(parent, building))
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
}

bool BuildingNuItem::init(Node* parent, std::shared_ptr<Building> building)
{
    NuItem::init(parent);
    this->building = building;

    return true;
}

RecipeNuItem* RecipeNuItem::create(Node* parent, spBuilding building)
{
    RecipeNuItem* pRet = new(std::nothrow) RecipeNuItem();
    if (pRet && pRet->init(parent, building))
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

void RecipeNuItem::other_init(spRecipe recipe)
{
    this->recipe = recipe;
    this->set_touch_ended_callback([this]() {
        CCLOG("trying to consume %s recipe", this->recipe->name.c_str());
        this->building->consume_recipe(this->recipe.get());
    });

    if (recipe->outputs.empty() == false)
    {
        std::string ing_str = Ingredient::type_to_string((*this->recipe->outputs.begin()).first);
        auto res_data = IngredientData(ing_str);
        this->item_icon->loadTexture(res_data.get_img_large());
        ((cocos2d::ui::Scale9Sprite*)(this->item_icon->getVirtualRenderer()))->getSprite()->getTexture()->setAliasTexParameters();
    };

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
}

TechNuItem* TechNuItem::create(Node* parent, spBuilding building)
{
    TechNuItem* pRet = new(std::nothrow) TechNuItem();
    if (pRet && pRet->init(parent, building))
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

void TechNuItem::other_init(spTechnology technology)
{
    this->technology = technology;
    this->set_touch_ended_callback([this]() {
        CCLOG("trying to consume %s technology", "A Technology");
        this->building->consume_recipe(this->technology->get_ingredient_requirements(this->building).get());
        this->technology->set_been_unlocked(true);

        //TODO fix this duplicating techs already in the vector, might need to
        //go to a map like resources
        //this->building->techtree->technologies.push_back(this->technology);
        res_count_t def = 0.0;
        res_count_t num_researched = map_get(this->building->techtree->tech_map, this->technology->sub_type, def);
        this->building->techtree->tech_map[this->technology->sub_type] = num_researched + 1;
    });

}

void TechNuItem::update_func(float dt)
{
    //FIXME, this lets someone press a button early i think
    // and avoid having the button disabled. Not sure.
    if (this->button->isHighlighted())
    {
        return;
    };

    auto ing_requirements = this->technology->get_ingredient_requirements(this->building);
    bool tech_is_satisfied = ing_requirements->is_satisfied(this->building->ingredients);
    if (tech_is_satisfied)
    {
        if (this->technology->get_been_unlocked() == false)
        {
            this->try_set_enable(true);
        }
        else
        {
            //if its not unique, means you can buy it again
            if (this->technology->is_unique == false)
            {
                this->try_set_enable(true);
            }
            else
            {
                this->try_set_enable(false);
            };
        }
    }
    else {
        this->try_set_enable(false);
    }

    spRecipe recipe = this->technology->get_ingredient_requirements(this->building);
    this->set_description(recipe->description);
}

ShopNuItem* ShopNuItem::create(cocos2d::Node* parent)
{
    ShopNuItem* pRet = new(std::nothrow) ShopNuItem();
    if (pRet && pRet->init(parent))
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

bool BuildingShopNuItem::init(Node* parent, spBuilding building)
{
    ShopNuItem::init(parent, building->id_key);
    this->building = building;

    this->set_image(building->data->get_img_large());
    this->set_title(building->name);
    // this->set_description(building->data->get_description());

    this->set_cost_lbl(std::to_string(building->get_cost()));
    this->_shop_cost = building->get_cost();

    this->set_been_bought(false);

    return true;
}

bool BuildingShopNuItem::get_been_bought()
{
    return this->_been_bought;
}

void BuildingShopNuItem::set_been_bought(bool val)
{
    this->_been_bought = val;
}

UpgradeBuildingShopNuItem* UpgradeBuildingShopNuItem::create(cocos2d::Node* parent, spBuilding building)
{
    UpgradeBuildingShopNuItem* pRet = new(std::nothrow) UpgradeBuildingShopNuItem();
    if (pRet && pRet->init(parent, building))
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
}

bool UpgradeBuildingShopNuItem::my_init(int building_level)
{
    this->building_level = building_level;

    std::stringstream ss;
    ss << "Level " << building_level;
    this->set_title(ss.str());
    this->set_description("Upgrade building\nincreasing its storage");

    long double shop_cost = scale_number(10.0L, ((res_count_t)this->building_level)-1.0L, 10.5L);
    this->_shop_cost = shop_cost;

    auto update_been_bought = [this](float dt) {
        this->set_been_bought(this->building->building_level >= this->building_level);
    };
    this->schedule(update_been_bought, 0.1f, "set_enabled");
    update_been_bought(0);

    auto touch_ended_cb = [this](){
        res_count_t cost = this->get_cost();
        double total_coins = BEATUP->get_total_coins();

        if (cost <= total_coins)
        {
            CCLOG("Bought level %i", this->building_level);

            BEATUP->add_total_coin(-((double)(cost)));

            auto building = BUILDUP->get_target_building();
            building->building_level = this->building_level;

            auto explosion_parts = ParticleSystemQuad::create("particles/upgrade.plist");
            explosion_parts->setPosition({570, 300});
            Scene* scene = Director::getInstance()->getRunningScene();
            scene->runAction(FShake::actionWithDuration(0.25f, 2.5f));
            scene->addChild(explosion_parts);

            do_vibrate(250);

            this->update_func(0);
        }
    };
    this->set_touch_ended_callback(touch_ended_cb);
    this->update_func(0);

    return true;
}

bool UpgradeBuildingShopNuItem::custom_status_check(float dt)
{
    //only activate menuitem if the building is one level below item's building level
    return this->building->building_level == this->building_level - 1;
};

HarvesterShopNuItem* HarvesterShopNuItem::create(Node* parent, spBuilding building)
{
    HarvesterShopNuItem* pRet = new(std::nothrow) HarvesterShopNuItem();
    if (pRet && pRet->init(parent, building))
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
}

std::mt19937 HarvesterShopNuItem::get_generator()
{
    auto gen = std::mt19937(std::default_random_engine{}());
    auto seeder = std::hash<std::string>();
    gen.seed(seeder(this->building->name+this->building->name)+ (int)this->harv_type);

    return gen;
}

void HarvesterShopNuItem::my_init_title()
{
    std::vector<std::string> names = {
        "Jamal", "Josh", "James", "Jimmy", "Jonathan", "Javert", "John",
        "Jackson", "Jax", "Jimothy", "Jasper", "Joe", "Jenson", "Jack",
        "Justin", "Jaleel", "Jamar", "Jesse", "Jaromir", "Jebediah", 
        "Johan", "Jericho"
    };

    auto gen = get_generator();
    std::string harvester_name = pick_one(names, &gen);

    this->set_title("Buy "+harvester_name);
}

bool HarvesterShopNuItem::init(Node* parent, spBuilding building)
{
    BuildingShopNuItem::init(parent, building);
    return true;
}

void HarvesterShopNuItem::my_init(WorkerSubType harv_type, Ingredient::SubType ing_type)
{
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

    auto gen = get_generator();
    std::vector<std::string> sprites = {
        set_path,
        pick_one(gen_paths("body", 49), &gen),
        pick_one(gen_paths("headwear", 49), &gen),
        pick_one(gen_paths("legs", 22), &gen),
        pick_one(gen_paths("shield", 49), &gen),
        pick_one(gen_paths("weapon", 49), &gen)
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

            res_count_t def = 0.0;
            work_ing_t map = { harv_type, ing_type };
            auto harvester_count = map_get(building->harvesters, map, def);
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
        res_count_t def = 0.0;
        work_ing_t pair = { this->harv_type, building->punched_sub_type };
        res_count_t harvesters_owned = map_get(building->harvesters, pair, def);
        this->set_count_lbl(harvesters_owned);
        this->_shop_cost = scale_number(Harvester::get_base_shop_cost(this->harv_type), harvesters_owned, 1.15L);

        std::stringstream ss;
        auto harvested_count = Harvester::get_to_harvest_count(this->harv_type);
        ss << "Adds " << harvested_count << " " << Ingredient::type_to_string(building->punched_sub_type) << "\nper sec";
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

            res_count_t def = 0.0;
            work_ing_t pair = { harv_type, ing_type };
            auto harvester_count = map_get(building->salesmen, pair, def);
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
        res_count_t def = 0.0;
        work_ing_t pair = { this->harv_type, building->punched_sub_type };
        res_count_t harvesters_owned = map_get(building->salesmen, pair, def);
        this->set_count_lbl(harvesters_owned);
        this->_shop_cost = scale_number(Salesman::get_base_shop_cost(this->harv_type), harvesters_owned, 1.15L);

        std::stringstream ss;
        auto sold_count = Salesman::get_to_sell_count(this->harv_type) * building->building_level;
        ss << "Sells " << sold_count << " " << Ingredient::type_to_string(building->punched_sub_type) << "\nper sec";
        this->set_description(ss.str());
    };
    this->schedule(update_harvesters_cb, 0.1f, "harvester_count");
    update_harvesters_cb(0);

}

bool SalesmanShopNuItem::custom_status_check(float dt)
{
    spBuilding target_building = BUILDUP->get_target_building();

    // check if high enough building level to fit more
    bool is_enabled = false;
    res_count_t def = 0.0;
    work_ing_t pair = { this->harv_type, target_building->punched_sub_type };
    res_count_t harvesters_owned = map_get(target_building->salesmen, pair, def);

    if (harvesters_owned < target_building->get_storage_space())
    {
        is_enabled = true;
    };

    return is_enabled;
};

SalesmanShopNuItem* SalesmanShopNuItem::create(Node* parent, spBuilding building)
{
    SalesmanShopNuItem* pRet = new(std::nothrow) SalesmanShopNuItem();
    if (pRet && pRet->init(parent, building))
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
}

void SalesmanShopNuItem::my_init_title()
{
    std::vector<std::string> names = {
        "Samson", "Sonny", "Smokey", "Sinead", "Sebastian", "Saul", "Stan",
        "Scott", "Spencer", "Sam", "Seth", "Stefan", "Jenson", "Shiloh",
        "Sequoia", "Sergeo", "Seren", "Seamus", "Spartacus", "Spike", 
        "Siegfried", "Sylvain"
    };

    auto gen = get_generator();
    std::string salesman_name = pick_one(names, &gen);

    this->set_title("Buy "+salesman_name);
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

    auto gen = get_generator();
    std::vector<std::string> sprites = {
        "set_3.png",
        pick_one(gen_paths("body", 5), &gen),
        pick_one(gen_paths("headwear", 5), &gen),
        pick_one(gen_paths("legs", 5), &gen),
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

void ConsumerShopNuItem::my_init_touch_ended_callback()
{
    this->set_touch_ended_callback([this]()
    {
        res_count_t cost = this->get_cost();
        double total_coins = BEATUP->get_total_coins();

        if (cost <= total_coins)
        {
            CCLOG("ConsumerShopNuItem bought a consumer");
            BEATUP->add_total_coin(-((double)(cost)));
            auto building = BUILDUP->get_target_building();

            res_count_t def = 0.0;
            work_ing_t pair = { harv_type, ing_type };
            auto harvester_count = map_get(building->consumers, pair, def);
            harvester_count++;
            building->consumers[{ harv_type, ing_type }] = harvester_count;
            this->update_func(0);
        }
    });

};

void ConsumerShopNuItem::my_init_update_callback()
{
    auto update_harvesters_cb = [this](float dt) {
        auto building = BUILDUP->get_target_building();
        work_ing_t pair = {this->harv_type, building->punched_sub_type};
        res_count_t def = 0;
        res_count_t harvesters_owned = map_get(building->consumers, pair, def);
        this->set_count_lbl(harvesters_owned);
        this->_shop_cost = scale_number(Salesman::get_base_shop_cost(this->harv_type), harvesters_owned, 1.15L);

        std::stringstream ss;
        auto sold_count = 5 * building->building_level;
        ss << "Consumes " << sold_count << " " << Ingredient::type_to_string(this->consumed_type) << "\nper sec (if hurt)";
        this->set_description(ss.str());
    };
    this->schedule(update_harvesters_cb, 0.1f, "harvester_count");
    update_harvesters_cb(0);

}

bool ConsumerShopNuItem::custom_status_check(float dt)
{
    spBuilding target_building = BUILDUP->get_target_building();

    // check if high enough building level to fit more
    bool is_enabled = false;
    work_ing_t pair = { this->harv_type, target_building->punched_sub_type };
    res_count_t def = 0.0;
    res_count_t harvesters_owned = map_get( target_building->consumers, pair, def);

    if (harvesters_owned < target_building->get_storage_space())
    {
        is_enabled = true;
    };

    return is_enabled;
};

ConsumerShopNuItem* ConsumerShopNuItem::create(Node* parent, spBuilding building, IngredientSubType consumed_type)
{
    ConsumerShopNuItem* pRet = new(std::nothrow) ConsumerShopNuItem();
    pRet->consumed_type = consumed_type;
    if (pRet && pRet->init(parent, building))
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
}

void ConsumerShopNuItem::my_init_title()
{
    std::vector<std::string> names = {
        "Charles", "Chaplin", "Champlain", "Connor", "Chap", "Chris", "Caleb",
        "Cameron", "Cyrill", "Cyrus", "Cody", "Corey", "Caiden", "Carter"
    };
    auto gen = get_generator();
    std::string consumer_name = pick_one(names, &gen);

    this->set_title("Buy "+consumer_name);
}

void ConsumerShopNuItem::my_init_sprite()
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

    auto gen = get_generator();
    std::vector<std::string> sprites = {
        set_path,
        pick_one(gen_paths("body", 49), &gen),
        pick_one(gen_paths("headwear", 49), &gen),
        pick_one(gen_paths("legs", 22), &gen),
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

void CombatShopNuItem::my_init_touch_ended_callback()
{
    this->set_touch_ended_callback([this]()
    {
        res_count_t cost = this->get_cost();
        double total_coins = BEATUP->get_total_coins();

        if (cost <= total_coins)
        {
            //enable this combat item
            this->update_func(0);
        }
    });

};

void CombatShopNuItem::my_init_update_callback()
{
    auto update_harvesters_cb = [this](float dt) {
        auto building = BUILDUP->get_target_building();

        res_count_t combats_owned = 0;
        this->set_count_lbl(combats_owned);
        this->_shop_cost = scale_number(10.0L, combats_owned, 1.15L);

        std::stringstream ss;
        ss << "Buffs your fighting skills";
        this->set_description(ss.str());
    };
    this->schedule(update_harvesters_cb, 0.1f, "update_combat");
    update_harvesters_cb(0);

}

bool CombatShopNuItem::custom_status_check(float dt)
{
    //TODO implement a check to see if this is available
    spBuilding target_building = BUILDUP->get_target_building();

    bool is_enabled = true;

    return is_enabled;
};

CombatShopNuItem* CombatShopNuItem::create(Node* parent, spBuilding building)
{
    CombatShopNuItem* pRet = new(std::nothrow) CombatShopNuItem();
    if (pRet && pRet->init(parent, building))
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
}

void CombatShopNuItem::my_init_title()
{
    std::string title = "CombatTool";
    this->set_title("Buy "+title);
}

void CombatShopNuItem::my_init_sprite()
{
    //TODO use a custom sprite
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
    auto scrollview = this->scrollable;
    this->create_inventory_item(scrollview);

    for (auto recipe : building->data->get_all_recipes())
    {
       auto convert_item = RecipeNuItem::create(scrollview, building);
       convert_item->other_init(recipe);
    }

};

void BuildingNuMenu::create_inventory_item(cocos2d::Node* parent)
{

};
