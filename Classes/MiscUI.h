#pragma once
#ifndef MISCUI_H
#define MISCUI_H

#include "2d/CCLabel.h"

namespace cocos2d {
    class Sprite;
    class Texture2D;
    class Label;
    struct Color4B;
    class Node;
    struct Color3B;
}

namespace cocos2d
{
    namespace ui
    {
        class Scale9Sprite;
        class ImageView;
        class Text;
        class TextField;
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

    void do_float(float x=100.0f, float x_variation=1.0f, float y=100.0f, float y_variation=10.0f, float duration=1.5f);
};

void load_default_button_textures(cocos2d::ui::Button* button);

void animate_flash_action(
    cocos2d::Node* target, float duration, float scale,
    cocos2d::Color3B to_color = cocos2d::Color3B::RED,
    cocos2d::Color3B end_color = cocos2d::Color3B::WHITE
    );

void try_set_text_color(cocos2d::ui::Text* text_node, const cocos2d::Color4B color);
void try_set_node_color(cocos2d::Node* node, const cocos2d::Color3B color);

void set_aliasing(cocos2d::Label* label, bool set_aliasing=true);
void set_aliasing(cocos2d::Texture2D* texture, bool val=true);
void set_aliasing(cocos2d::Sprite* sprite, bool val=true);
void set_aliasing(cocos2d::ui::Text* text, bool val=true);
void set_aliasing(cocos2d::ui::TextField* text, bool val=true);
void set_aliasing(cocos2d::ui::ImageView* img_view, bool val=true);
void set_aliasing(cocos2d::ui::Button* button, bool val=true);
void set_aliasing(cocos2d::ui::Scale9Sprite* scale9, bool val=true);

//inits texture and outline and aliasing
void prep_button(cocos2d::ui::Button* button);
// this one binds a popScene along with the above
void prep_back_button(cocos2d::ui::Button* back_button);

#endif
