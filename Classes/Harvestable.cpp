#include "Harvestable.h"
#include "FShake.h"

#include "Beatup.h"
#include "HouseBuilding.h"

USING_NS_CC;

bool Harvestable::init()
{
    this->clip = cocos2d::ClippingNode::create();
    this->stencil = cocos2d::DrawNode::create();

    this->clip->setStencil(this->stencil);
    this->clip->setInverted(true);

    this->sprite = cocos2d::Sprite::createWithSpriteFrameName("weapon_ice.png");
    this->sprite->setScale(4);
    this->clip->addChild(this->sprite);

    this->addChild(this->clip);
    this->setGlobalZOrder(1000000.0f);

    auto sprite_size = this->sprite->getContentSize();
    this->setContentSize(sprite_size*this->sprite->getScale());
    this->sprite->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);

    this->setTouchEnabled(true); //true otherwise it cant be clicked

    //NOTE need to addChild this to something still

    return true;
};

void Harvestable::onTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event)
{
    //TODO dont dig out points already dug out OR dig consecutively more from the top
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
};
