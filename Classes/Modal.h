#pragma once
#ifndef MODAL_H
#define MODAL_H

#include <string>
#include <functional>

#include "2d/CCNode.h"

namespace cocos2d
{
    class ParticleSystemQuad;
    class Node;

    namespace ui
    {
        class Layout;
        class Text;
        class ScrollView;
        class ListView;
    };
};

//a box with a title and a close button
class BaseModal : public cocos2d::Node
{
    protected:
        //base node for all children
        cocos2d::Node* _node;
        cocos2d::ui::Layout* _exit_layout;
        cocos2d::ui::Layout* _close_panel;

        cocos2d::ui::Text* _title_lbl;

    public:
        //TODO figure out how to clean this up properly, maybe subclass Ref*
        CREATE_FUNC(BaseModal);
        bool init();
        virtual void init_callbacks();
        std::function<void()> on_layout_touched;

        void set_title(const std::string& title);
};

//modal with scrollable text in its body
class TextBlobModal : public BaseModal
{
    private:
        //TODO figure how to resize dynamically
        cocos2d::ui::ListView* _body_scroll;

    public:
        CREATE_FUNC(TextBlobModal);
        virtual bool init();

        void set_body(const std::string& body_txt);
};

//modal with paginated items TODO
class PageModal : public BaseModal
{
};

#endif
