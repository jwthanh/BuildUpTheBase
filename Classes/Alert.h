#pragma once
#ifndef ALERT_H
#define ALERT_H

#include "cocos2d.h"
#include "ui/CocosGUI.h"

class Alert : public cocos2d::ui::Layout
{
    public:
        static Alert* create();
        static Alert* create(std::string header_msg, std::string sub_header_msg, std::string body_msg);

        bool onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event);
        void onTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event);

        void shrink_close(cocos2d::Vec2 pos);
        void grow_open(cocos2d::Vec2 pos);

        cocos2d::ui::Button* close_btn;
        cocos2d::ui::Button* done_btn;

        std::string header_str;
        std::string sub_header_str;
        std::string body_str;

        cocos2d::ui::Text* header_txt;
        cocos2d::ui::Text* sub_header_txt;
        cocos2d::ui::Text* body_txt;

        void set_header_str(std::string msg);
        void set_sub_header_str(std::string msg);
        void set_body_str(std::string msg);


};

#endif
