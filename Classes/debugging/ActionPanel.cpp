#include "ActionPanel.h"

#include <typeindex>
#include <typeinfo>

#include "2d/CCMenuItem.h"

#include "NodeBuilder.h"
#include "MiscUI.h"
#include "2d/CCActionInterval.h"
#include "GameLogic.h"
#include <sstream>

bool ActionPanel::init()
{
    auto node = get_prebuilt_node_from_csb("panels/animation_panel.csb");
    this->_panel = dynamic_cast<cocos2d::ui::Layout*>(node->getChildByName("animation_panel"));
    this->_panel->removeFromParent();
    this->_panel->setPosition({ 460, 320 });

    auto reset_btn = dynamic_cast<cocos2d::ui::Button*>(this->_panel->getChildByName("buttons_panel")->getChildByName("reset_btn"));
    reset_btn->addTouchEventListener([this](TOUCH_CALLBACK_PARAMS){
        this->reset_target();
    });

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

    auto create_button = [listview, this](std::string text, std::function<cocos2d::Action*()> build_action)
    {
        auto menu_btn = cocos2d::ui::Button::create();
        prep_button(menu_btn);
        menu_btn->setScale9Enabled(true);
        menu_btn->setContentSize({ 150.0f, 20.0f });
        menu_btn->setTitleText(text);
        listview->addChild(menu_btn);

        menu_btn->addTouchEventListener([build_action, this, text](TOUCH_CALLBACK_PARAMS)
        {
            if (evt == TouchEventType::ENDED)
            {
                this->reset_target();
                auto action = build_action();
                this->output_parameters(text);
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
    auto slider = dynamic_cast<cocos2d::ui::Slider*>(this->_panel->getChildByName("buttons_panel")->getChildByName("duration_slider"));
    float max = 2.0f;
    return (float)slider->getPercent() / 100.0f * max;
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
    auto slider = dynamic_cast<cocos2d::ui::Slider*>(this->_panel->getChildByName("buttons_panel")->getChildByName("rate_slider"));
    float max = 100.0f;
    return (float)slider->getPercent() / 100.0f * max;
};

void ActionPanel::output_parameters(std::string func_name)
{
    std::stringstream ss;
    ss << func_name << std::endl;
    ss << "  target x: " << this->get_target_pos_x() << std::endl;
    ss << "  target y: " << this->get_target_pos_y() << std::endl;
    ss << "  duration: " << this->get_duration() << std::endl;
    ss << "  rate: " << this->get_rate() << std::endl;

    CCLOG("%s", ss.str().c_str());
};
