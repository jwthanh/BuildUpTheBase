#include "NuMenu.h"
#include <numeric>
#include <sstream>
#include <random>

#include "Util.h"
#include "MiscUI.h"
#include "FShake.h"

#include "HouseBuilding.h"
#include "Buildup.h"
#include "Beatup.h"
#include "StaticData.h"
#include "Worker.h"
#include "GameLogic.h"
#include "Recipe.h"
#include "Technology.h"
#include "HarvestScene.h"
#include "NodeBuilder.h"
#include "Ingredients.h"

#include "magic_particles/MagicEmitter.h"

#include "2d/CCParticleSystemQuad.h"
#include "base/CCDirector.h"
#include "2d/CCRenderTexture.h"
#include "base/CCEventDispatcher.h"
#include "ui/UIText.h"
#include "ui/UIButton.h"
#include "ui/UIImageView.h"
#include "ui/UIScale9Sprite.h"
#include <cocostudio/CocosStudioExtension.h>
#include "ui/UILayout.h"
#include "ui/UIListView.h"

#include "utilities/vibration.h"
#include "external/easylogging.h"
#include "banking/Bank.h"


USING_NS_CC;

cocos2d::ui::Button* NuItem::orig_button = NULL;
cocos2d::TTFConfig NuItem::ttf_config = TTFConfig(
    "pixelmix.ttf",
    16,
    GlyphCollection::CUSTOM,
    "\"!#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~¡¢£¤¥¦§¨©ª«¬­®¯°±²³´µ¶·¸¹º»¼½¾¿ÀÁÂÃÄÅÆÇÈÉÊËÌÍÎÏÐÑÒÓÔÕÖ×ØÙÚÛÜÝÞßàáâãäåæçèéêëìíîïðñòóôõö÷øùúûüýþ \n",
    false,
    2
);

NuItem* NuItem::create(cocos2d::ui::Widget* parent)
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

    auto setup_text_node = [](ui::Text* text_node)
    {
        Label* renderer = (Label*)text_node->getVirtualRenderer();
        set_aliasing(text_node, true);
        renderer->setOverflow(Label::Overflow::SHRINK);
        renderer->setTTFConfig(NuItem::ttf_config);
    };

    if (NuItem::orig_button == NULL)
    {
        NuItem::orig_button = static_cast<cocos2d::ui::Button*>(get_prebuilt_node_from_csb("editor/buttons/menu_item.csb")->getChildByName("menu_item_btn"));
        load_default_button_textures(NuItem::orig_button);
        NuItem::orig_button->retain();
        NuItem::orig_button->setSwallowTouches(false);

        ((ui::Text*)NuItem::orig_button->getChildByName("title_panel")->getChildByName("title_lbl"))->setString(std::string("", 200));
        ((ui::Text*)NuItem::orig_button->getChildByName("description_panel")->getChildByName("description_lbl"))->setString(std::string("", 200));
        ((ui::Text*)NuItem::orig_button->getChildByName("cost_panel")->getChildByName("cost_lbl"))->setString(std::string("", 200));
        ((ui::Text*)NuItem::orig_button->getChildByName("cost_panel")->getChildByName("count_lbl"))->setString(std::string("", 200));

    };

    //clone orig button, background is still messed though
    this->button = (ui::Button*)NuItem::orig_button->clone();
    this->button->setSwallowTouches(false);


    this->button->setCascadeColorEnabled(true);
    for (auto child : this->button->getChildren())
    {
        child->setCascadeColorEnabled(true);
    }

    load_default_button_textures(this->button); //TODO figure out why this is still needed, why the clone doesnt do this for me

    this->button->removeFromParent();

    parent->addChild(this); //FIXME hack to get the selector to run. ideally the button would be a child of this but then button doesnt go on the right spot
    ui::ListView* scrollview_parent = dynamic_cast<ui::ListView*>(parent);
    if (scrollview_parent)
    {
        scrollview_parent->pushBackCustomItem(button);
    }
    else
    {
        CCLOG("!!!!\n\nadding NuItem to not a listview\n\n\n");
        parent->addChild(button);
    }

    this->_original_image_path = "";

    this->item_icon = dynamic_cast<cocos2d::ui::ImageView*>(button->getChildByName("item_icon"));
    this->title_lbl = dynamic_cast<cocos2d::ui::Text*>(button->getChildByName("title_panel")->getChildByName("title_lbl"));
    setup_text_node(this->title_lbl);
    this->desc_lbl = dynamic_cast<cocos2d::ui::Text*>(button->getChildByName("description_panel")->getChildByName("description_lbl"));
    setup_text_node(this->desc_lbl);
    this->cost_lbl = dynamic_cast<cocos2d::ui::Text*>(button->getChildByName("cost_panel")->getChildByName("cost_lbl"));
    setup_text_node(this->cost_lbl);
    this->count_lbl = dynamic_cast<cocos2d::ui::Text*>(button->getChildByName("cost_panel")->getChildByName("count_lbl"));
    setup_text_node(this->count_lbl);

    //progress stuff for achievement buttons
    this->current_lbl = dynamic_cast<cocos2d::ui::Text*>(button->getChildByName("progress_panel")->getChildByName("current_lbl"));
    this->middle_lbl = dynamic_cast<cocos2d::ui::Text*>(button->getChildByName("progress_panel")->getChildByName("middle_lbl"));
    this->total_lbl = dynamic_cast<cocos2d::ui::Text*>(button->getChildByName("progress_panel")->getChildByName("total_lbl"));
    this->set_progress_panel_visible(false);

    this->schedule(CC_SCHEDULE_SELECTOR(NuItem::update_func));

    auto reposition_labels = [this](float dt)
    {
        if (this->getParent()->isVisible() == false || this->getParent()->getParent()->isVisible() == false) { return; } //dont do anything if not visible

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
    this->schedule(reposition_labels, FPS_4, "reposition_labels");
    reposition_labels(0);

    return true;

};

void NuItem::update_func(float dt)
{
    if (this->getParent()->isVisible() == false || this->getParent()->getParent()->isVisible() == false) { return; } //dont do anything if not visible
    //either cooldown doesnt exist or the cooldown is over
    bool cooldown_satisfied = this->cooldown == NULL || this->cooldown->passed_threshold();
    if (cooldown_satisfied)
    {
        this->try_set_enable(true);
        this->set_image(this->_original_image_path);
    }
    else
    {
        this->try_set_enable(false);
        this->set_image("locked.png");
    }
};

void NuItem::set_touch_ended_callback(std::function<void(void)> callback)
{
    auto wrapped_callback = [callback, this]()
    {
        callback();
        this->button->runAction(FShake::actionWithDuration(0.1f, 5.0f));
    };
    bind_touch_ended(this->button, wrapped_callback);

};

void NuItem::set_original_image(std::string path)
{
    this->_original_image_path = path;
};

void NuItem::set_image(std::string path, ui::Widget::TextureResType texture_type)
{
    //same loaded image, do nothing
    if (this->item_icon->getRenderFile().file == path)
    {
        return;
    };

    image_view_scale9_hack(this->item_icon);
    this->item_icon->loadTexture(path, texture_type);

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
        this->cost_lbl->setString(this->get_currency_prefix()+cost);
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

void NuItem::set_progress_panel_visible(bool visible)
{
    try_set_visible(this->current_lbl, visible);
    try_set_visible(this->middle_lbl, visible);
    try_set_visible(this->total_lbl, visible);
};

void ShopNuItem::add_available_coins(res_count_t new_coins)
{
    BANK->pocket_or_bank_coins(new_coins);
};

res_count_t ShopNuItem::get_available_coins()
{
    return BEATUP->get_total_coins();
};

bool ShopNuItem::init(Node* parent, std::string id_key)
{
    NuItem::init(parent);

    this->id_key = id_key;
    this->_shop_cost = -1;
    this->_last_shop_cost = -1;

    return true;
}

bool ShopNuItem::custom_status_check(float dt)
{
    return true;
};

void ShopNuItem::update_func(float dt)
{
    if (this->getParent()->isVisible() == false || this->getParent()->getParent()->isVisible() == false) { return; } //dont do anything if not visible
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
    res_count_t total_coins = this->get_available_coins();

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

        // this->set_cost_lbl(beautify_double(rounded_cost));
        if (rounded_cost != this->_last_shop_cost) {
            this->set_cost_lbl(beautify_double(rounded_cost));
            this->_last_shop_cost = rounded_cost;
        };

        Color3B color = { 243, 162, 173 };
        try_set_text_color(this->cost_lbl, Color4B::RED);
        try_set_node_color(this->button, color);
    }
    else
    {
        try_set_enable(true);

        // this->set_cost_lbl(beautify_double(rounded_cost));
        if (rounded_cost != this->_last_shop_cost) {
            this->set_cost_lbl(beautify_double(rounded_cost));
            this->_last_shop_cost = rounded_cost;
        };

        try_set_node_color(this->button, Color3B::WHITE);
    };



}

BuildingShopNuItem* BuildingShopNuItem::create(cocos2d::ui::Widget* parent, spBuilding building)
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

BuildingNuItem* BuildingNuItem::create(cocos2d::ui::Widget* parent, std::shared_ptr<Building> building)
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

RecipeNuItem* RecipeNuItem::create(cocos2d::ui::Widget* parent, spBuilding building)
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
        IngredientData res_data(ing_str);
        this->item_icon->loadTexture(res_data.get_img_large());
        ((cocos2d::ui::Scale9Sprite*)(this->item_icon->getVirtualRenderer()))->getSprite()->getTexture()->setAliasTexParameters();
    };

    if (recipe->_callback == NULL)
    {
        CCLOG("no callback set, using default, recipe %s", this->recipe->name.c_str());
        recipe->_callback = [this]() {
            for (auto pair : this->recipe->outputs) {
                IngredientSubType ing_type = pair.first;
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
    if (this->getParent()->isVisible() == false || this->getParent()->getParent()->isVisible() == false) { return; } //dont do anything if not visible
    //FIXME, this lets someone press a button early i think
    // and avoid having the button disabled. Not sure.
    if (this->button->isHighlighted())
    {
        return;
    };

    auto& all_ingredients = BUILDUP->get_all_ingredients();
    if (this->recipe->is_satisfied(all_ingredients)){
        this->try_set_enable(true);
    } else {
        this->try_set_enable(false);
    }

    //TODO this'll get fucky when recipes output more than one type,
    // you make one recipe, the count'll go up by two
    res_count_t result_count = 0;
    for (auto output_ing : this->recipe->outputs) {
        auto out_type = output_ing.first;
        result_count += BUILDUP->count_ingredients(out_type);
    };
    this->set_count_lbl(result_count);
}

TechNuItem* TechNuItem::create(cocos2d::ui::Widget* parent, spBuilding building)
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

        res_count_t def = 0.0;
        res_count_t num_researched = map_get(this->building->techtree->tech_map, this->technology->sub_type, def);
        this->building->techtree->tech_map[this->technology->sub_type] = num_researched + 1;
    });

}

void TechNuItem::update_func(float dt)
{
    if (this->getParent()->isVisible() == false || this->getParent()->getParent()->isVisible() == false) { return; } //dont do anything if not visible
    //FIXME, this lets someone press a button early i think
    // and avoid having the button disabled. Not sure.
    if (this->button->isHighlighted())
    {
        return;
    };

    auto ing_requirements = this->technology->get_ingredient_requirements(this->building);
    auto& all_ingredients = BUILDUP->get_all_ingredients();
    bool tech_is_satisfied = ing_requirements->is_satisfied(all_ingredients);
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

TargetRecipeNuItem* TargetRecipeNuItem::create(cocos2d::ui::Widget* parent, spBuilding building)
{
    TargetRecipeNuItem* pRet = new(std::nothrow) TargetRecipeNuItem();
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

void TargetRecipeNuItem::other_init(spRecipe recipe)
{
    this->recipe = recipe;
    this->set_touch_ended_callback([this]() {
        cocos2d::Scene* scene = cocos2d::Director::getInstance()->getRunningScene();
        auto harvest_scene = dynamic_cast<HarvestScene*>(scene->getChildByName("HarvestScene"));
        if (harvest_scene)
        {
            harvest_scene->target_recipe = this->recipe;
            harvest_scene->removeChildByName("harvestable");

            auto arena_kill_panel = dynamic_cast<ui::Layout*>(harvest_scene->getChildByName("arena_kill_panel"));
            auto particle = MagicEmitter::create("TargetRecipeChanged");

            auto pos = arena_kill_panel->getPosition();
            particle->setPosition(pos);

            harvest_scene->addChild(particle);
            // CCLOG("target recipe changed to %s", this->recipe->name.c_str());
        }
        else
        {
            CCLOG("cant find HarvestScene");
        }
    });

}

void TargetRecipeNuItem::update_func(float dt)
{
    if (this->getParent()->isVisible() == false || this->getParent()->getParent()->isVisible() == false) { return; } //dont do anything if not visible
    //FIXME, this lets someone press a button early i think
    // and avoid having the button disabled. Not sure.
    if (this->button->isHighlighted())
    {
        return;
    };

    cocos2d::Scene* scene = cocos2d::Director::getInstance()->getRunningScene();
    auto harvest_scene = dynamic_cast<HarvestScene*>(scene->getChildByName("HarvestScene"));
    if (this->recipe == harvest_scene->target_recipe)
    {
        this->set_image("anvil.png");

        this->try_set_enable(false);
        Color3B selected_color = {82, 148, 126}; //green
        try_set_node_color(this->button, selected_color);

    }
    else {
        this->set_image("grey_boxCheckmark.png");

        this->try_set_enable(true);
        Color3B unselected_color = Color3B::WHITE;
        try_set_node_color(this->button, unselected_color);
    }

    spRecipe recipe = this->recipe;
    this->set_description(recipe->description);
}

UpgradeWalletNuItem* UpgradeWalletNuItem::create(cocos2d::ui::Widget* parent, spBuilding building)
{
    UpgradeWalletNuItem* pRet = new(std::nothrow) UpgradeWalletNuItem();
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

void UpgradeWalletNuItem::add_available_coins(res_count_t new_coins)
{
    GameLogic::getInstance()->add_city_investment(new_coins);
};

res_count_t UpgradeWalletNuItem::get_available_coins()
{
    return GameLogic::getInstance()->get_city_investment();
};


void UpgradeWalletNuItem::other_init()
{
    Technology technology = Technology(TechSubType::RaiseWalletCap);
    technology.set_been_unlocked(true);

    this->set_image("sell_all.png");

    this->set_touch_ended_callback([this]() {
        this->add_available_coins(-this->_shop_cost);

        Technology technology = Technology(TechSubType::RaiseWalletCap);
        auto marketplace = BUILDUP->city->building_by_name("The Marketplace");
        res_count_t def = 0.0;
        res_count_t num_researched = map_get(marketplace->techtree->tech_map, technology.sub_type, def);
        marketplace->techtree->tech_map[technology.sub_type] = num_researched + 1;
    });

    auto update_callback = [this](float dt) {
        if (this->getParent()->isVisible() == false || this->getParent()->getParent()->isVisible() == false) { return ; } //dont do anything if not visible
        //FIXME, this lets someone press a button early i think
        // and avoid having the button disabled. Not sure.
        if (this->button->isHighlighted())
        {
            return ;
        };
        std::stringstream ss;

        Technology technology = Technology(TechSubType::RaiseWalletCap);
        auto marketplace = BUILDUP->city->building_by_name("The Marketplace");

        res_count_t def = 0.0;
        res_count_t num_researched = map_get(marketplace->techtree->tech_map, technology.sub_type, def);

        ss << "lvl " << num_researched;
        this->count_lbl->setString(ss.str());
        ss.str("");

        this->_shop_cost = BEATUP->get_max_coin_storage() / 100.0f;
        this->set_cost_lbl(beautify_double(this->_shop_cost));

        ss << "Spend investments to increase.";

        this->set_description(ss.str());
    };
    this->schedule(update_callback, FPS_10, "update_wallet_callback");
    update_callback(0);

}

ShopNuItem* ShopNuItem::create(cocos2d::ui::Widget* parent)
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

UpgradeBuildingShopNuItem* UpgradeBuildingShopNuItem::create(cocos2d::ui::Widget* parent, spBuilding building)
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

const int MAX_BUILDING_LEVEL = 15;

bool UpgradeBuildingShopNuItem::my_init(int building_level)
{
    this->building_level = building_level;

    auto update_title_and_desc = [this](float dt) {
        std::stringstream ss;

        if (this->building_level <= MAX_BUILDING_LEVEL) {
            ss << "Level " << this->building_level;
            this->set_title(ss.str());
            this->set_description("Upgrade building\nincreasing its storage");
        } else {
            ss << "Max level (lv " << this->building->building_level << ")";
            this->set_title(ss.str());
            this->set_description("Upgraded to the max!");
        };
    };
    update_title_and_desc(0.0f);
    this->schedule(update_title_and_desc, FPS_60, "update_title_and_desc");

    long double shop_cost = scale_number(10.0L, ((res_count_t)this->building_level)-1.0L, 10.5L);
    this->_shop_cost = shop_cost;

    auto custom_update_func = [this](float dt) {
        if (this->building->building_level == this->building_level) {
            this->building_level++;
            long double shop_cost = scale_number(10.0L, ((res_count_t)this->building_level)-1.0L, 10.5L);
            this->_shop_cost = shop_cost;
        };

        if (this->building_level > MAX_BUILDING_LEVEL) {
            this->set_been_bought(true);
        };
    };

    this->schedule(custom_update_func, FPS_4, "set_enabled");
    custom_update_func(0);

    auto touch_ended_cb = [this](){
        res_count_t cost = this->get_cost();
        res_count_t total_coins = this->get_available_coins();

        if (cost <= total_coins)
        {
            this->add_available_coins(-cost);

            auto building = BUILDUP->get_target_building();
            building->building_level = this->building_level;

            Scene* scene = Director::getInstance()->getRunningScene();
            Node* harvest_scene = scene->getChildByName("HarvestScene");
            if (harvest_scene)
            {
                auto explosion_parts = ParticleSystemQuad::create("particles/upgrade.plist");
                explosion_parts->setPosition({570, 300});
                explosion_parts->setAutoRemoveOnFinish(true);

                harvest_scene->addChild(explosion_parts);

                harvest_scene->runAction(FShake::actionWithDuration(0.25f, 2.5f));

                do_vibrate(175);
            }
            else
            {
                LOG(WARNING) << "no proper scene, potential crash?";
            }
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

HarvesterShopNuItem* HarvesterShopNuItem::create(cocos2d::ui::Widget* parent, spBuilding building)
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

    this->set_title("Hire "+harvester_name);
}

bool HarvesterShopNuItem::init(Node* parent, spBuilding building)
{
    BuildingShopNuItem::init(parent, building);
    return true;
}

void HarvesterShopNuItem::my_init(WorkerSubType harv_type, IngredientSubType ing_type)
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

        return output;
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
        "set_0.png",
        "body_4.png",
        pick_one(gen_paths("headwear", 49), &gen),
        // pick_one(gen_paths("legs", 22), &gen),
        pick_one(gen_paths("shield", 49), &gen),
        pick_one(gen_paths("weapon", 49), &gen)
    };
    for (auto path : sprites)
    {
        base_node->addChild(Sprite::createWithSpriteFrameName(path));
    }

    base_node->setPosition(8,8);
    base_node->setScaleY(-1.0f);

    RenderTexture* rt = RenderTexture::create(16, 16);
    rt->retain();
    rt->begin();
    base_node->visit();
    rt->end();

    ui::Scale9Sprite* vr = dynamic_cast<ui::Scale9Sprite*>(this->item_icon->getVirtualRenderer());
    vr->setSpriteFrame(rt->getSprite()->getSpriteFrame());
};

void HarvesterShopNuItem::my_init_touch_ended_callback()
{
    this->set_touch_ended_callback([this]()
    {
        res_count_t cost = this->get_cost();
        res_count_t total_coins = this->get_available_coins();

        if (cost <= total_coins)
        {
            this->add_available_coins(-cost);
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
        this->_shop_cost = scale_number_slow(Harvester::get_base_shop_cost(this->harv_type), harvesters_owned, 1.15L, 6.0L);

        std::stringstream ss;
        auto harvested_count = Harvester::get_to_harvest_count(this->harv_type, this->ing_type);
        ss << "Adds " << harvested_count << " " << Ingredient::type_to_string(building->punched_sub_type) << "\nper sec";
        this->set_description(ss.str());
    };
    this->schedule(update_harvesters_cb, FPS_4, "harvester_count");
    update_harvesters_cb(0);

};

void SalesmanShopNuItem::my_init_touch_ended_callback()
{
    this->set_touch_ended_callback([this]()
    {
        res_count_t cost = this->get_cost();
        res_count_t total_coins = this->get_available_coins();

        if (cost <= total_coins)
        {
            this->add_available_coins(-cost);
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
    this->schedule(update_harvesters_cb, FPS_4, "harvester_count");
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

SalesmanShopNuItem* SalesmanShopNuItem::create(cocos2d::ui::Widget* parent, spBuilding building)
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

    this->set_title("Hire "+salesman_name);
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
        res_count_t total_coins = this->get_available_coins();

        if (cost <= total_coins)
        {
            this->add_available_coins(-cost);
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
    this->schedule(update_harvesters_cb, FPS_4, "harvester_count");
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

ConsumerShopNuItem* ConsumerShopNuItem::create(cocos2d::ui::Widget* parent, spBuilding building, IngredientSubType consumed_type)
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

    this->set_title("Hire "+consumer_name);
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


    auto gen = get_generator();
    std::vector<std::string> sprites = {
        "set_2.png",
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
