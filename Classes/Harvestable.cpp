#include "Harvestable.h"
#include "FShake.h"

#include "Beatup.h"
#include "HouseBuilding.h"
#include "ShatterNode.h"
#include "MiscUI.h"
#include "GameLogic.h"
#include "Util.h"

USING_NS_CC;

bool Harvestable::init()
{
    this->setTouchEnabled(true); //true otherwise it cant be clicked
    this->setAnchorPoint(Vec2::ANCHOR_MIDDLE);

    this->clip = cocos2d::ClippingNode::create();
    this->stencil = cocos2d::DrawNode::create();

    this->clip->setStencil(this->stencil);
    this->clip->setInverted(true);

    this->sprite = cocos2d::Sprite::createWithSpriteFrameName("weapon_ice.png");
    this->sprite->setScale(4);
    this->clip->addChild(this->sprite);

    this->addChild(this->clip);

    auto sprite_size = this->sprite->getContentSize();
    this->setContentSize(sprite_size*this->sprite->getScale());

    this->sprite->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
    this->sprite->setPosition(get_relative(this->getContentSize()));

    this->click_limit = 15;
    this->current_clicks = 0;

    return true;
};

void Harvestable::onTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event)
{
    //TODO dont dig out points already dug out OR dig consecutively more from the top
    this->current_clicks += 1;
    if (this->current_clicks < this->click_limit) {
        auto size = 20.0f;
        Vec2 origin = Vec2(
                this->sprite->getContentSize().width*this->sprite->getScaleX()*CCRANDOM_0_1(),
                this->sprite->getContentSize().height*this->sprite->getScaleY()*CCRANDOM_0_1()
                );

        Vec2 destination = origin +Vec2(size, size);
        this->stencil->drawSolidRect(origin, destination, Color4F::MAGENTA);

        this->runAction(FShake::actionWithDuration(0.075f, 2.5f));

        auto farm = this->beatup->buildup->city->building_by_name("The Farm");
        farm->create_resources(Resource::Ingredient, 1, "Berry");

        float click_ratio = static_cast<float>(this->current_clicks) / this->click_limit;
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

    } else {
        this->shatter();

        GameLogic::getInstance()->add_total_harvests(1);
        CCLOG("total of %i harvests now", GameLogic::getInstance()->get_total_harvests());
    };
};

void Harvestable::shatter()
{
    this->sprite->setVisible(false);
    this->setTouchEnabled(false);

    float sprite_scale = this->sprite->getScale();
    Size sprite_size = this->sprite->getContentSize();

    auto shatter_sprite = ShatterSprite::createWithSpriteFrame(this->sprite->getSpriteFrame());
    shatter_sprite->setScale(sprite_scale);
    shatter_sprite->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
    shatter_sprite->setPosition(get_relative(this->getContentSize()));
    this->addChild(shatter_sprite);

    shatter_sprite->setOpacity(0); //hide this so it shatters it doesnt leave anything behind

    //spawn label //TODO fix hardcoded name
    auto floating_label = FloatingLabel::createWithTTF("+1 Harvest", DEFAULT_FONT, 12);
    floating_label->setPosition(
            this->getPosition() + Vec2(sprite_size*sprite_scale)
        );
    floating_label->setAnchorPoint(Vec2::ANCHOR_BOTTOM_RIGHT);
    this->getParent()->addChild(floating_label);
    floating_label->do_float();

    CallFunc* remove = CallFunc::create([this](){ this->removeFromParent(); });

    auto shatter_action = ShatterAction::create(0.75f);
    shatter_sprite->runAction(Sequence::createWithTwoActions(shatter_action, remove));

};
