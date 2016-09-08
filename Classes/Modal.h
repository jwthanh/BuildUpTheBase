#pragma once
#ifndef MODAL_H
#define MODAL_H
#include <string>


namespace cocos2d 
{
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
class BaseModal 
{
    protected:
        //base node for all children
        cocos2d::Node* _node;

        cocos2d::ui::Text* _title_lbl;

    public:
        //TODO figure out how to clean this up properly, maybe subclass Ref*
        BaseModal(cocos2d::Node* parent);

        void set_title(const std::string& title);
};

//modal with scrollable text in its body
class TextBlobModal : public BaseModal
{
    private:
        //TODO figure how to resize dynamically
        cocos2d::ui::ListView* _body_scroll;

    public:
        TextBlobModal(cocos2d::Node* parent);

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

        void animate_open();
        void animate_close();

        void set_string(std::string message);
};

#endif
