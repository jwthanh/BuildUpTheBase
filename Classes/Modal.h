#pragma once
#ifndef MODAL_H
#define MODAL_H

#include <string>
#include <functional>

#include "2d/CCNode.h"

using VoidFuncNoArgs = std::function<void()>;

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
        class Button;
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

        virtual cocos2d::Node* _build_root_node();

    public:
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
        bool init() override;

        void set_body(const std::string& body_txt);
};

class ActionTextBlobModal : public TextBlobModal
{
    private:
        cocos2d::ui::Button* _confirm_btn;
        cocos2d::ui::Button* _abort_btn;
        cocos2d::Node* _build_root_node() override;

    public:
        CREATE_FUNC(ActionTextBlobModal);
        bool init() override;

        void set_confirm_button_text(std::string confirm_text);
        void set_confirm_callback(VoidFuncNoArgs confirm_callback);

        void set_abort_button_text(std::string abort_text);
        void set_abort_callback(VoidFuncNoArgs abort_callback);
};

#endif
