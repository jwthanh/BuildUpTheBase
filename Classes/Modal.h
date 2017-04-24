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

        cocos2d::ui::Text* _title_lbl;

    public:
        //TODO figure out how to clean this up properly, maybe subclass Ref*
        CREATE_FUNC(BaseModal);
        bool init();
        virtual void init_callbacks();

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

//only one at a time, used for flavour text along the bottom
class PopupPanel
{
    private:
        cocos2d::ui::Layout* _layout;

        //initial position to animate to, taken from cocos studio
        float initial_x, initial_y;

    public:
        PopupPanel(cocos2d::ui::Layout* panel);
        virtual void init_layout_touch_handler();
        std::function<void()> on_layout_touched;

        void animate_open();
        void animate_close();

        std::string get_string();
        void set_string(std::string message);
        void set_image(std::string path, bool is_plist = false);

        void set_visible(bool is_visible);

        void play_particle(cocos2d::ParticleSystemQuad* parts);
};

#endif
