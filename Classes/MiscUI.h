#pragma once
#ifndef MISCUI_H
#define MISCUI_H

#include "cocos2d.h"

namespace cocos2d
{
    namespace ui
    {
        class Button;
    }
}

class FloatingLabel : public cocos2d::Label
{
public:

    FloatingLabel(cocos2d::TextHAlignment hAlignment = cocos2d::TextHAlignment::LEFT,
                  cocos2d::TextVAlignment vAlignment = cocos2d::TextVAlignment::TOP);

    static FloatingLabel* createWithTTF(const std::string& text, const std::string& fontFilePath, float fontSize,
        const cocos2d::Size& dimensions = cocos2d::Size::ZERO, cocos2d::TextHAlignment hAlignment = cocos2d::TextHAlignment::LEFT,
        cocos2d::TextVAlignment vAlignment = cocos2d::TextVAlignment::TOP);

    static FloatingLabel* createWithTTF(const cocos2d::TTFConfig& ttfConfig, const std::string& text,
        cocos2d::TextHAlignment hAlignment = cocos2d::TextHAlignment::LEFT, int maxLineWidth = 0);

    void do_float();
};

void load_default_button_textures(cocos2d::ui::Button* button);

#endif
