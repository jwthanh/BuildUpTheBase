#pragma once
#ifndef MAINMENU_H
#define MAINMENU_H


#include "ShopScene.h"

#include "constants.h"

class MainMenu : public Shop
{
    public:
        CREATE_FUNC(MainMenu);
        static cocos2d::Scene* createScene();

        static void cleanup_beatup_scene();
        static cocos2d::Scene* beatup_scene;


        virtual void pop_scene(cocos2d::Ref* pSender) { GameLayer::pop_scene(pSender); }; //use GameLayer instead of ShopScene's pop_scene

        virtual void onKeyReleased(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event *pEvent) override;

        bool init() override;
        static bool quick_start_game();
        static bool resume_game();
        static bool open_options();
        static bool open_levelselect();
        static bool open_objective();

};

class OptionsMenu : public Shop
{
    public:
        CREATE_FUNC(OptionsMenu);
        static cocos2d::Scene* createScene();

        virtual void pop_scene(cocos2d::Ref* pSender) { GameLayer::pop_scene(pSender); }; //use GameLayer instead of ShopScene's pop_scene

        virtual void onKeyReleased(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event *pEvent) override;

        bool init() override;

        static bool open_reset();

        static bool toggle_sound();
        static bool toggle_music();
        static bool toggle_tutorials();
        static bool toggle_vibration();

};

class LevelSelectMenu : public Shop
{
    public:
        CREATE_FUNC(LevelSelectMenu);
        static cocos2d::Scene* createScene();

        virtual void pop_scene(cocos2d::Ref* pSender) { GameLayer::pop_scene(pSender); }; //use GameLayer instead of ShopScene's pop_scene

        virtual void onKeyReleased(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event *pEvent) override;

        bool init() override;

        BoolFuncNoArgs start_new_game(int level);

};

class ObjectiveMenu : public Shop
{
    public:
        CREATE_FUNC(ObjectiveMenu);
        static cocos2d::Scene* createScene();

        virtual void pop_scene(cocos2d::Ref* pSender) { GameLayer::pop_scene(pSender); }; //use GameLayer instead of ShopScene's pop_scene

        virtual void onKeyReleased(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event *pEvent) override;

        bool init() override;

};

class ResetMenu : public Shop
{
    public:
        CREATE_FUNC(ResetMenu);
        static cocos2d::Scene* createScene();

        virtual void pop_scene(cocos2d::Ref* pSender) { GameLayer::pop_scene(pSender); }; //use GameLayer instead of ShopScene's pop_scene

        virtual void onKeyReleased(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event *pEvent) override;

        bool init() override;

        static bool reset_all();
        static bool reset_total_hit_stat();
        static bool reset_total_coin_stat();
        static bool reset_all_combos();
        static bool reset_all_fist_weapons();
        static bool reset_levels();

};

#endif
