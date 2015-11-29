#pragma once
#ifndef PROGRESSBAR_H
#define PROGRESSBAR_H

#include "cocos2d.h"

class Beatup;

class ProgressBar
{
    public:
        Beatup* beatup;

        enum Layer {
            Front,
            Back,
            Both
        };

        cocos2d::Node* base_node;
        cocos2d::LayerColor* color_layer;

        cocos2d::ProgressTimer* front_timer;
        cocos2d::ProgressTimer* back_timer;
        cocos2d::Sprite* background;

        cocos2d::Label* lbl;
        void update_string(std::string msg);

        bool wait_to_clear;

        float target_percentage;

        cocos2d::RenderTexture* rt;

        ProgressBar(Beatup* beatup, std::string front_sprite_frame_path="", std::string back_sprite_frame_path="");

        void fit_back_to_front();
        ~ProgressBar();

        void setAnchorPoint(cocos2d::Vec2 vec2);
        void setPosition(cocos2d::Vec2 vec2);
        void setVisible(bool val);

        void bump(float duration, float new_scale, ProgressBar::Layer layer = ProgressBar::Layer::Both);
        cocos2d::Sequence* get_bump_seq(float duration, float new_scale);
        cocos2d::Sequence* get_back_bump_seq(float duration, float new_scale);

        bool do_finish_bump;
        float scale;
        void setScale(float scale);
        cocos2d::Vec2 get_back_scale();

        void set_percentage(float percentage); //move both to percentage
        void scroll_to_percentage(float percentage); //scroll front then cut back to percentage

};

#endif
