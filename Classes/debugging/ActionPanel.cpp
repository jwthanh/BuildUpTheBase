#include "ActionPanel.h"

#include "2d/CCMenuItem.h"

#include "NodeBuilder.h"
#include "MiscUI.h"

bool ActionPanel::init()
{
    auto node = get_prebuilt_node_from_csb("panels/animation_panel.csb");
    this->_panel = dynamic_cast<cocos2d::ui::Layout*>(node->getChildByName("animation_panel"));
    this->_panel->removeFromParent();
    this->_panel->setPosition({ 460, 320 });

    this->addChild(this->_panel);

    this->_target = NULL;
    this->_duration = 1.0f;
    this->_target_x = 500.0f;
    this->_target_y = 500.0f;
    this->_rate = 10.0f;

    return true;
};

void ActionPanel::set_target(cocos2d::Node* target)
{
    this->_target = target;

    this->init_action_buttons();
};

void ActionPanel::init_action_buttons()
{
    auto listview = dynamic_cast<cocos2d::ui::ListView*>(this->_panel->getChildByName("actions_listview"));

    for (int i = 0; i < 25; i++)
    {
        auto menu_btn = cocos2d::ui::Button::create();
        prep_button(menu_btn);
        menu_btn->setScale9Enabled(true);
        menu_btn->setContentSize({ 150.0f, 20.0f });
        menu_btn->setTitleText("first item");
        listview->addChild(menu_btn);
    }
};
