#include "ActionPanel.h"

#include <typeindex>
#include <typeinfo>

#include "2d/CCMenuItem.h"

#include "NodeBuilder.h"
#include "MiscUI.h"
#include "2d/CCActionInterval.h"
#include "GameLogic.h"

bool ActionPanel::init()
{
    auto node = get_prebuilt_node_from_csb("panels/animation_panel.csb");
    this->_panel = dynamic_cast<cocos2d::ui::Layout*>(node->getChildByName("animation_panel"));
    this->_panel->removeFromParent();
    this->_panel->setPosition({ 460, 320 });

    this->addChild(this->_panel);

    this->_target = NULL;
    // this->_duration = 1.0f;
    // this->_target_x = 500.0f;
    // this->_target_y = 500.0f;
    // this->_rate = 10.0f;

    return true;
};

void ActionPanel::set_target(cocos2d::Node* target)
{
    this->_target = target;

    this->_default_x = target->getPositionX();
    this->_default_y = target->getPositionY();

    this->init_action_buttons();
};

void ActionPanel::init_action_buttons()
{
    if (this->_target == NULL)
    {
        CCASSERT(false, "needs a target");
    }
    auto listview = dynamic_cast<cocos2d::ui::ListView*>(this->_panel->getChildByName("actions_listview"));

    //typedef std::unordered_map<std::type_index, int> type_map;
    //auto tmap = type_map();
    //tmap[typeid(cocos2d::MoveBy)] = 1

    auto create_button = [listview, this](std::string text, std::function<cocos2d::Action*()> build_action)
    {
        auto menu_btn = cocos2d::ui::Button::create();
        prep_button(menu_btn);
        menu_btn->setScale9Enabled(true);
        menu_btn->setContentSize({ 150.0f, 20.0f });
        menu_btn->setTitleText(text);
        listview->addChild(menu_btn);

        menu_btn->addTouchEventListener([build_action, this](cocos2d::Ref* target, TouchEventType evt)
        {
            if (evt == TouchEventType::ENDED)
            {
                this->reset_target();
                auto action = build_action();
                this->_target->runAction(action);
            }
        });

        return menu_btn;
    };

    auto move_by = [this](){
        return cocos2d::MoveBy::create(this->get_duration(), this->get_target_pos());
    };

    auto move_to = [this](){
        return cocos2d::MoveTo::create(this->get_duration(), this->get_target_pos());
    };

    create_button("move by", move_by);
    create_button("move to", move_to);

}

void ActionPanel::reset_target()
{
    this->_target->stopAllActions();
    this->_target->setPosition({this->_default_x, this->_default_y});
};

float ActionPanel::get_duration()
{
    return 1.0f; //TODO use slider
}

cocos2d::Vec2 ActionPanel::get_target_pos()
{
    return {this->get_target_pos_x(), this->get_target_pos_y()};
}

float ActionPanel::get_target_pos_x()
{
    auto text_input = dynamic_cast<cocos2d::ui::TextField*>(this->_panel->getChildByName("buttons_panel")->getChildByName("x_pos_text"));
    return std::stof(text_input->getString());

}

float ActionPanel::get_target_pos_y()
{
    auto text_input = dynamic_cast<cocos2d::ui::TextField*>(this->_panel->getChildByName("buttons_panel")->getChildByName("y_pos_text"));
    return std::stof(text_input->getString());
}

float ActionPanel::get_rate()
{
    return 10.0f; //TODO use slider
};
