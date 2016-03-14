#pragma once
#ifndef NUMENU_H
#define NUMENU_H

#include "cocos2d.h"
#include "GameLayer.h"
#include "Buyable.h"

class PlainShopItem;
class Fist;
class ShopItem;
class Beatup;
class Scrollable;
class Building;


class NuItem : public cocos2d::ui::Widget
{
    public:
        CREATE_FUNC(NuItem);
        Beatup* beatup;

        NuItem(){};
        void my_init(Beatup* beatup, cocos2d::Node* parent);

        cocos2d::ui::Button* button;
        cocos2d::ui::ImageView* item_icon;
        cocos2d::ui::Text* title_lbl;
        cocos2d::ui::Text* desc_lbl;
        cocos2d::ui::Text* cost_lbl;

        void set_image(std::string path);
        void set_title(std::string title);
        void set_description(std::string description);
        void set_cost(std::string cost);

        //handle stuff like been bought, can be pressed etc
        void update_func(float dt) {};
};

class ShopNuItem : public Buyable, public NuItem
{
    public:
        CREATE_FUNC(ShopNuItem);
        ShopNuItem():Buyable("unset_in_shopnuitem") {};

        void my_init(Beatup* beatup, Node* parent, std::string id_key);
        void update_func(float dt);
};

class BuildingShopNuItem : public ShopNuItem
{
    public:
        CREATE_FUNC(BuildingShopNuItem);
        BuildingShopNuItem(){};

        std::shared_ptr<Building> building;
        void my_init(std::shared_ptr<Building> building, Node* parent);
};

class NuMenu : public GameLayer
{
    public:
        BEATUP_CREATE_FUNC(NuMenu);

        bool init();
        virtual void init_items(){};

        Beatup* beatup;

};

class BuyBuildingsNuMenu : public NuMenu
{
    public:
        BEATUP_CREATE_FUNC(BuyBuildingsNuMenu);
        void init_items();
};

class BuildingNuMenu : public NuMenu
{
    public:
        std::shared_ptr<Building> building;
        static BuildingNuMenu* BuildingNuMenu::create(Beatup* beatup, std::shared_ptr<Building> building);
        void init_items();

        void create_inventory_item(cocos2d::Node* parent);
};

#endif
