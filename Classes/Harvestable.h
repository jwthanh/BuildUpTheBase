#pragma once
#ifndef HARVESTABLE_H
#define HARVESTABLE_H

#include "GameLayer.h"

#include "SubTypes.h"
#include "ui/UIWidget.h"

namespace cocos2d
{
    class MotionStreak;
    class NodeGrid;
    class Sprite;
    class ClippingNode;
    class DrawNode;

    namespace ui
    {
        class Text;
        class Widget;
    }
}


class Beatup;

class Harvestable : public cocos2d::ui::Widget
{
    private:
        IngredientSubType _current_output_ing_type;
        res_count_t _current_clicks;


    //needs to do something when harvested, animations
    //
    //it might represent a building or a specific resource
    public:
        spBuilding building;

        float initial_scale;

        CREATE_FUNC(Harvestable);

        virtual bool init() override;
        virtual std::string get_sprite_path();
        virtual void init_sprite();
        cocos2d::Size get_sprite_size();

        virtual void init_clicks();

        cocos2d::ClippingNode* clip;
        cocos2d::DrawNode* stencil;
        cocos2d::Sprite* sprite;

        int click_limit;
        res_count_t get_current_clicks() const;
        void reset_current_clicks();
        void set_current_clicks(res_count_t new_clicks);
        void add_current_clicks(res_count_t new_clicks);

        bool onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event) override;

        virtual void animate_touch_start(cocos2d::Touch* touch);
        std::string get_create_output_message();
        virtual cocos2d::Color4B get_create_output_color();
        virtual res_count_t get_per_touch_output();

        IngredientSubType get_output_ing_type() const;
        void set_output_ing_type(IngredientSubType ing_type);

        void spawn_label_on_touch(cocos2d::Touch* touch, float end_scale, float duration, std::string floating_msg, cocos2d::Color4B text_color);

		// rough outline of call order
		/* onTouchBegan -> animate_touch_start
         * [animate_touch_start] -> get_per_touch_output
         * [animate_touch_start] -> run_flash_action
         [animate_touch_start] -> spawn_label_on_touch

         * onTouchEnded -> [animate_touch_end]
         * [animate_touch_end] -> animate_harvest -> animate_clip ->animate_rotate
         * [animate_touch_end] -> on_harvest
         * [animate_touch_end] -> should_shatter -> get_shatter_text -> shatter
         */

        void onTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event) override;
        virtual void animate_touch_end(cocos2d::Touch* touch);

		bool is_harvestable();
		void handle_harvest();
		bool harvest_handle();
		virtual void animate_harvest();

        virtual void on_harvest();
        virtual void animate_clip();
        virtual void animate_rotate();

        void try_to_shatter();
        virtual bool should_shatter();
        virtual std::string get_shatter_text();
        virtual cocos2d::Color4B get_shatter_text_color();
        bool is_shattering;
        virtual void shatter();

        virtual float get_click_ratio() const;

};

class FarmingHarvestable : public Harvestable
{
    public:
        CREATE_FUNC(FarmingHarvestable);
        std::string get_sprite_path();
};

class GraveyardHarvestable : public Harvestable
{
    public:
        CREATE_FUNC(GraveyardHarvestable);
        std::string get_sprite_path();
};

class TreeHarvestable : public Harvestable
{
    public:
        CREATE_FUNC(TreeHarvestable);
        std::string get_sprite_path();
        void animate_touch_start(cocos2d::Touch* touch);

        virtual void shatter() override;

        virtual cocos2d::Color4B get_create_output_color();
};

class DumpsterHarvestable : public Harvestable
{

    public:
        CREATE_FUNC(DumpsterHarvestable);
        virtual void init_clicks() override;

        std::string get_sprite_path();

        virtual void on_harvest();
        virtual void animate_clip() override;

        virtual void shatter() override;

        spItem scavenge_item;
        void generate_item(); //builds the item this'll give the player
};

class MiningHarvestable : public Harvestable
{
    public:
        CREATE_FUNC(MiningHarvestable);
        virtual std::string get_sprite_path();

        virtual void animate_touch_start(cocos2d::Touch* touch);
        virtual void animate_clip() override;
        cocos2d::Color4B get_create_output_color();
        virtual void animate_rotate() override;
};

class CraftingHarvestable : public Harvestable
{
    public:
        spRecipe recipe;
        CraftingHarvestable(spRecipe recipe);
        static CraftingHarvestable* create(spRecipe recipe);

        bool can_satisfy_recipe_per_click();

        void init_clicks() override;
        std::string get_sprite_path() override;
        virtual float get_click_ratio() const override;

        void on_harvest();
        void animate_touch_start(cocos2d::Touch* touch);

        void onTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event) override;

        virtual void animate_clip() override;

        virtual bool should_shatter() override;
        virtual std::string get_shatter_text() override;
        virtual cocos2d::Color4B get_shatter_text_color() override;
        virtual void shatter() override;
};

class FightingHarvestable : public Harvestable
{
    public:
        CREATE_FUNC(FightingHarvestable);

        bool init() override;

        spFighter enemy;
        void spawn_enemy();

        bool is_critical_hit;

        std::string get_sprite_path() override;

        virtual bool should_shatter() override;
        virtual void shatter();

        virtual void animate_clip() override;
        virtual void animate_harvest() override;
        virtual void animate_rotate() override;

        virtual void animate_touch_start(cocos2d::Touch* touch);
        virtual void animate_touch_end(cocos2d::Touch* touch);

        virtual void on_harvest() override;
        res_count_t get_per_touch_output();
        int get_combat_crit_chance();
        res_count_t get_combat_crit_factor();
};

class UndeadHarvestable : public Harvestable
{
    public:
        CREATE_FUNC(UndeadHarvestable);
        virtual bool init() override;
        virtual std::string get_sprite_path() override;


        virtual void animate_clip() override;
        virtual void animate_rotate() override;
};

class MarketHarvestable : public Harvestable
{
    public:
        CREATE_FUNC(MarketHarvestable);
        virtual std::string get_sprite_path() override;
};
#endif
