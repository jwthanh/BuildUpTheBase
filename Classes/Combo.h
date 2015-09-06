#pragma once
#ifndef COMBO_H
#define COMBO_H

#include "cocos2d.h"
#include "Buyable.h"

class ProgressBar;

class Beatup;
class Fist;

#include "Fist.h"

class Combo : public Buyable
{
    public:
        Combo(Beatup* beatup, std::string id_key, std::string name);

        Beatup* beatup;


        std::string name;

        bool is_combo_met;
        int last_matched_punch_idx;
        std::vector<FistHands>* hand_order;

        float stamina_regen;

        unsigned int activate_count;
        bool handle_hand(FistHands hand);
        void activate();

        ProgressBar* cd_bar;

        bool can_activate;
        bool can_punch();

        float cooldown;
        Clock* cooldown_clock;
        struct Defaults {
            float cooldown;
        } defaults;

        float shake_time; //how much the face'll shake

        void update(float dt);

        void toggle_order();
        void color_combo_chars();
        void hide_order();
        cocos2d::Label* order_lbl;
        bool order_is_shown;

        static std::string order_to_string(std::vector<FistHands>* hand_order);

};

#endif
