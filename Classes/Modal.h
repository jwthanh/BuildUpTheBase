#pragma once
#ifndef MODAL_H
#define MODAL_H
#include <string>


namespace cocos2d 
{
    class Node;

    namespace ui 
    {
        class Text;
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
        BaseModal(cocos2d::Node* parent);

        void set_title(const std::string& title);
};

//modal with scrollable text in its body
class TextBlobModal : public BaseModal
{
    private:
        cocos2d::ui::Text* _body_lbl;

    public:
        TextBlobModal(cocos2d::Node* parent);

        void set_body(const std::string& body_txt);
};

//modal with paginated items
class PageModal : public BaseModal
{

};

#endif
