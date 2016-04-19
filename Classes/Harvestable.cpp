#include "Harvestable.h"
#include "FShake.h"

#include "Beatup.h"
#include "HouseBuilding.h"
#include "ShatterNode.h"
#include "MiscUI.h"
#include "GameLogic.h"
#include "Util.h"
#include "Recipe.h"
#include "attribute.h"

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
    this->click_limit = 15;
    this->current_clicks = 0;
}

bool Harvestable::init()
{
    this->setTouchEnabled(true); //true otherwise it cant be clicked
    this->setAnchorPoint(Vec2::ANCHOR_MIDDLE);

    this->building = BUILDUP->target_building;

    this->clip = cocos2d::ClippingNode::create();
    this->stencil = cocos2d::DrawNode::create();

    this->clip->setStencil(this->stencil);
    this->clip->setInverted(true);
    this->addChild(this->clip);

    //NOTE: this needs to add the sprite to this->clip
    this->init_sprite();

    this->init_clicks();

    this->setPosition(((GameLayer*)NULL)->get_center_pos()); //hack to call without access to an instance
    this->setName("harvestable");

    this->setScale(this->getScale()*4); //4*4 scale now


    return true;
};

bool Harvestable::should_shatter()
{
    return this->current_clicks >= this->click_limit;
}

void Harvestable::on_harvest()
{
    //this is on every click
    this->building->create_ingredients(Ingredient::string_to_type(this->building->punched_sub_type), 1);

    //this should be on shatter eventually, to line up with the floating label
    GameLogic::getInstance()->add_total_harvests(1);
    CCLOG("total of %i harvests now", GameLogic::getInstance()->get_total_harvests());
}

void Harvestable::onTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event)
{
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
    shake_action->setTag(1); //idk if there's a reason to pick a specific number over another
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

    auto shatter_action = ShatterAction::create(0.75f);
    shatter_sprite->runAction(Sequence::createWithTwoActions(shatter_action, remove));

};

void MiningHarvestable::init_sprite()
{

    float scale = 4;
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

    auto top_left_sprite = create_sprite("dirt_1.png", Vec2::ANCHOR_TOP_LEFT);
    auto top_right_sprite = create_sprite("dirt_2.png", Vec2::ANCHOR_TOP_RIGHT);
    auto bot_left_sprite = create_sprite("dirt_2.png", Vec2::ANCHOR_BOTTOM_LEFT);
    auto bot_right_sprite = create_sprite("dirt_1.png", Vec2::ANCHOR_BOTTOM_RIGHT);

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
        MIN(sprite_size.height - (click_ratio*sprite_size.height), sprite_size.height - size) //from the top, go a percentage of the way down
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

    this->click_limit = 1000000; //some really high number they'll never click

    this->enemy = std::make_shared<Fighter>(BUILDUP->city, "Brawler");
    this->enemy->team = Fighter::TeamTwo;
    this->enemy->sprite_name = "ogre10x10.png";
    this->enemy->attrs->health->set_vals(20);

    //this->sprite->setScale(this->sprite->getScale()*2);
    //this->sprite->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
    //this->setContentSize(this->get_sprite_size());

    FighterNode* fighter_node = FighterNode::create(this->enemy);
    fighter_node->setScale(0.25f);
    fighter_node->setPosition(Vec2(50, 0));
    fighter_node->xp_bar->setVisible(false); //dont need to see this for an enemy
    this->addChild(fighter_node);

    this->setScale(this->getScale()*1.5f);
    this->setPosition(((GameLayer*)NULL)->get_center_pos(-150, 0)); //hack to call without access to an instance

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
    this->enemy = std::make_shared<Fighter>(BUILDUP->city, "Challenger");
    this->enemy->team = Fighter::TeamTwo;
    this->enemy->sprite_name = "harvester.png";
    this->enemy->attrs->health->set_vals(30);

    FighterNode* fighter_node = dynamic_cast<FighterNode*>(this->getChildByName("fighter_node"));
    fighter_node->FighterNode::set_fighter(this->enemy);

    this->building->create_ingredients(Ingredient::Soul, 1);
}

void FightingHarvestable::on_harvest()
{
    //TODO name fighter to player_avatar or something
    auto player = BUILDUP->fighter;

    auto battle = std::make_shared<Battle>(BUILDUP);
    battle->combatants.push_back(player);
    auto fighter_node = dynamic_cast<FighterNode*>(this->getChildByName("fighter_node"));

    battle->combatants.push_back(this->enemy);

    battle->do_battle();
};

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
    spark_parts->setTotalParticles((int)(total_particles * click_ratio));
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
        intensity = 2.0f;
    };

    if (intensity != 0.0f) {
        this->runAction(FShake::actionWithDuration(0.075f, intensity));
    };
}
