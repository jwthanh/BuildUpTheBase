#pragma once
#ifndef HARVESTSCENE_H
#define HARVESTSCENE_H

#include "GameLayer.h"

class HarvestScene : public GameLayer
{
    public:
        CREATE_FUNC(HarvestScene);
        bool init();
        void update(float dt);

        virtual void onKeyReleased(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event *pEvent) override;

        void add_harvestable();

};

#endif
