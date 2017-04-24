#include "PopupPanel.h"

#include "MiscUI.h"
#include "Util.h"
#include "external/easylogging.h"
#include "utilities/vibration.h"

#include "NodeBuilder.h"

#include "2d/CCActionInterval.h"
#include "2d/CCActionInstant.h"
#include "ui/UIText.h"
#include "ui/UILayout.h"
#include "ui/UIImageView.h"
#include "ui/UIListView.h"
#include "2d/CCLayer.h"
#include "2d/CCActionEase.h"
#include "2d/CCParticleSystemQuad.h"



PopupPanel::PopupPanel(cocos2d::ui::Layout* panel) :
    _layout(panel)
{
    this->initial_x = panel->getPositionX();
    this->initial_y = panel->getPositionY();

    auto label = dynamic_cast<cocos2d::ui::Text*>(this->_layout->getChildByName("popup_panel_inner")->getChildByName("popup_lbl"));
    set_aliasing(label);

    //default touch handler
    this->on_layout_touched = [this](){
        this->animate_close();
    };
    this->init_layout_touch_handler();

    this->set_image("exclamation.png", true);
};

void PopupPanel::init_layout_touch_handler()
{

    this->_layout->addTouchEventListener([this](cocos2d::Ref* target, cocos2d::ui::Widget::TouchEventType evt)
    {
        if (evt == cocos2d::ui::Widget::TouchEventType::ENDED)
        {
            do_vibrate(5);
            this->on_layout_touched();
        }
    });
};


void PopupPanel::animate_open()
{
    this->set_visible(true); //because it starts off visible false, should be fixed some other way though
    this->_layout->stopAllActions(); //prevents previous animate_closes from affecting this one

    this->set_image("exclamation.png", true); //reset image

    LOG(INFO) << "[Popup] " << this->get_string();

    this->_layout->setPosition(cocos2d::Vec2(
        this->initial_x,
        -100.0f
    ));

    float duration = 0.3f;
    cocos2d::ActionInterval* show_action = cocos2d::Spawn::createWithTwoActions(
        cocos2d::FadeIn::create(duration/2.0f),
        cocos2d::EaseBackOut::create(
            cocos2d::MoveTo::create(
                duration,
                cocos2d::Vec2(this->initial_x, this->initial_y)
            )
        )
    );

    auto delay = cocos2d::DelayTime::create(10.0f);
    auto remove = cocos2d::CallFunc::create([this]() { this->animate_close(); });

    this->_layout->runAction(cocos2d::Sequence::create(
        show_action, delay, remove, NULL
    ));
};

void PopupPanel::animate_close()
{
    this->set_visible(true); //because it starts off visible false, should be fixed some other way though

    float duration = 0.3f;

    this->_layout->setPosition(cocos2d::Vec2(
        this->initial_x,
        this->initial_y
    ));

    cocos2d::CallFunc* disable_touch = cocos2d::CallFunc::create([this]()
    {
        this->_layout->setTouchEnabled(false);
    });
    cocos2d::CallFunc* enable_touch = cocos2d::CallFunc::create([this]()
    {
        this->_layout->setTouchEnabled(true);
    });

    cocos2d::ActionInterval* hide_action = cocos2d::Spawn::createWithTwoActions(
        cocos2d::FadeOut::create(duration),
        cocos2d::EaseBackIn::create(
            cocos2d::MoveTo::create(
                duration,
                cocos2d::Vec2(this->initial_x, -100)
            )
        )
    );

    this->_layout->runAction(cocos2d::Sequence::create(
        disable_touch, hide_action, enable_touch, NULL
    ));
};

std::string PopupPanel::get_string()
{
    auto label = dynamic_cast<cocos2d::ui::Text*>(this->_layout->getChildByName("popup_panel_inner")->getChildByName("popup_lbl"));
    return label->getString();
}

void PopupPanel::set_string(std::string message)
{
    auto label = dynamic_cast<cocos2d::ui::Text*>(this->_layout->getChildByName("popup_panel_inner")->getChildByName("popup_lbl"));
    label->setString(message);
}

void PopupPanel::set_image(std::string path, bool is_plist)
{
    auto image_view = dynamic_cast<cocos2d::ui::ImageView*>(this->_layout->getChildByName("popup_panel_inner")->getChildByName("popup_img"));
    if (is_plist)
    {
        image_view->loadTexture(path, cocos2d::ui::Widget::TextureResType::PLIST);
    }
    else
    {
        image_view->loadTexture(path, cocos2d::ui::Widget::TextureResType::LOCAL);
    };
    set_aliasing(image_view); //needs to get reapplied after you load a new texture
}

void PopupPanel::set_visible(bool is_visible)
{
    this->_layout->setVisible(is_visible);
};

void PopupPanel::play_particle(cocos2d::ParticleSystemQuad* parts)
{
    this->_layout->addChild(parts, -1);
};
