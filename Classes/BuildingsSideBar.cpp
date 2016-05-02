#include "BuildingsSideBar.h"

#include "cocos2d.h"
#include "cocostudio/ActionTimeline/CSLoader.h"
#include "ui/CocosGUI.h"

#include "MiscUI.h"

USING_NS_CC;

SideListView::SideListView(Node* parent) : parent(parent)
{
    this->setup_listviews();
    this->setup_tab_buttons();
};

ui::Button* SideListView::_create_button(std::string node_name)
{
    auto inst = CSLoader::getInstance();
    Node* harvest_scene_editor = inst->createNode("editor/scenes/base_scene.csb");

    ui::Button* button = dynamic_cast<ui::Button*>(harvest_scene_editor->getChildByName(node_name));
    load_default_button_textures(button);
    button->removeFromParent();

    this->parent->addChild(button);

    return button;
};

void SideListView::setup_tab_buttons()
{
    auto toggle_buttons = [this](Ref* target, ui::Widget::TouchEventType evt) {
        if (evt == ui::Widget::TouchEventType::ENDED) {
            this->shop_listview->setVisible(false);
            this->detail_listview->setVisible(false);

            this->tab_shop_btn->setEnabled(true);
            this->tab_detail_btn->setEnabled(true);

            ui::Button* target_button = dynamic_cast<ui::Button*>(target);
            target_button->setEnabled(false);

            if (target_button == this->tab_shop_btn) { this->shop_listview->setVisible(true); }
            else if (target_button == this->tab_detail_btn) { this->detail_listview->setVisible(true); }
        };
    };

    this->tab_shop_btn = this->_create_button("tab_1_btn");
    this->tab_shop_btn->addTouchEventListener(toggle_buttons);

    this->tab_detail_btn = this->_create_button("tab_2_btn");
    this->tab_detail_btn->addTouchEventListener(toggle_buttons);

    //prepress the shop button
    this->tab_shop_btn->setEnabled(false);

}

void SideListView::setup_listviews()
{
    this->shop_listview = this->_create_listview("shop_listview");
    this->detail_listview = this->_create_listview("detail_listview");
};

ui::ListView* SideListView::_create_listview(std::string node_name)
{
    auto inst = CSLoader::getInstance();
    Node* harvest_scene_editor = inst->createNode("editor/scenes/base_scene.csb");

    auto listview = static_cast<ui::ListView*>(harvest_scene_editor->getChildByName(node_name));
    listview->removeFromParent();
    this->parent->addChild(listview);

    return listview;
}
