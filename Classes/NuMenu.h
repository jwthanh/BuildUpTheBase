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
class Beatup;

class NuItem : public cocos2d::ui::Widget
{
    public:
        NuItem(cocos2d::Node* parent);

        cocos2d::ui::Button* button;
        cocos2d::ui::ImageView* item_icon;
        cocos2d::ui::Text* title_lbl;
        cocos2d::ui::Text* desc_lbl;
        cocos2d::ui::Text* cost_lbl;

        void set_image(std::string path);
        void set_title(std::string title);
        void set_description(std::string description);
        void set_cost(std::string cost);
};

class NuMenu : public GameLayer
{
    public:
        static NuMenu* create(Beatup* beatup);

        bool init();

        Beatup* beatup;
        cocos2d::Scene* beatup_scene;

};
#endif
