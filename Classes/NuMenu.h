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

        static cocos2d::ui::Button* orig_button;
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
        void set_count_lbl(res_count_t count);

        void try_set_enable(bool is_enable);

        void set_touch_ended_callback(std::function<void(void)> callback);

        //handle stuff like been bought, can be pressed etc
        virtual void update_func(float dt) {};
};

class BuildingNuItem : public NuItem
{
    public:
        spBuilding building;

        CREATE_FUNC(BuildingNuItem);
        virtual void my_init(std::shared_ptr<Building> building, Node* parent);
};

class RecipeNuItem : public BuildingNuItem
{
    public:
        CREATE_FUNC(RecipeNuItem);

        spRecipe recipe;

        virtual void other_init(spRecipe recipe);
        virtual void update_func(float dt) override;
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

class UpgradeBuildingShopNuItem : public BuildingShopNuItem
{
    public:
        CREATE_FUNC(UpgradeBuildingShopNuItem);
        UpgradeBuildingShopNuItem(){};
        virtual void my_init(std::shared_ptr<Building> building, Node* parent) override;
};

class HarvesterShopNuItem : public ShopNuItem
{
    public:
        CREATE_FUNC(HarvesterShopNuItem);

    virtual void my_init(Node* parent, WorkerSubType harv_type, Ingredient::SubType ing_type);
    virtual void my_init_title();
    virtual void my_init_sprite();
    virtual void my_init_touch_ended_callback();
    virtual void my_init_update_callback();

    WorkerSubType harv_type;
    Ingredient::SubType ing_type;
};

class SalesmanShopNuItem : public HarvesterShopNuItem
{
    public:
        CREATE_FUNC(SalesmanShopNuItem);

        virtual void my_init_title() override;
        virtual void my_init_sprite() override;
        void my_init_touch_ended_callback() override;
        void my_init_update_callback() override;
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
