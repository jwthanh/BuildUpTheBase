#include "Modal.h"

#include "cocos2d.h"
#include "cocostudio/ActionTimeline/CSLoader.h"

#include "GameLayer.h"
#include "MiscUI.h"
#include "Util.h"

USING_NS_CC;

BaseModal::BaseModal(Node* parent)
{
    auto inst = cocos2d::CSLoader::getInstance();

    cocos2d::Node* root_message_node = inst->CSLoader::createNode("editor/details/message_detail.csb");
    this->_node = root_message_node->getChildByName("message_panel");
    this->_node->removeFromParent();
    this->_node->setPosition(GameLayer::get_center_pos());

    //title
    this->_title_lbl = dynamic_cast<cocos2d::ui::Text*>(this->_node->getChildByName("title_lbl"));
    this->set_title("TEST");
    set_aliasing(this->_title_lbl, true);


    ui::Layout* close_panel = dynamic_cast<ui::Layout*>(this->_node->getChildByName("close_panel"));
    auto cb = [this](Ref* target, ui::Widget::TouchEventType type) {
        if (type == ui::Widget::TouchEventType::ENDED)
        {
            this->_node->removeFromParent();
        };
    };
    close_panel->addTouchEventListener(cb);

    parent->addChild(this->_node);
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
    for (auto s : split_messages)
    {
        auto line = ui::Text::create(s, "", 24.0f);
        Label* renderer = (Label*)line->getVirtualRenderer();
        renderer->setTTFConfig(ttf_config);
        set_aliasing(renderer);

        this->_body_scroll->addChild(line);
    };
};
