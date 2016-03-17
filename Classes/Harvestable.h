#pragma once
#ifndef HARVESTABLE_H
#define HARVESTABLE_H

#include "cocos2d.h"

class Harvestable : public cocos2d::Node
{
    //needs to do something when harvested, animations
    //
    //it might represent a building or a specific resource
    public:
        CREATE_FUNC(Harvestable);

        cocos2d::ClippingNode* clip;
        cocos2d::DrawNode* stencil;
        cocos2d::Sprite* sprite;

        bool init() override;
};

#endif
