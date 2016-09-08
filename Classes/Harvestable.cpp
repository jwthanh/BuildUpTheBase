#include "Harvestable.h"
#include "FShake.h"

#include "HouseBuilding.h"
#include "ShatterNode.h"
#include "MiscUI.h"
#include "GameLogic.h"
#include "Util.h"
#include "Recipe.h"
#include "attribute.h"
#include "cocostudio/ActionTimeline/CSLoader.h"
#include <numeric>

#include "Fighter.h"
#include "attribute_container.h"
#include "ProgressBar.h"
#include "Technology.h"
#include "Modal.h"

USING_NS_CC;

Size Harvestable::get_sprite_size()
{
    return this->sprite->getContentSize() * this->sprite->getScale();
}

void Harvestable::init_sprite()
{
    this->sprite = cocos2d::Sprite::createWithSpriteFrameName(this->get_sprite_path());
    this->sprite->setScale(4);
    this->sprite->getTexture()->setAliasTexParameters();

    this->setContentSize(this->get_sprite_size());

    this->sprite->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
    this->sprite->setPosition(get_relative(this->getContentSize()));

    this->clip->addChild(this->sprite);
};

std::string Harvestable::get_sprite_path()
{
    return "weapon_ice.png";
};

void Harvestable::init_clicks()
{
    this->click_limit = 150;
    this->current_clicks = 0;
}

bool Harvestable::init()
{
    this->setTouchEnabled(true); //true otherwise it cant be clicked

    //allow touches to propagate passed it
    this->setSwallowTouches(false); //this seems to mean to let the events propagate past this, i dont see the harm in allowing it

    this->setAnchorPoint(Vec2::ANCHOR_MIDDLE);

    this->building = BUILDUP->get_target_building();

    this->clip = cocos2d::ClippingNode::create();
    this->stencil = cocos2d::DrawNode::create();

    this->clip->setStencil(this->stencil);
    this->clip->setInverted(true);
    this->addChild(this->clip);

    //NOTE: this needs to add the sprite to this->clip
    this->init_sprite();

    this->init_clicks();

    auto inst = CSLoader::getInstance();
    Node* harvest_scene_editor = inst->createNode("editor/scenes/base_scene.csb");
    Node* harvestable_pos = harvest_scene_editor->getChildByName("harvestable_pos");
    this->setPosition(harvestable_pos->getPosition()); 
    this->setName("harvestable");

    this->initial_scale = 4;
    this->setScale(this->initial_scale);


    return true;
};

bool Harvestable::should_shatter()
{
    return this->current_clicks >= this->click_limit;
}

void Harvestable::on_harvest()
{
    //this is on every click
    this->building->create_ingredients(this->building->punched_sub_type, this->get_per_touch_output());

    //this should be on shatter eventually, to line up with the floating label
    GameLogic::getInstance()->add_total_harvests(1);
}

res_count_t Harvestable::get_per_touch_output()
{
    res_count_t base = 1.0L;

    auto tech_map = this->building->techtree->get_tech_map();
    TechSubType tech_type = Technology::SubType::ClickDoublePower;
    res_count_t _def = 0L;
    res_count_t times_doubled = map_get(tech_map, tech_type, _def);
    if (times_doubled > 0){
        base *= std::pow(2.0L, times_doubled);
    };

    return base;
}

void Harvestable::spawn_label_on_touch(cocos2d::Touch* touch, float end_scale, float duration, std::string floating_msg)
{
    /*
     * if touch is null, it uses center of sprite
     */

    auto scale_to = ScaleTo::create(duration, end_scale);
    auto ease = EaseElasticOut::create(scale_to, duration);
    this->runAction(ease);

    Vec2 pos;
    auto floating_label = FloatingLabel::createWithTTF(floating_msg, DEFAULT_FONT, 30);
    if (touch)
    {
        pos = touch->getLocation();
    }
    else 
    {
        auto sprite_size = this->get_sprite_size();
        pos = this->getPosition() + Vec2(sprite_size);
    }

    pos.x += cocos2d::rand_minus1_1()*20.0f;
    pos.y += cocos2d::rand_minus1_1()*20.0f;

#ifdef __ANDROID__
    pos.y += 75.0f; //dont get hidden by finger
#endif

    floating_label->setPosition(pos);
    floating_label->setAnchorPoint(Vec2::ANCHOR_MIDDLE_BOTTOM);
    this->getParent()->addChild(floating_label);
    floating_label->do_float();
}

void Harvestable::animate_touch_start(cocos2d::Touch* touch)
{
    float end_scale = this->initial_scale*0.85f;
    float duration = 0.5f;

    res_count_t output = this->get_per_touch_output();

    std::stringstream ss;
    ss << "+" << beautify_double(output) << " " << Ingredient::type_to_string(this->building->punched_sub_type);

    std::string floating_msg = ss.str();
    if (this->building->can_fit_more_ingredients(this->building->punched_sub_type) == false)
    {
        std::vector<std::string> choices = {
            "Storage full",
            "Upgrade building",
            "Too many ingredients",
            "Sell some ingredients"
        };
        floating_msg = pick_one(choices);

        end_scale = this->initial_scale*0.95f;
        duration = 0.15f;

        Node* running_scene = Director::getInstance()->getRunningScene()->getChildByName("HarvestScene");
        Node* building_info_panel = running_scene->getChildByName("building_info_panel");
        Node* raw_ingredient_count = building_info_panel->getChildByName("ingredient_count");
        ui::Text* ingredient_count = dynamic_cast<ui::Text*>(raw_ingredient_count);
        animate_flash_action(ingredient_count, 0.2f, 1.15f);
    };

    this->spawn_label_on_touch(touch, end_scale, duration, floating_msg);
}

bool Harvestable::onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event)
{
    bool base_val = Widget::onTouchBegan(touch, event);

    if (this->_hitted)
    {
        this->animate_touch_start(touch);
    }

    return base_val;
};

void Harvestable::animate_touch_end(cocos2d::Touch* touch)
{
    float end_scale = this->initial_scale;
    float duration = 0.5f;
    auto scale_to = ScaleTo::create(duration, end_scale);
    auto ease = EaseElasticOut::create(scale_to, duration);
    this->runAction(ease);
}

void Harvestable::onTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event)
{
    //Widget::onTouchEnded(touch, event); //this shouldnt be called because releaseUpEvent hasnt been set I guess. 


    animate_touch_end(touch);

    //FIXME remove hardcoded check for arena, otherwise the sword wont rotate
    if (this->building->can_fit_more_ingredients(this->building->punched_sub_type) == false && this->building->name != "The Arena") 
    {
        return;
    };

    this->current_clicks += 1;

    this->animate_harvest();
    this->on_harvest();

    if (this->should_shatter()) {
        this->shatter();
    };

};

void Harvestable::animate_clip()
{
    auto size = 20.0f;
    Vec2 origin = Vec2(
        this->sprite->getContentSize().width*this->sprite->getScaleX()*CCRANDOM_0_1(),
        this->sprite->getContentSize().height*this->sprite->getScaleY()*CCRANDOM_0_1()
    );

    Vec2 destination = origin +Vec2(size, size);
    this->stencil->drawSolidRect(origin, destination, Color4F::MAGENTA);
}

float Harvestable::get_click_ratio() const
{
    return static_cast<float>(this->current_clicks) / this->click_limit;
}

void Harvestable::animate_harvest()
{
    this->animate_clip();

    auto shake_action = FShake::actionWithDuration(0.075f, 2.5f);
    this->runAction(shake_action);

    this->animate_rotate();
}

void Harvestable::animate_rotate()
{

    float click_ratio = this->get_click_ratio();
    float rotation = 0.0f;

    if (click_ratio >= 0.8f) {
        rotation = 15.0f;
    } else if (click_ratio >= 0.6f) {
        rotation = 6.0f;
    } else if (click_ratio >= 0.4f) {
        rotation = 2.0f;
    };

    if (rotation != 0.0f) {
        auto rotate_by = RotateBy::create(0.05f, rotation);
        auto rotate_to = RotateTo::create(0.05f, 0);
        this->sprite->runAction(Sequence::createWithTwoActions(rotate_by, rotate_to));
        this->clip->runAction(Sequence::createWithTwoActions(rotate_by, rotate_to));
    };
}

std::string Harvestable::get_shatter_text()
{
    return "+1 Harvest";
};

void Harvestable::shatter()
{
    this->sprite->setVisible(false);
    this->setTouchEnabled(false);

    float sprite_scale = this->sprite->getScale();
    Size sprite_size = this->get_sprite_size();

    auto shatter_sprite = ShatterSprite::createWithSpriteFrame(this->sprite->getSpriteFrame());
    shatter_sprite->setScale(sprite_scale);
    shatter_sprite->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
    shatter_sprite->setPosition(get_relative(this->getContentSize()));
    this->addChild(shatter_sprite);

    shatter_sprite->setOpacity(0); //hide this so it shatters it doesnt leave anything behind

    //spawn label
    //TODO fix hardcoded name
    this->spawn_label_on_touch(NULL, 1, 1, this->get_shatter_text());

    CallFunc* remove = CallFunc::create([this](){ this->removeFromParent(); });

    auto shatter_action = ShatterAction::create(0.5f);
    //shatter_sprite->runAction(shatter_action);
    shatter_sprite->runAction(Sequence::createWithTwoActions(shatter_action, remove));

};

bool FarmingHarvestable::init()
{
    bool val = Harvestable::init();

    this->initial_scale = 0.8f;
    this->setScale(this->initial_scale);

    return val;
};

std::string FarmingHarvestable::get_sprite_path()
{
    return "farm.png";
};

bool GraveyardHarvestable::init()
{
    bool val = Harvestable::init();

    this->initial_scale = 2.5f;
    this->setScale(this->initial_scale);

    return val;
};

std::string GraveyardHarvestable::get_sprite_path()
{
    return "grave.png";
};

bool TreeHarvestable::init()
{
    bool val = Harvestable::init();

    this->initial_scale = 1.0f;
    this->setScale(this->initial_scale);

    return val;
}

std::string TreeHarvestable::get_sprite_path()
{
    return "tree.png";
}

bool DumpsterHarvestable::init()
{
    bool val = Harvestable::init();

    this->initial_scale = 0.70f;
    this->setScale(this->initial_scale);

    return val;
}

std::string DumpsterHarvestable::get_sprite_path()
{
    return "dump_darker.png";
}

void MiningHarvestable::init_sprite()
{

    float scale = 2.75f;
    auto create_sprite = [this,scale](std::string sprite_path, Vec2 relative_pos)
    {
        auto sprite = Sprite::createWithSpriteFrameName(sprite_path);
        sprite->setScale(scale);
        sprite->setAnchorPoint(relative_pos);

        auto pos = get_relative(this->getContentSize(), relative_pos);
        sprite->setPosition(pos);

        return sprite;

    };

    auto sprite_size = Size(15, 16);
    sprite_size.width *= scale * 2;
    sprite_size.height *= scale * 2;
    this->setContentSize(sprite_size);

    std::vector<std::string> choices = {"dirt_1.png", "dirt_2.png"};
    auto top_left_sprite = create_sprite(pick_one(choices), Vec2::ANCHOR_TOP_LEFT);
    auto top_right_sprite = create_sprite(pick_one(choices), Vec2::ANCHOR_TOP_RIGHT);
    auto bot_left_sprite = create_sprite(pick_one(choices), Vec2::ANCHOR_BOTTOM_LEFT);
    auto bot_right_sprite = create_sprite(pick_one(choices), Vec2::ANCHOR_BOTTOM_RIGHT);

    auto rt = RenderTexture::create((int)this->getContentSize().width, (int)this->getContentSize().height);
    rt->retain();
    rt->begin();
    top_left_sprite->visit();
    top_right_sprite->visit();
    bot_left_sprite->visit();
    bot_right_sprite->visit();
    rt->end();

    this->sprite = Sprite::createWithTexture(rt->getSprite()->getTexture());

    this->sprite->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
    this->sprite->setPosition(get_relative(this->getContentSize()));
    this->clip->addChild(this->sprite);
    
};


void MiningHarvestable::animate_clip()
{
    float click_ratio = this->get_click_ratio();

    auto sprite_size = this->get_sprite_size();
    auto size = 20.0f;
    Vec2 origin = Vec2(
        MIN(sprite_size.width, sprite_size.width - size)*CCRANDOM_0_1(), //random along the width, dont go so far right
        sprite_size.height - (click_ratio*sprite_size.height) //from the top, go a percentage of the way down
        );

    Vec2 destination = origin + Vec2(size, size);
    this->stencil->drawSolidRect(origin, destination, Color4F::MAGENTA);
}

void MiningHarvestable::animate_rotate()
{
    float intensity = 0.0f;
    float click_ratio = this->get_click_ratio();

    if (click_ratio >= 0.8f) {
        intensity = 15.0f;
    } else if (click_ratio >= 0.6f) {
        intensity = 6.0f;
    } else if (click_ratio >= 0.4f) {
        intensity = 2.0f;
    };

    if (intensity != 0.0f) {
        this->runAction(FShake::actionWithDuration(0.075f, intensity));
    };
}
CraftingHarvestable::CraftingHarvestable(spRecipe recipe)
    : recipe(recipe)
{
}

CraftingHarvestable* CraftingHarvestable::create(spRecipe recipe)
{
    CraftingHarvestable *pRet = new(std::nothrow) CraftingHarvestable(recipe);
    if (pRet && pRet->init()) {
        pRet->autorelease(); return pRet;
    }
    else {
        delete pRet;
        pRet = nullptr;
        return nullptr;
    }
}

std::string CraftingHarvestable::get_sprite_path()
{
    return "anvil.png";
};

float CraftingHarvestable::get_click_ratio() const
{
    if (this->recipe == NULL)
    {
        return Harvestable::get_click_ratio();
    }
    else
    {
        //TODO fix harcoded 3
        return static_cast<float>(this->recipe->current_clicks) / 3;
    };
}

bool CraftingHarvestable::can_satisfy_recipe_per_click()
{
    if (this->recipe == NULL) { return false; }

    auto all_ingredients = BUILDUP->get_all_ingredients();

    //go through each component, for each ing, make sure we've got at least one
    // otherwise short circuit and fail fast
    for (auto component : this->recipe->components)
    {
        IngredientSubType ing_type = component.first;

        //either the amount required minus click so far, or 0 so its not negative
        res_count_t amount_required = std::max(0.0L, component.second - this->recipe->current_clicks);

        //if there's at least one required, check for at least one
        res_count_t _def = 0.0;
        if (amount_required >= 1.0 && map_get(all_ingredients, ing_type, _def) < 1.0)
        {
            return false;
        }

    };

    return true;
};

void CraftingHarvestable::on_harvest()
{
    if (!this->recipe) { return; }

    auto all_ingredients = BUILDUP->get_all_ingredients();

    bool can_satisfy = this->can_satisfy_recipe_per_click();

    //if all parts are satisfied, then actually use them and increment counter
    if (can_satisfy == true)
    {
        for (auto component : this->recipe->components)
        {
            IngredientSubType ing_type = component.first;
            res_count_t amount_required = std::max(0.0L, component.second - this->recipe->current_clicks);

            if (amount_required > 0)
            {
                BUILDUP->remove_shared_ingredients_from_all(ing_type, 1);
                CCLOG("spending a %s", Ingredient::type_to_string(ing_type).c_str());
            }
            else
            {
                CCLOG("dont need to spend %s", Ingredient::type_to_string(ing_type).c_str());
            }
        };
        this->recipe->current_clicks += 1;

    }
};

void CraftingHarvestable::animate_touch_start(cocos2d::Touch* touch)
{
    float end_scale = this->initial_scale*0.85f;
    float duration = 0.5f;

    std::stringstream ss;
    if (this->recipe == NULL)
    {
        ss << "No recipe yet";
        std::string floating_msg = ss.str();
        this->spawn_label_on_touch(touch, end_scale, duration, floating_msg);
    }
    else 
    {
        if (this->can_satisfy_recipe_per_click() && !this->should_shatter())
        {
            //TODO doesn't make sure it needs at least one of any given ing type
            for (auto component : this->recipe->components)
            {
                ss << "used a " << Ingredient::type_to_string(component.first);
                std::string floating_msg = ss.str();
                this->spawn_label_on_touch(touch, end_scale, duration, floating_msg);
            }
        }
        else
        {
            //TODO make sure you're actually missing each ing type
            for (auto component : this->recipe->components)
            {
                ss << "missing a " << Ingredient::type_to_string(component.first);
                std::string floating_msg = ss.str();
                this->spawn_label_on_touch(touch, end_scale, duration, floating_msg);
            }
        }
    }


    //TODO handle message for being too full on recipe, like in
    //Harvestable::animate_touch_start

};

void CraftingHarvestable::init_clicks()
{
    if (!this->recipe) {
        CCLOG("trying to init_clicks without a recipe");
        Harvestable::init_clicks();
        return;
    };

    this->current_clicks = 0;
    this->click_limit = this->recipe->clicks_required;
};

//same as Harvestable::onTouchEnded except it doesn't check for room
void CraftingHarvestable::onTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event)
{

    animate_touch_end(touch);

    this->current_clicks += 1;

    this->animate_harvest();
    this->on_harvest();

    //recipes completed
    if (this->should_shatter()) {
        this->shatter();
    };

};

void CraftingHarvestable::animate_clip()
{
    if (this->can_satisfy_recipe_per_click())
    {
        auto sprite_size = this->get_sprite_size();
        auto size = 20.0f;
        Vec2 origin = Vec2(
                MIN(sprite_size.width, sprite_size.width - size)*CCRANDOM_0_1(), //random along the width, dont go so far right
                sprite_size.height
                );

        float click_ratio = this->get_click_ratio();
        auto spark_parts = ParticleSystemQuad::create("particles/anvil_spark.plist");
        spark_parts->setScale(1.0 / 4.0f);
        spark_parts->setPosition(origin);
        int total_particles = spark_parts->getTotalParticles();
        int active_particles = (int)(total_particles * click_ratio);
        //add 10 so there's always visible sparks, cheaper than mix/max calls
        spark_parts->setTotalParticles(active_particles+10);
        this->addChild(spark_parts);
    };
}

bool CraftingHarvestable::should_shatter()
{
    bool has_recipe = this->recipe != NULL;
    //dont ever shatter if no recipe
    if (has_recipe == false)
    {
        return false;
    }

    //use clicks instead of ingredient count, because it needs partial progress
    res_count_t clicks_required = 3.0; //TODO use biggest number of component in recipe
    bool is_satisfied = this->recipe->current_clicks >= clicks_required;

    return is_satisfied;
};

std::string CraftingHarvestable::get_shatter_text()
{
    return "Crafted!";
};

void CraftingHarvestable::shatter()
{
    if (this->recipe != NULL) {
        this->recipe->callback();
        this->recipe->current_clicks = 0;
    };

    Harvestable::shatter();
}

bool FightingHarvestable::init()
{
    Harvestable::init();

    this->initial_scale = 4;
    this->setScale(this->initial_scale);

    this->click_limit = 1000000; //some really high number they'll never click

    //setup enemy node
    this->enemy = std::make_shared<Fighter>("temp");
    FighterNode* fighter_node = FighterNode::create(this->enemy);
    fighter_node->setScale(0.25f);
    fighter_node->setPosition(Vec2(50, 0));
    fighter_node->xp_bar->setVisible(false); //dont need to see this for an enemy
    this->addChild(fighter_node);

    this->spawn_enemy();
    
    fighter_node->sprite->addTouchEventListener([this](Ref* target, ui::Widget::TouchEventType type)
    {
        if (type == TouchEventType::BEGAN)
        {
            //// this nearly worked, the coords werent right, so this->_hitted
            ////was always false, otherwise worked to sim a click
            // EventTouch evt = cocos2d::EventTouch();
            // evt.setEventCode(EventTouch::EventCode::BEGAN);
            // Touch touch = Touch();
            // Vec2 vec = { 1, 1 };
            // vec = this->sprite->convertToWorldSpace(vec);

            // touch.setTouchInfo(-1, vec.x, vec.y);
            // std::vector<Touch*> touches ={  &touch  };
            // evt.setTouches(touches);
            // this->sprite->getEventDispatcher()->dispatchEvent(&evt);


            //copy and pasted from Harvester::onTouchEnded
            this->current_clicks += 1;

            this->animate_harvest();
            this->on_harvest();

            if (this->should_shatter()) {
                this->shatter();
            };

        }
    });

    //adjust position because the enemy and sword are both offset from the center
    this->setPosition(Vec2(this->getPosition().x-110, this->getPosition().y)); 


    this->is_critical_hit = false;

    return true;
};

std::string FightingHarvestable::get_sprite_path()
{
    return "sword.png";
}

void FightingHarvestable::animate_clip()
{
    auto player = BUILDUP->fighter;
    double damage = player->attrs->damage->current_val;
    res_count_t output = damage;

    std::stringstream ss;
    ss << beautify_double(output);

    std::string floating_msg = ss.str();
    auto floating_label = FloatingLabel::createWithTTF(floating_msg, DEFAULT_FONT, 30);

    if (this->is_critical_hit)
    {
        floating_label->setTextColor(Color4B::ORANGE);
    }

    auto fighter_node = dynamic_cast<FighterNode*>(this->getChildByName("fighter_node"));
    auto pos = fighter_node->convertToWorldSpace(this->getTouchEndPosition());
    pos.x += cocos2d::rand_minus1_1()*20.0f;
    pos.y += cocos2d::rand_minus1_1()*20.0f;

#ifdef __ANDROID__
    pos.y += 75.0f; //dont get hidden by finger
#endif

    floating_label->setPosition(pos);
    floating_label->setAnchorPoint(Vec2::ANCHOR_MIDDLE_BOTTOM);
    this->getParent()->addChild(floating_label);
    floating_label->do_float();

};

void FightingHarvestable::animate_harvest()
{
    if (BUILDUP->fighter->attrs->health->is_empty() == false)
    {
        Harvestable::animate_harvest();
    }
    else
    {
        //shake hp panel if dead
        auto harvest_scene = this->getParent();
        auto player_info_panel = harvest_scene->getChildByName("player_info_panel");
        player_info_panel->runAction(FShake::actionWithDuration(0.075f, 10));
    }
}

void FightingHarvestable::animate_rotate()
{
    float rotation = 30.0f;
    auto rotate_by = RotateBy::create(0.05f, rotation);
    auto rotate_to = RotateTo::create(0.05f, 0);
    this->sprite->runAction(Sequence::createWithTwoActions(rotate_by, rotate_to));
    this->clip->runAction(Sequence::createWithTwoActions(rotate_by, rotate_to));
};

bool FightingHarvestable::should_shatter()
{
    if (this->enemy == NULL) { return false; }
    return this->enemy->attrs->health->is_empty();
}

void FightingHarvestable::spawn_enemy()
{
    auto game_logic = GameLogic::getInstance();

    this->enemy = std::make_shared<Fighter>("Challenger");
    this->enemy->team = Fighter::TeamTwo;
    this->enemy->sprite_name = "harvester.png";

    res_count_t base_hp = 20.0;
    res_count_t base_slow_rate = 3.0;

    auto workshop = BUILDUP->city->building_by_name("The Workshop");
    auto tech_map = workshop->techtree->get_tech_map();

    auto tech_type = Technology::SubType::CombatWeakenEnemy;
    res_count_t _def = 0L;
    res_count_t decrease_count = map_get(tech_map, tech_type, _def);
    res_count_t final_slow_rate = std::max(0.01L, base_slow_rate + (0.005*decrease_count));

    res_count_t enemy_hp = scale_number_slow(base_hp, (res_count_t)game_logic->get_total_kills(), 1.05L, final_slow_rate);
    enemy_hp = std::max(base_hp, enemy_hp); //make sure the enemy has at least 20 hp

    double enemy_dmg = 3;
    this->enemy->attrs->health->set_vals((int)enemy_hp);
    this->enemy->attrs->damage->set_vals((int)enemy_dmg);

    FighterNode* fighter_node = dynamic_cast<FighterNode*>(this->getChildByName("fighter_node"));
    fighter_node->set_fighter(this->enemy);

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

    //use the texture from RenderTexture and replace the ImageView's Scale9
    //texture
    auto img_view = fighter_node->sprite;
    ui::Scale9Sprite* scale9_sprite = dynamic_cast<ui::Scale9Sprite*>(img_view->getVirtualRenderer());
    Sprite* tex_sprite = dynamic_cast<Sprite*>(rt->getSprite());
    scale9_sprite->setSpriteFrame(tex_sprite->getSpriteFrame());

};

const std::map<res_count_t, std::string> kill_messages = {
    { 1.0, "The first kill!" },
    { 2.0, "The second kill! This is a weird feeling." },
    { 3.0, "The third kill! This isn't right." },
    { 4.0, "The fourth kill! It doesn't matter though." },
    { 5.0, "The fifth kill! I'll ignore this feeling." },
    { 10.0, "The tenth kill! But I can't, it won't go away." },
    { 15.0, "The fifteenth kill! It gnaws on me..." },
    { 25.0, "The twenty-fifth kill! I need more." },
    { 35.0, "The thirtieth kill! I need more strength." },
    { 50.0, "The fiftieth kill! I need more power." },
    { 75.0, "The seventieth kill! I need more souls." },
    { 100.0, "The hundredth kill! I've done it. I've killed one hundred souls. I don't know what this means" }
};

void FightingHarvestable::shatter()
{
    auto game_logic = GameLogic::getInstance();
    game_logic->add_total_kills(1);

    auto popup_panel = GameLogic::get_popup_panel();
    //auto it = std::find(kill_messages.begin(), kill_messages.end(), game_logic->get_total_kills());
    auto it = kill_messages.find(game_logic->get_total_kills());
    std::string kill_message = "Another kill, another day";
    if (it != kill_messages.end())
    {
        kill_message = (*it).second;
    }
    popup_panel->set_string(kill_message);
    popup_panel->animate_open();

    this->spawn_enemy();

    this->building->create_ingredients(Ingredient::SubType::Soul, 1);
}

void FightingHarvestable::on_harvest()
{
    //TODO name fighter to player_avatar or something
    auto player = BUILDUP->fighter;
    res_count_t damage = this->get_per_touch_output();

    std::mt19937 gen = std::mt19937(std::random_device{}());
    std::uniform_int_distribution<int> distribution(0, 100);
    int crit_result = distribution(gen);

    int crit_chance = this->get_combat_crit_chance(); 

    if (crit_result <= crit_chance)
    {
        this->is_critical_hit = true;
        res_count_t crit_factor = this->get_combat_crit_factor();
        damage *= crit_factor;
        do_vibrate(16);
    }
    else
    {
        this->is_critical_hit = false;
    }


    player->attrs->damage->set_vals(damage);

    auto battle = std::make_shared<Battle>();
    battle->combatants.push_back(player);

    battle->combatants.push_back(this->enemy);

    battle->do_battle();
};

void FightingHarvestable::animate_touch_start(cocos2d::Touch* touch)
{
    //do nothing
};

void FightingHarvestable::animate_touch_end(cocos2d::Touch* touch)
{
    //do nothing
};

res_count_t FightingHarvestable::get_per_touch_output()
{
    res_count_t base = 5.0L;

    auto tech_map = this->building->techtree->get_tech_map();

    auto tech_type = Technology::SubType::CombatDamage;
    res_count_t _def = 0L;
    res_count_t times_increased = map_get(tech_map, tech_type, _def);

    base = scale_number(base, times_increased, 1.2L);

    return std::floor(base + (base*0.15*CCRANDOM_MINUS1_1()));
}

int FightingHarvestable::get_combat_crit_chance()
{
    int base = 1;

    auto tech_map = this->building->techtree->get_tech_map();

    auto tech_type = Technology::SubType::CombatCritChance;
    res_count_t _def = 0L;
    res_count_t times_increased = map_get(tech_map, tech_type, _def);

    if (times_increased > 0){
        base += 1.0L * times_increased;
    };

    return std::floor(base);
}

res_count_t FightingHarvestable::get_combat_crit_factor()
{
    int base = 5;

    auto tech_map = this->building->techtree->get_tech_map();

    auto tech_type = Technology::SubType::CombatCritFactor;
    res_count_t _def = 0L;
    res_count_t times_increased = map_get(tech_map, tech_type, _def);

    if (times_increased > 0){
        base += 1.0L * times_increased;
    };

    return std::floor(base);
}

std::string UndeadHarvestable::get_sprite_path()
{
    return "necro_open.png";
}

void UndeadHarvestable::init_sprite()
{
    Harvestable::init_sprite();

    auto cache = SpriteFrameCache::getInstance();
    Vector<SpriteFrame*> frames = Vector<SpriteFrame*>();


    frames.pushBack(cache->getSpriteFrameByName("necro_open.png"));
    frames.pushBack(cache->getSpriteFrameByName("necro_open.png"));
    frames.pushBack(cache->getSpriteFrameByName("necro_open.png"));
    frames.pushBack(cache->getSpriteFrameByName("necro_open.png"));
    frames.pushBack(cache->getSpriteFrameByName("necro_open.png"));
    frames.pushBack(cache->getSpriteFrameByName("necro_open.png"));
    frames.pushBack(cache->getSpriteFrameByName("necro_closed.png"));
    frames.pushBack(cache->getSpriteFrameByName("necro_closed.png"));
    frames.pushBack(cache->getSpriteFrameByName("necro_closed.png"));
    frames.pushBack(cache->getSpriteFrameByName("necro_closed.png"));
    frames.pushBack(cache->getSpriteFrameByName("necro_open.png"));
    frames.pushBack(cache->getSpriteFrameByName("necro_open.png"));
    frames.pushBack(cache->getSpriteFrameByName("necro_open.png"));
    frames.pushBack(cache->getSpriteFrameByName("necro_closed.png"));
    frames.pushBack(cache->getSpriteFrameByName("necro_closed.png"));
    cocos2d::Animation* anim = cocos2d::Animation::createWithSpriteFrames(frames, 0.1f, 1);

    cocos2d::Animate* anim_action = cocos2d::Animate::create(anim);
    this->sprite->runAction(RepeatForever::create(anim_action));
};

void UndeadHarvestable::animate_clip()
{
    auto sprite_size = this->get_sprite_size();
    auto size = 20.0f;
    Vec2 origin = Vec2(
        sprite_size.width*CCRANDOM_0_1(),
        sprite_size.height*CCRANDOM_0_1()
    );

    float click_ratio = this->get_click_ratio();
    auto spark_parts = ParticleSystemQuad::create("particles/blood.plist");
    spark_parts->setScale(1.0 / 4.0f);
     spark_parts->setPosition(origin);
    int total_particles = spark_parts->getTotalParticles();
    int particle_count = (int)(total_particles * click_ratio);
    spark_parts->setTotalParticles(particle_count+10);
    this->addChild(spark_parts);

};

void UndeadHarvestable::animate_rotate()
{
    float intensity = 0.0f;
    float click_ratio = this->get_click_ratio();

    if (click_ratio >= 0.8f) {
        intensity = 15.0f;
    } else if (click_ratio >= 0.6f) {
        intensity = 6.0f;
    } else if (click_ratio >= 0.4f) {
        intensity = 4.0f;
    };

    if (intensity != 0.0f) {
        this->runAction(FShake::actionWithDuration(0.075f, intensity));
    };
}
