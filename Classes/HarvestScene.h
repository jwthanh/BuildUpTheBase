#pragma once
#ifndef HARVESTSCENE_H
#define HARVESTSCENE_H

#include "GameLayer.h"
#include "Resources.h"

class BaseScene : public GameLayer
{
    public:
        CREATE_FUNC(BaseScene);


        virtual bool init();
        virtual void update(float dt);
        virtual void onKeyReleased(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event *pEvent) override;

        virtual void onEnter() override;

        void create_side_buttons();
        void create_shop_button();
        void create_city_button();
        void create_detail_button();

        void create_info_panel();
        void create_player_info_panel();
        void create_building_scrollview();
        void create_inventory_pageview();
        void create_shop_pageview();

        cocos2d::ui::Widget* create_detail_alert(Ingredient::SubType ing_type);


};

class HarvestScene : public BaseScene
{
    public:
        CREATE_FUNC(HarvestScene);
        virtual bool init();
        virtual void update(float dt);
        cocos2d::ui::Text* recipe_lbl;
        spRecipe target_recipe;

        void create_recipe_lbl();


        void add_harvestable();
};

#endif
