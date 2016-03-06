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

class NuMenu : public GameLayer
{
    public:

        CREATE_FUNC(NuMenu);

        Beatup* beatup;
        cocos2d::Scene* beatup_scene;

};
#endif
