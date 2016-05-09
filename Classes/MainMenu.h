#pragma once
#ifndef MAINMENU_H
#define MAINMENU_H


#include "constants.h"
#include "GameLayer.h"

class BuildingNode : public cocos2d::Node 
{
    public:
        BuildingNode();
        CREATE_FUNC(BuildingNode);

        spBuilding building;
        void set_building(spBuilding bldg);

        cocos2d::LayerColor* bg_layer;
        cocos2d::Label* building_label;

};


class CharacterMenu : public GameLayer
{
    public:
        spFighter fighter;

        cocos2d::Label* main_lbl;

        static CharacterMenu* create(spFighter fighter);
        static cocos2d::Scene* createScene();

        virtual void onKeyPressed(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event *pEvent) override;
        virtual void onKeyReleased(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event *pEvent);
        virtual void onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event);
        bool init() override;

};

#endif
