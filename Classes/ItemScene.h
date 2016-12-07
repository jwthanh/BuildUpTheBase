#ifndef ITEMSCENE_H
#define ITEMSCENE_H

#include "GameLayer.h"

namespace cocos2d
{
    namespace ui
    {
        class Layout;
        class Text;
        class ListView;
        class Button;
    }
}

class ItemScene : public GameLayer
{
    private:
        const std::string& get_scene_title();
        const std::string& get_default_detail_panel_title();
        const std::string& get_default_detail_panel_description();
        const std::string& get_sell_btn_text();

    public:
        CREATE_FUNC(ItemScene);
        static cocos2d::Scene* ItemScene::createScene();

        bool init() override;

        void init_children();
        cocos2d::ui::Layout* init_panel();
        void init_title_lbl(cocos2d::Node* panel, std::string title);
        void init_back_btn(cocos2d::Node* panel);
        cocos2d::ui::Button* init_sell_btn(cocos2d::Node* panel);

        virtual void init_callbacks(); //these should be changed in the child classes

        //nodes along the right side
        cocos2d::ui::Layout* panel;
        cocos2d::ui::ListView* items_listview;

        //nodes inside the detail panel
        cocos2d::ui::Layout* item_detail_panel;
        cocos2d::ui::ListView* item_listview_description;
        cocos2d::ui::Text* item_name;
        cocos2d::ui::Text* item_desc;
        cocos2d::ui::Button* item_sell_btn;
};

class AltarItemScene : public ItemScene
{
    private:
        const std::string& get_scene_title();
        const std::string& get_default_detail_panel_title();
        const std::string& get_default_detail_panel_description();
        const std::string& get_sell_btn_text();

    public:
        CREATE_FUNC(AltarItemScene);
        bool init() override;
        void init_callbacks() override;
        static cocos2d::Scene* AltarItemScene::createScene();
};

class EquipItemScene : public ItemScene
{
    private:
        const std::string& get_scene_title();
        const std::string& get_default_detail_panel_title();
        const std::string& get_default_detail_panel_description();
        const std::string& get_sell_btn_text();

    public:
        CREATE_FUNC(EquipItemScene);
        bool init() override;
        void init_callbacks() override;
        static cocos2d::Scene* EquipItemScene::createScene();
};

#endif
