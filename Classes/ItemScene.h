#ifndef ITEMSCENE_H
#define ITEMSCENE_H

#include "GameLayer.h"

#include "Item.h"

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
        virtual const std::string& get_scene_title();
        virtual const std::string& get_default_detail_panel_title();
        virtual const std::string& get_default_detail_panel_description();
        virtual const std::string& get_sell_btn_text();

    public:
        static ItemScene* create(ItemSlotType slot_type=ItemSlotType::Unset);
        static cocos2d::Scene* createScene(ItemSlotType slot_type=ItemSlotType::Unset);

        virtual bool init(ItemSlotType slot_type=ItemSlotType::Unset);

        virtual void init_children();
        virtual cocos2d::ui::Layout* init_panel();
        virtual void init_title_lbl(cocos2d::Node* panel, std::string title);
        virtual void init_back_btn(cocos2d::Node* panel);
        cocos2d::ui::Button* init_sell_btn(cocos2d::Node* panel);

        virtual void init_callbacks(); //these should be changed in the child classes
        virtual void reset_detail_panel();

        virtual vsItem get_items();
        ItemSlotType filtered_slot_type;

        //nodes along the right side
        cocos2d::ui::Layout* panel;
        cocos2d::ui::ListView* items_listview;

        //nodes inside the detail panel
        cocos2d::ui::Layout* item_detail_panel;

        cocos2d::ui::Layout* item_misc_info_panel;
        cocos2d::ui::Text* item_misc_info_header_lbl;
        cocos2d::ui::Text* item_misc_info_body_lbl;

        cocos2d::ui::ListView* item_listview_description;
        cocos2d::ui::Text* item_name;
        cocos2d::ui::Text* item_desc;
        cocos2d::ui::Button* item_sell_btn;
};

class AltarItemScene : public ItemScene
{
    private:
        const std::string& get_scene_title() override;
        const std::string& get_default_detail_panel_title() override;
        const std::string& get_default_detail_panel_description() override;
        const std::string& get_sell_btn_text() override;

    public:
        static AltarItemScene* create(ItemSlotType slot_type=ItemSlotType::Unset);
        virtual bool init(ItemSlotType slot_type=ItemSlotType::Unset) override;
        void init_callbacks() override;
        static cocos2d::Scene* createScene(ItemSlotType slot_type=ItemSlotType::Unset);
};

class ChanceItemScene : public ItemScene
{
    private:
        const std::string& get_scene_title() override;
        const std::string& get_default_detail_panel_title() override;
        const std::string& get_default_detail_panel_description() override;
        const std::string& get_sell_btn_text() override;

        void convert_item_upgrade(std::stringstream& body_ss, const spItem& item);
        void convert_item_resource(std::stringstream& body_ss, const spItem& item);
        void convert_item_coins(std::stringstream& body_ss, const spItem& item);

    public:
        static ChanceItemScene* create(ItemSlotType slot_type=ItemSlotType::Unset);
        virtual bool init(ItemSlotType slot_type=ItemSlotType::Unset) override;
        void init_callbacks() override;
        static cocos2d::Scene* createScene(ItemSlotType slot_type=ItemSlotType::Unset);

        void convert_item_to_chance(spItem item);

        virtual void onKeyReleased(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event *pEvent);
};

class EquipItemScene : public ItemScene
{
    private:
        const std::string& get_scene_title() override;
        const std::string& get_default_detail_panel_title() override;
        const std::string& get_default_detail_panel_description() override;
        const std::string& get_sell_btn_text() override;

    public:
        static EquipItemScene* create(ItemSlotType slot_type=ItemSlotType::Unset);
        virtual bool init(ItemSlotType slot_type=ItemSlotType::Unset) override;
        void init_callbacks() override;
        static cocos2d::Scene* createScene(ItemSlotType slot_type=ItemSlotType::Unset);
};

class ScrapItemScene : public ItemScene
{
    private:
        const std::string& get_scene_title() override;
        const std::string& get_default_detail_panel_title() override;
        const std::string& get_default_detail_panel_description() override;
        const std::string& get_sell_btn_text() override;

    public:
        static ScrapItemScene* create();
        virtual bool init() override;
        void init_callbacks() override;
        static cocos2d::Scene* createScene();
};

#endif
