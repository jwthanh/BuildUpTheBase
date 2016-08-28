#include "Modal.h"

#include "cocos2d.h"
#include "cocostudio/ActionTimeline/CSLoader.h"

#include "GameLayer.h"
#include "MiscUI.h"

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
    ui::ScrollView* body_scroll = dynamic_cast<ui::ScrollView*>(this->_node->getChildByName("body_scroll"));
    body_scroll->setScrollBarAutoHideEnabled(false);
    body_scroll->setScrollBarEnabled(true);
    body_scroll->scrollToTop(0.0f, false);

    //fill message up
    this->_body_lbl = dynamic_cast<cocos2d::ui::Text*>(body_scroll->getChildByName("body_lbl"));
    set_aliasing(this->_body_lbl);
    this->set_body("No body yet");

};

void TextBlobModal::set_body(const std::string& body)
{
    this->_body_lbl->setString(body);
};

