#pragma once
#ifndef NUMENU_H
#define NUMENU_H

#include "cocos2d.h"
#include "GameLayer.h"
#include "Buyable.h"
#include "Worker.h"
#include "Resources.h"

class PlainShopItem;
class Fist;
class ShopItem;
class Scrollable;
class Building;


class NuItem : public cocos2d::ui::Widget
{
    public:
        CREATE_FUNC(NuItem);

        NuItem(){};
        virtual void my_init(cocos2d::Node* parent);

        cocos2d::ui::Button* button;
        cocos2d::ui::ImageView* item_icon;
        cocos2d::ui::Text* title_lbl;
        cocos2d::ui::Text* desc_lbl;
        cocos2d::ui::Text* cost_lbl;
        cocos2d::ui::Text* count_lbl;

        void set_image(std::string path);
        void set_title(std::string title);
        void set_description(std::string description);
        void set_cost_lbl(std::string cost);
        void set_count_lbl(int count);

        void try_set_enable(bool is_enable);

        void set_touch_ended_callback(std::function<void(void)> callback);

        //handle stuff like been bought, can be pressed etc
        virtual void update_func(float dt) {};
};

class RecipeNuItem : public NuItem
{
    public:
        CREATE_FUNC(RecipeNuItem);

        spRecipe recipe;
        spBuilding building;

        virtual void my_init(spRecipe recipe, std::shared_ptr<Building> building, Node* parent);
        virtual void update_func(float dt);
};

class ShopNuItem : public Buyable, public NuItem
{
    public:
        CREATE_FUNC(ShopNuItem);
        ShopNuItem() : Buyable("unset_in_shopnuitem"), resource_cost(NULL) {};

        ResourceCondition* resource_cost;

        virtual void my_init(Node* parent, std::string id_key);
        virtual void update_func(float dt);
};

class BuildingShopNuItem : public ShopNuItem
{
    public:
        CREATE_FUNC(BuildingShopNuItem);
        BuildingShopNuItem(){};

        std::shared_ptr<Building> building;
        virtual void my_init(std::shared_ptr<Building> building, Node* parent);
};

class HarvesterShopNuItem : public ShopNuItem
{
    public:
        CREATE_FUNC(HarvesterShopNuItem);

    virtual void my_init(Node* parent, Harvester::SubType harv_type, Ingredient::SubType ing_type);
    void my_init_title();
    void my_init_sprite();
    void my_init_touch_ended_callback();
    void my_init_update_callback();

    Harvester::SubType harv_type;
    Ingredient::SubType ing_type;
};

class NuMenu : public GameLayer
{
    public:
        CREATE_FUNC(NuMenu);
        virtual void onEnter() override;

        Scrollable* scrollable;

        bool init();
        void create_back_item(cocos2d::Node* parent);

        virtual void init_items(){};


};

class BuyBuildingsNuMenu : public NuMenu
{
    public:
        CREATE_FUNC(BuyBuildingsNuMenu);
        void init_items();
};

class BuildingNuMenu : public NuMenu
{
    public:
        std::shared_ptr<Building> building;

        static BuildingNuMenu* create(std::shared_ptr<Building> building);
        void init_items();

        void create_inventory_item(cocos2d::Node* parent);
};

#endif
