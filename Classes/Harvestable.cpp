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
    res_count_t times_doubled = map_get(tech_map, Technology::SubType::ClickDoublePower, 0L);
    if (times_doubled > 0){
        base *= 2.0L * times_doubled;
    };

    return base;
}

void Harvestable::animate_touch_start(cocos2d::Touch* touch)
{
    float end_scale = this->initial_scale*0.85f;
    float duration = 0.5f;

    res_count_t output = get_per_touch_output();

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

    auto scale_to = ScaleTo::create(duration, end_scale);
    auto ease = EaseElasticOut::create(scale_to, duration);
    this->runAction(ease);

    auto floating_label = FloatingLabel::createWithTTF(floating_msg, DEFAULT_FONT, 30);
    auto pos = touch->getLocation();
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

    if (this->building->can_fit_more_ingredients(this->building->punched_sub_type) == false) 
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

void Harvestable::shatter()
{
    this->sprite->setVisible(false);
    this->setTouchEnabled(false);

    // auto rt = RenderTexture::create(
    //     (int)(this->get_sprite_size().width*this->getScaleX()) * 2,
    //     (int)(this->get_sprite_size().height*this->getScaleY()) * 2,
    //     Texture2D::PixelFormat::RGBA8888,
    //     CC_GL_DEPTH24_STENCIL8
    //     );
    // rt->retain();

    // rt->begin();
    // this->Node::visit();
    // rt->end();

    // rt->saveToFile("out4.png");

    float sprite_scale = this->sprite->getScale();
    Size sprite_size = this->get_sprite_size();

    auto shatter_sprite = ShatterSprite::createWithSpriteFrame(this->sprite->getSpriteFrame());
    shatter_sprite->setScale(sprite_scale);
    shatter_sprite->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
    shatter_sprite->setPosition(get_relative(this->getContentSize()));
    this->addChild(shatter_sprite);

    shatter_sprite->setOpacity(0); //hide this so it shatters it doesnt leave anything behind

    //spawn label //TODO fix hardcoded name
    auto floating_label = FloatingLabel::createWithTTF("+1 Harvest", DEFAULT_FONT, 24);
    floating_label->setPosition(
        this->getPosition() + Vec2(sprite_size)
        );
    floating_label->setAnchorPoint(Vec2::ANCHOR_BOTTOM_RIGHT);
    this->getParent()->addChild(floating_label);
    floating_label->do_float();

    CallFunc* remove = CallFunc::create([this](){ this->removeFromParent(); });

    auto shatter_action = ShatterAction::create(0.5f);
    //shatter_sprite->runAction(shatter_action);
    shatter_sprite->runAction(Sequence::createWithTwoActions(shatter_action, remove));

};

bool FarmingHarvestable::init()
{
    bool val = Harvestable::init();

    this->initial_scale = 2.5f;
    this->setScale(this->initial_scale);

    return val;
};

std::string FarmingHarvestable::get_sprite_path()
{
    return "bush.png";
};

bool GraveyardHarvestable::init()
{
    bool val = Harvestable::init();

    this->initial_scale = 7.0f;
    this->setScale(this->initial_scale);

    return val;
};

std::string GraveyardHarvestable::get_sprite_path()
{
    return "bones.png";
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

void CraftingHarvestable::animate_clip()
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
    spark_parts->setTotalParticles((int)(total_particles * click_ratio));
    this->addChild(spark_parts);
}

void CraftingHarvestable::shatter()
{
    if (this->recipe != NULL) {
        CCLOG("clicked enough, time to output");
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
    this->enemy = std::make_shared<Fighter>("Brawler");
    this->enemy->team = Fighter::TeamTwo;
    this->enemy->sprite_name = "ogre10x10.png";
    this->enemy->attrs->health->set_vals(20);

    FighterNode* fighter_node = FighterNode::create(this->enemy);
    fighter_node->setScale(0.25f);
    fighter_node->setPosition(Vec2(50, 0));
    fighter_node->xp_bar->setVisible(false); //dont need to see this for an enemy
    this->addChild(fighter_node);

    //adjust position because the enemy and sword are both offset from the center
    this->setPosition(Vec2(this->getPosition().x-110, this->getPosition().y)); 

    return true;
};

std::string FightingHarvestable::get_sprite_path()
{
    return "sword.png";
}

void FightingHarvestable::animate_clip()
{
    //Do nothing deliberately
};

void FightingHarvestable::animate_harvest()
{
    if (BUILDUP->fighter->attrs->health->is_empty() == false)
    {
        Harvestable::animate_harvest();
    }
    else
    {
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

void FightingHarvestable::shatter()
{
    this->enemy = std::make_shared<Fighter>("Challenger");
    this->enemy->team = Fighter::TeamTwo;
    this->enemy->sprite_name = "harvester.png";
    this->enemy->attrs->health->set_vals(30);

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

    this->building->create_ingredients(Ingredient::SubType::Soul, 1);
}

void FightingHarvestable::on_harvest()
{
    //TODO name fighter to player_avatar or something
    auto player = BUILDUP->fighter;
    auto damage = this->get_per_touch_output();
    player->attrs->damage->set_vals(damage);

    float end_scale = this->initial_scale*0.85f;
    float duration = 0.5f;

    res_count_t output = damage;

    std::stringstream ss;
    ss << beautify_double(output);

    std::string floating_msg = ss.str();
    auto scale_to = ScaleTo::create(duration, end_scale);
    auto ease = EaseElasticOut::create(scale_to, duration);
    this->runAction(ease);

    auto floating_label = FloatingLabel::createWithTTF(floating_msg, DEFAULT_FONT, 30);

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
    res_count_t base = 10.0L;

    auto tech_map = this->building->techtree->get_tech_map();
    res_count_t times_doubled = map_get(tech_map, Technology::SubType::CombatDamage, 0L);
    if (times_doubled > 0){
        base += 2.0L * times_doubled;
    };

    return std::floor(base + (base*0.15*CCRANDOM_MINUS1_1()));
}

std::string UndeadHarvestable::get_sprite_path()
{
    return "necro_open.png";
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
