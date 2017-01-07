#include "ActionPanel.h"

#include <sstream>

#include "2d/CCMenuItem.h"
#include "2d/CCActionInterval.h"
#include "2d/CCActionEase.h"

#include "constants.h"
#include "MiscUI.h"
#include "NodeBuilder.h"

bool ActionPanel::init()
{
    auto node = get_prebuilt_node_from_csb("panels/animation_panel.csb");
    this->_panel = dynamic_cast<cocos2d::ui::Layout*>(node->getChildByName("animation_panel"));
    this->_panel->removeFromParent();
    this->_panel->setPosition({ 660, 320 });

    auto reset_btn = dynamic_cast<cocos2d::ui::Button*>(this->_panel->getChildByName("buttons_panel")->getChildByName("reset_btn"));
    reset_btn->addTouchEventListener([this](TOUCH_CALLBACK_PARAMS){
        this->reset_target();
    });

    this->addChild(this->_panel);

    this->_target = NULL;
    this->_default_pos_x = 0.0f;
    this->_default_pos_y = 0.0f;
    this->_default_scale_x = 0.0f;
    this->_default_scale_y = 0.0f;

    return true;
};

void ActionPanel::set_target(cocos2d::Node* target)
{
    this->_target = target;

    this->_default_pos_x = target->getPositionX();
    this->_default_pos_y = target->getPositionY();
    this->_default_scale_x = target->getScaleX();
    this->_default_scale_y = target->getScaleY();

    this->init_action_buttons();
};

void ActionPanel::init_action_buttons()
{
    if (this->_target == NULL)
    {
        CCASSERT(false, "needs a target");
    }

    //sets the current action for the panel, and rebuilds the modifiers to use the new current action
    auto create_action_button = [this](std::string btn_name, std::function<cocos2d::ActionInterval*()> build_action)
    {
        //customize button for the panel
        auto menu_btn = dynamic_cast<cocos2d::ui::Button*>(this->_panel->getChildByName(btn_name));
        prep_button(menu_btn);

        menu_btn->addTouchEventListener([build_action, this](TOUCH_CALLBACK_PARAMS)
        {
            if (evt == TouchEventType::ENDED)
            {
                this->current_action = build_action;
                this->init_modifier_buttons();
            }
        });
    };

    auto move_by = [this](){
        return cocos2d::MoveBy::create(this->get_duration(), this->get_target_pos());
    };
    create_action_button("move_by_btn", move_by);

    auto move_to = [this](){
        return cocos2d::MoveTo::create(this->get_duration(), this->get_target_pos());
    };
    create_action_button("move_to_btn", move_to);

    auto scale_by = [this](){
        return cocos2d::ScaleBy::create(this->get_duration(), this->get_target_pos_x(), this->get_target_pos_y());
    };
    create_action_button("scale_by_btn", scale_by);

    auto scale_to = [this](){
        return cocos2d::ScaleTo::create(this->get_duration(), this->get_target_pos_x(), this->get_target_pos_y());
    };
    create_action_button("scale_to_btn", scale_to);

};

void ActionPanel::init_modifier_buttons()
{
    if (this->_target == NULL)
    {
        CCASSERT(false, "needs a target");
    }

    auto listview = dynamic_cast<cocos2d::ui::ListView*>(this->_panel->getChildByName("actions_listview"));
    listview->removeAllChildren();

    auto create_button = [listview, this](std::string text, std::function<cocos2d::ActionInterval*(cocos2d::ActionInterval*)> build_modifier)
    {
        auto menu_btn = cocos2d::ui::Button::create();
        prep_button(menu_btn);

        //customize button for the panel
        menu_btn->setScale9Enabled(true);
        menu_btn->setContentSize({ 150.0f, 20.0f });
        menu_btn->setTitleText(text);
        listview->addChild(menu_btn);

        //on touching ending on the menu button, call the build_action and run its Action on the target
        menu_btn->addTouchEventListener([build_modifier, this, text](TOUCH_CALLBACK_PARAMS)
        {
            if (evt == TouchEventType::ENDED)
            {
                this->reset_target();
                auto action = this->current_action();
                action = build_modifier(action);
                this->output_parameters(text);
                this->_target->runAction(action);
            }
        });
    };


    //vanilla ease
    auto ease_in = [this](cocos2d::ActionInterval* action){ return cocos2d::EaseIn::create(action, this->get_rate()); };
    create_button("ease in", ease_in);
    auto ease_out = [this](cocos2d::ActionInterval* action){ return cocos2d::EaseOut::create(action, this->get_rate()); };
    create_button("ease out", ease_out);
    auto ease_in_out = [this](cocos2d::ActionInterval* action){ return cocos2d::EaseInOut::create(action, this->get_rate()); };
    create_button("ease in out", ease_in_out);

    //ease back
    auto ease_back_in = [this](cocos2d::ActionInterval* action){ return cocos2d::EaseBackIn::create(action); };
    create_button("ease back in", ease_back_in);
    auto ease_back_out = [this](cocos2d::ActionInterval* action){ return cocos2d::EaseBackOut::create(action); };
    create_button("ease back out", ease_back_out);
    auto ease_back_in_out = [this](cocos2d::ActionInterval* action){ return cocos2d::EaseBackInOut::create(action); };
    create_button("ease back in out", ease_back_in_out);

    //ease bounce
    auto ease_bounce_in = [this](cocos2d::ActionInterval* action){ return cocos2d::EaseBounceIn::create(action); };
    create_button("ease bounce in", ease_bounce_in);
    auto ease_bounce_out = [this](cocos2d::ActionInterval* action){ return cocos2d::EaseBounceOut::create(action); };
    create_button("ease bounce out", ease_bounce_out);
    auto ease_bounce_in_out = [this](cocos2d::ActionInterval* action){ return cocos2d::EaseBounceInOut::create(action); };
    create_button("ease bounce in out", ease_bounce_in_out);
}

void ActionPanel::reset_target()
{
    this->_target->stopAllActions();
    this->_target->setPosition({this->_default_pos_x, this->_default_pos_y});
    this->_target->setScale(this->_default_scale_x, this->_default_scale_y);
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
