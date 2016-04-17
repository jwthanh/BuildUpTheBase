#pragma once
#ifndef HARVESTSCENE_H
#define HARVESTSCENE_H

#include "GameLayer.h"

class HarvestScene : public GameLayer
{
    public:
        CREATE_FUNC(HarvestScene);

        cocos2d::ui::Text* recipe_lbl;
        spRecipe target_recipe;

        bool init();
        void update(float dt);
        virtual void onKeyReleased(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event *pEvent) override;

        void create_side_buttons();
        void create_shop_button();
        void create_city_button();
        void create_detail_button();

        void create_recipe_lbl();

        void create_info_panel();
        void create_player_info_panel();

        void add_harvestable();

};

#endif
