#pragma once
#ifndef HARVESTABLE_H
#define HARVESTABLE_H

#include "cocos2d.h"
#include <cocos2d/cocos/ui/UIWidget.h>
#include <cocos2d/cocos/ui/UILayout.h>

#include "GameLayer.h"

class Beatup;

class Harvestable : public cocos2d::ui::Widget
{
    //needs to do something when harvested, animations
    //
    //it might represent a building or a specific resource
    public:
        Beatup* beatup;
        BEATUP_CREATE_FUNC(Harvestable);

        cocos2d::ClippingNode* clip;
        cocos2d::DrawNode* stencil;
        cocos2d::Sprite* sprite;

        bool init() override;
        void onTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event) override;
};

#endif
