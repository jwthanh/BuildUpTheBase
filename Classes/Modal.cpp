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

bool BaseModal::init()
{
    //background color
    LayerColor* layer_color = cocos2d::LayerColor::create({30, 144, 255, 85});
    this->addChild(layer_color);

    cocos2d::Node* root_message_node = get_prebuilt_node_from_csb("editor/details/message_detail.csb");
    this->_node = root_message_node->getChildByName("message_panel");
    this->_node->removeFromParent();
    this->_node->setPosition(cocos2d::Vec2{960, 640}/ 2);
    this->addChild(this->_node);
    this->_close_panel = dynamic_cast<ui::Layout*>(this->_node->getChildByName("close_panel"));

    //title
    this->_title_lbl = dynamic_cast<cocos2d::ui::Text*>(this->_node->getChildByName("title_lbl"));
    this->set_title("Unnamed Modal");
    set_aliasing(this->_title_lbl, true);

    //invisible layout to tap to close
    this->_exit_layout = ui::Layout::create();
    this->_exit_layout->setAnchorPoint(Vec2::ZERO);
    this->_exit_layout->setContentSize({ 1000, 1000 });
    this->_exit_layout->setTouchEnabled(true);
    this->addChild(this->_exit_layout, -1); //add it behind everything else in this.

    this->init_callbacks();

    int modal_zindex = 9999;
    this->setLocalZOrder(modal_zindex);

    return true;
};

void BaseModal::init_callbacks()
{
    this->on_layout_touched = [this]() {
       do_vibrate(5);
       this->removeFromParent();
    };

    auto touch_handler = [this]()
    {
        this->on_layout_touched();
    };

    bind_touch_ended(this->_close_panel, touch_handler);
    bind_touch_ended(this->_exit_layout, touch_handler);
};

void BaseModal::set_title(const std::string& title)
{
    this->_title_lbl->setString(title);
};

bool TextBlobModal::init()
{
    BaseModal::init();

    //scrollable body
    this->_body_scroll = dynamic_cast<ui::ListView*>(this->_node->getChildByName("body_scroll"));
    this->_body_scroll->setScrollBarEnabled(true);
    this->_body_scroll->scrollToTop(0.0f, false);
    this->_body_scroll->setUnifySizeEnabled(true);

    //fill message up
    this->set_body("No body yet");

    return true;
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
