#pragma once
#ifndef BUILDINGSSIDEBAR_H
#define BUILDINGSSIDEBAR_H
#include <string>
#include <memory>
#include "ui/CocosGUI.h"

namespace cocos2d
{
    class Ref;
    class Node;

    namespace ui
    {
        class ListView;
        class Button;
    }
}

class Building;

///Base ListView for sidebar
/// 
///It creates a ui::ListView specific to each building
class SideListView
{
    public:
        std::shared_ptr<Building> current_target;
        cocos2d::Node* parent;

        cocos2d::ui::ListView* shop_listview;
        cocos2d::ui::ListView* detail_listview;
        cocos2d::ui::ListView* building_listview;
        cocos2d::ui::ListView* powers_listview;

        cocos2d::ui::Button* tab_shop_btn;
        cocos2d::ui::Button* tab_detail_btn;
        cocos2d::ui::Button* tab_building_btn;
        cocos2d::ui::Button* tab_powers_btn;


        SideListView(cocos2d::Node* parent, std::shared_ptr<Building> current_target);

        virtual cocos2d::ui::ListView* _create_listview(std::string node_name);
        virtual cocos2d::ui::Button* _create_button(std::string node_name);

        void toggle_buttons(cocos2d::Ref* target, cocos2d::ui::Widget::TouchEventType evt);
        void setup_tab_buttons();

        void setup_listviews();
        bool try_push_back(int child_tag, cocos2d::ui::ListView* listview);

        void setup_shop_listview_as_harvesters();
        void setup_detail_listview_as_recipes();
        void setup_building_listview_as_upgrades();
        void setup_powers_listview_as_powers();
};

#endif
