#pragma once
#ifndef HARVESTSCENE_H
#define HARVESTSCENE_H

#include "GameLayer.h"
#include "Resources.h"
#include "magic_particles/_core/magic.h"
//#include "magic_particles/MagicEmitter.h"
//#include "Harvestable.h"

class SideListView;
class PopupPanel;
class Harvestable;
class Miner;
class MagicEmitter;

class BaseScene : public GameLayer
{
    private:
        static Node* _harvest_scene_from_editor;

    public:

        MagicEmitter* cur_node;
        int t_emitter;
        int k_emitter;
        HM_EMITTER m_emitter[1000];
        void SelectEmitter(int emitter);

        CREATE_FUNC(BaseScene);

        Node* get_original_scene_from_editor();

        virtual bool init();
        virtual void update(float dt);

        virtual void onKeyReleased(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event *pEvent) override;
        virtual void onEnter() override;
        virtual void onSwipeLeft(float dt) override;
        virtual void onSwipeRight(float dt) override;

        std::shared_ptr<SideListView> sidebar;
        std::shared_ptr<PopupPanel> popup_panel;

        cocos2d::ui::Text* upgrade_lbl;
        spRecipe target_recipe;

        std::shared_ptr<Miner> miner;

        void create_goal_loadingbar();
        void create_info_panel();
        void create_username_input();
        void create_player_info_panel();
        void create_building_choicelist();
        void create_inventory_listview();
        void create_shop_listview();
        void create_popup_panel();

        cocos2d::ui::Widget* create_ingredient_detail_alert(Ingredient::SubType ing_type);

        void scroll_to_target_building();

        Harvestable* harvestable; //cant be shared_ptr because its cocos2d::Node
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
