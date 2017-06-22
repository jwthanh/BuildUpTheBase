#pragma once
#ifndef MISCUI_H
#define MISCUI_H

namespace cocos2d {
    class Vec2;
    class Sprite;
    class Texture2D;
    class Label;
    struct Color4B;
    class Node;
    struct Color3B;
}

namespace cocos2d
{
    class Sequence;
    struct Color3B;
    namespace ui
    {
        class TextBMFont;
        class Scale9Sprite;
        class ImageView;
        class Text;
        class TextField;
        class Button;
    }
}

cocos2d::Label* do_float(float x = 100.0f, float x_variation = 1.0f, float y = 100.0f, float y_variation = 10.0f, float duration = 1.5f);
void load_default_button_textures(cocos2d::ui::Button* button);

//builds a sequence that scales a node up and then back down, highlighting it to_color and returning to end_color
cocos2d::Sequence* build_flash_action(
    float duration,
    float scale, float original_scale,
    cocos2d::Color3B to_color,
    cocos2d::Color3B end_color
);
//runs the flash sequence
void run_flash_action(
    cocos2d::Node* target, float duration,
    float scale, float original_scale,
    cocos2d::Color3B to_color,
    cocos2d::Color3B end_color
);

void animate_modal_open(
    cocos2d::Node* target,
    float duration,
    float end_scale,
    cocos2d::Vec2 start_pos,
    cocos2d::Vec2 end_pos
);

void try_set_text_color(cocos2d::ui::Text* text_node, const cocos2d::Color4B color);
void try_set_node_color(cocos2d::Node* node, const cocos2d::Color3B color);

void set_aliasing(cocos2d::ui::TextBMFont* label, bool set_aliasing=true);
void set_aliasing(cocos2d::Label* label, bool set_aliasing=true);
void set_aliasing(cocos2d::Texture2D* texture, bool val=true);
void set_aliasing(cocos2d::Sprite* sprite, bool val=true);
void set_aliasing(cocos2d::ui::Text* text, bool val=true);
void set_aliasing(cocos2d::ui::TextField* text, bool val=true);
void set_aliasing(cocos2d::ui::ImageView* img_view, bool val=true);
void set_aliasing(cocos2d::ui::Button* button, bool val=true);
void set_aliasing(cocos2d::ui::Scale9Sprite* scale9, bool val=true);

//FIXME (by upgrading cocos probably) this resets ImageView somehow so loadTexture doesn't crash
// so we toggle it once to change it, then toggle it again to reset to default
void image_view_scale9_hack(cocos2d::ui::ImageView* img_view);

//inits texture and outline and aliasing
void prep_button(cocos2d::ui::Button* button);
// this one binds a popScene along with the above
void prep_back_button(cocos2d::ui::Button* back_button);

#endif