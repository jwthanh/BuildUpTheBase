#include "Harvestable.h"

USING_NS_CC;

bool Harvestable::init()
{
    this->clip = cocos2d::ClippingNode::create();

    this->stencil = cocos2d::DrawNode::create();
    this->stencil->drawSolidRect(cocos2d::Vec2::ZERO, cocos2d::Vec2(10, 10), Color4F::MAGENTA);

    this->clip->setStencil(this->stencil);
    this->clip->setInverted(true);


    this->sprite = cocos2d::Sprite::createWithSpriteFrameName("weapon_ice.png");
    this->sprite->setScale(4);
    this->clip->addChild(this->sprite);

    this->addChild(this->clip);

    //NOTE need to addChild this to something still

    return true;
};
