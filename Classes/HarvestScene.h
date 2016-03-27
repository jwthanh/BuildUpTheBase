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

        cocos2d::ui::Button* create_shop_button();
        cocos2d::ui::Button* create_city_button();
        cocos2d::ui::Button* create_detail_button();

        cocos2d::ui::Layout* create_info_panel();

        void add_harvestable();

};

#endif
