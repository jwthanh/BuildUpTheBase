#pragma once
#ifndef HARVESTABLE_H
#define HARVESTABLE_H

#include "cocos2d.h"
#include "ui/CocosGUI.h"

#include "GameLayer.h"

class Beatup;

class Harvestable : public cocos2d::ui::Widget
{
    //needs to do something when harvested, animations
    //
    //it might represent a building or a specific resource
    public:
        spBuilding building;

        CREATE_FUNC(Harvestable);
        void onTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event) override;

        virtual bool init() override;
        virtual std::string get_sprite_path();
        virtual void init_sprite();
        cocos2d::Size get_sprite_size();

        virtual void init_clicks();

        cocos2d::ClippingNode* clip;
        cocos2d::DrawNode* stencil;
        cocos2d::Sprite* sprite;

        int click_limit;
        int current_clicks;

        float get_click_ratio() const;

        virtual bool should_shatter();
        virtual void animate_harvest();
        virtual void animate_rotate();
        virtual void animate_clip();
        virtual void shatter();
        virtual void on_harvest();

};

class MiningHarvestable : public Harvestable
{
    public:
        CREATE_FUNC(MiningHarvestable);
        void init_sprite() override;

        virtual void animate_clip() override;
        virtual void animate_rotate() override;
};

class CraftingHarvestable : public Harvestable
{
    public:
        spRecipe recipe;
        CraftingHarvestable(spRecipe recipe);
        static CraftingHarvestable* create(spRecipe recipe);

        void init_clicks() override;
        std::string get_sprite_path() override;

        virtual void animate_clip() override;
        virtual void shatter() override;
};

class FightingHarvestable : public Harvestable
{
    public:
        CREATE_FUNC(FightingHarvestable);
        bool init() override;

        std::string get_sprite_path() override;

        virtual bool should_shatter() override;
        virtual void animate_clip() override;
        virtual void animate_rotate() override;

        virtual void on_harvest() override;

};

#endif
