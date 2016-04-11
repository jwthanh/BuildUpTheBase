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

        bool init() override;
        virtual void animate_harvest();
        virtual void init_sprite();
        virtual std::string get_sprite_path();
    void init_clicks();
    void onTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event) override;

        cocos2d::ClippingNode* clip;
        cocos2d::DrawNode* stencil;
        cocos2d::Sprite* sprite;

        int click_limit;
        int current_clicks;

        void shatter();

};

class MiningHarvestable : public Harvestable
{
    public:
        CREATE_FUNC(MiningHarvestable);
        void init_sprite() override;
        virtual void animate_harvest() override;
};

class CraftingHarvestable : public Harvestable
{
    public:
        spRecipe recipe;
        CraftingHarvestable(spRecipe recipe);
        static CraftingHarvestable* create(spRecipe recipe);

        void init_clicks();
        std::string get_sprite_path() override;

        virtual void animate_harvest() override;
};

#endif
