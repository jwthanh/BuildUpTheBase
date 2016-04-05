#pragma once
#ifndef MAINMENU_H
#define MAINMENU_H


#include "BaseMenu.h"

#include "constants.h"
#include "HouseBuilding.h"

class MainMenu : public BaseMenu
{
    public:
        CREATE_FUNC(MainMenu);
        static cocos2d::Scene* createScene();

        static void cleanup_beatup_scene();
        static cocos2d::Scene* beatup_scene;


        virtual void pop_scene(cocos2d::Ref* pSender) { GameLayer::pop_scene(pSender); }; //use GameLayer instead of ShopScene's pop_scene

        virtual void onKeyPressed(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event *pEvent) override;
        void onKeyReleased(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event);

        bool init() override;
        static bool quick_start_game();
        static bool resume_game();
        static bool open_options();
        static bool open_levelselect();
        static bool open_objective();

};

class OptionsMenu : public BaseMenu
{
    public:
        CREATE_FUNC(OptionsMenu);
        static cocos2d::Scene* createScene();

        virtual void pop_scene(cocos2d::Ref* pSender) { GameLayer::pop_scene(pSender); }; //use GameLayer instead of ShopScene's pop_scene

        virtual void onKeyPressed(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event *pEvent) override;

        bool init() override;

        static bool open_reset();

        static bool toggle_sound();
        static bool toggle_music();
        static bool toggle_tutorials();
        static bool toggle_vibration();

};

class LevelSelectMenu : public BaseMenu
{
    public:
        CREATE_FUNC(LevelSelectMenu);
        static cocos2d::Scene* createScene();

        virtual void pop_scene(cocos2d::Ref* pSender) { GameLayer::pop_scene(pSender); }; //use GameLayer instead of ShopScene's pop_scene

        virtual void onKeyPressed(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event *pEvent) override;

        bool init() override;

        BoolFuncNoArgs start_new_game(int level);

};

class ObjectiveMenu : public BaseMenu
{
    public:
        CREATE_FUNC(ObjectiveMenu);
        static cocos2d::Scene* createScene();

        virtual void pop_scene(cocos2d::Ref* pSender) { GameLayer::pop_scene(pSender); }; //use GameLayer instead of ShopScene's pop_scene

        virtual void onKeyPressed(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event *pEvent) override;

        bool init() override;

};

class ResetMenu : public BaseMenu
{
    public:
        CREATE_FUNC(ResetMenu);
        static cocos2d::Scene* createScene();

        virtual void pop_scene(cocos2d::Ref* pSender) { GameLayer::pop_scene(pSender); }; //use GameLayer instead of ShopScene's pop_scene

        virtual void onKeyPressed(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event *pEvent) override;

        bool init() override;

        static bool reset_all();
        static bool reset_total_hit_stat();
        static bool reset_total_coin_stat();
        static bool reset_levels();

};


class CityMenu : public GameLayer
{
    public:
        cocos2d::Label* main_lbl;

        CREATE_FUNC(CityMenu);

        virtual void pop_scene(cocos2d::Ref* pSender) { GameLayer::pop_scene(pSender); }; //use GameLayer instead of ShopScene's pop_scene

        virtual void onKeyPressed(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event *pEvent) override;

        bool init() override;

};

class InventoryMenu : public GameLayer
{
    public:
        spBuilding building;

        cocos2d::Label* main_lbl;

        static InventoryMenu* create(spBuilding building);
        static cocos2d::Scene* createScene();

        virtual void onKeyPressed(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event *pEvent) override;

        cocos2d::ui::Widget* create_detail_alert(spBuilding building, Ingredient::IngredientType type);

        bool init() override;

};

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
