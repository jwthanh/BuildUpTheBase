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

class NuItem : public Buyable, public cocos2d::ui::Widget
{
    NuItem(std::string id_key) : Buyable(id_key) {

    };
};

class NuMenu : public GameLayer
{
    public:
        CREATE_FUNC(NuMenu);

        void menu_init();

        Beatup* beatup;
        cocos2d::Scene* beatup_scene;

};
#endif
