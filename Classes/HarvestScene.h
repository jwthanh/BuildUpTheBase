#pragma once
#ifndef HARVESTSCENE_H
#define HARVESTSCENE_H

#include "GameLayer.h"
#include "Resources.h"

class SideListView;

class BaseScene : public GameLayer
{
    private:
        static Node* _harvest_scene_from_editor;

    public:
        CREATE_FUNC(BaseScene);

        Node* get_original_scene_from_editor();

        virtual bool init();
        virtual void update(float dt);

        virtual void onKeyReleased(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event *pEvent) override;
        virtual void onEnter() override;
        virtual void onSwipeLeft(float dt) override;
        virtual void onSwipeRight(float dt) override;

        std::shared_ptr<SideListView> sidebar;

        cocos2d::ui::Text* recipe_lbl;
        spRecipe target_recipe;
        void create_goal_loadingbar();

        void create_info_panel();
        void create_username_input();
        void create_player_info_panel();
        void create_building_choicelist();
        void create_inventory_listview();
        void create_shop_listview();

        cocos2d::ui::Widget* create_detail_alert(Ingredient::SubType ing_type);

        void scroll_to_target_building();

};

class HarvestScene : public BaseScene
{
    private:
        int _layer_building_level;

    public:
        CREATE_FUNC(HarvestScene);
        virtual bool init();
        virtual void update(float dt);

        Clock* autosave_clock;

        void create_recipe_lbl();

        void add_harvestable();
};

#endif
