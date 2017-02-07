#include "Modal.h"


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


USING_NS_CC;

BaseModal::BaseModal(Node* parent)
{
    //background color
    LayerColor* layer_color = cocos2d::LayerColor::create({30, 144, 255, 85});
    parent->addChild(layer_color);

    cocos2d::Node* root_message_node = get_prebuilt_node_from_csb("editor/details/message_detail.csb");
    this->_node = root_message_node->getChildByName("message_panel");
    this->_node->removeFromParent();
    this->_node->setPosition(cocos2d::Vec2{960, 640}/ 2);

    //title
    this->_title_lbl = dynamic_cast<cocos2d::ui::Text*>(this->_node->getChildByName("title_lbl"));
    this->set_title("TEST");
    set_aliasing(this->_title_lbl, true);

    //invisible layout to tap to close
    ui::Layout* exit_layout = ui::Layout::create();
    exit_layout->setAnchorPoint(Vec2::ZERO);
    exit_layout->setContentSize({ 1000, 1000 });
    exit_layout->setTouchEnabled(true);

    ui::Layout* close_panel = dynamic_cast<ui::Layout*>(this->_node->getChildByName("close_panel"));
    auto close_modal = [this, layer_color, exit_layout]() {
            do_vibrate(5);
            this->_node->removeFromParent();
            layer_color->removeFromParent();
            exit_layout->removeFromParent();
    };

    bind_touch_ended(close_panel, close_modal);
    bind_touch_ended(exit_layout, close_modal);

    int modal_zindex = 9999;
    parent->addChild(exit_layout, modal_zindex-1);
    parent->addChild(this->_node, modal_zindex);
};

void BaseModal::set_title(const std::string& title)
{
    this->_title_lbl->setString(title);
};

TextBlobModal::TextBlobModal(cocos2d::Node* parent) : BaseModal(parent)
{

    //scrollable body
    this->_body_scroll = dynamic_cast<ui::ListView*>(this->_node->getChildByName("body_scroll"));
    this->_body_scroll->setScrollBarEnabled(true);
    this->_body_scroll->scrollToTop(0.0f, false);
    this->_body_scroll->setUnifySizeEnabled(true);

    //fill message up
    this->set_body("No body yet");
};

void TextBlobModal::set_body(const std::string& body)
{
    std::string raw_log_string = body;
    cocos2d::TTFConfig ttf_config = TTFConfig("pixelmix.ttf", 24, GlyphCollection::ASCII, NULL, false, 2);

    this->_body_scroll->removeAllChildren();

    auto split_messages = split(raw_log_string, '\n'); //TODO fix stripping deliberate whitespace
    std::stringstream joined_ss;

    this->_body_scroll->setItemsMargin(10.0f);

    for (auto s : split_messages)
    {
        auto line = ui::Text::create(s, "", 24.0f);
        line->setTextAreaSize({700.0, 0});

        Label* renderer = (Label*)line->getVirtualRenderer();
        renderer->setTTFConfig(ttf_config);
        set_aliasing(renderer);

        this->_body_scroll->addChild(line);
    };
};


PopupPanel::PopupPanel(cocos2d::ui::Layout* panel) :
    _layout(panel)
{
    this->initial_x = panel->getPositionX();
    this->initial_y = panel->getPositionY();

    auto label = dynamic_cast<cocos2d::ui::Text*>(this->_layout->getChildByName("popup_panel_inner")->getChildByName("popup_lbl"));
    set_aliasing(label);

    this->_layout->addTouchEventListener([this](cocos2d::Ref* target, cocos2d::ui::Widget::TouchEventType evt)
    {
        if (evt == ui::Widget::TouchEventType::ENDED)
        {
            do_vibrate(5);
            this->animate_close();
        }
    });

    this->set_image("exclamation.png", true);
};

void PopupPanel::animate_open()
{
    this->set_visible(true); //because it starts off visible false, should be fixed some other way though
    this->_layout->stopAllActions(); //prevents previous animate_closes from affecting this one

    this->set_image("exclamation.png", true); //reset image

    LOG(INFO) << "[Popup] " << this->get_string();

    this->_layout->setPosition(Vec2(
        this->initial_x,
        -100.0f
    ));

    float duration = 0.3f;
    cocos2d::ActionInterval* show_action = cocos2d::Spawn::createWithTwoActions(
        cocos2d::FadeIn::create(duration/2.0f),
        cocos2d::EaseBackOut::create(
            cocos2d::MoveTo::create(
                duration,
                Vec2(this->initial_x, this->initial_y)
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

    this->_layout->setPosition(Vec2(
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
                Vec2(this->initial_x, -100)
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
        image_view->loadTexture(path, ui::Widget::TextureResType::PLIST);
    }
    else
    {
        image_view->loadTexture(path, ui::Widget::TextureResType::LOCAL);
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
