#pragma once
#ifndef FIST_H
#define FIST_H

#include "cocos2d.h"
#include "constants.h"

class FistSprite;
class Beatup;
class Clock;
class BaseMenu;

class Shield;
class Fist;

class Coin;
class Gore;

class PunchLog;
struct PunchLogItem;

class Combo;

class Fist
{
    private:

    public:

        FistHands hand;
        cocos2d::ParticleSystemQuad* spark_part;

        void block();
        bool is_punching;
        FistSprite* sprite;
        FistSprite* moving_sprite;
        FistSprite* hit_sprite;

        Beatup* beatup;

        float x, y;


        bool is_left;

        Fist(FistSprite* sprite, bool is_left);

        Fist* other;

        void punch(bool is_charged_punch = false);
        cocos2d::Vec2 get_punch_pos();

        void set_weapon_active(FistWeaponTypes weapon_type, bool is_active);
        void toggle_weapon_active(FistWeaponTypes weapon_type);
        void one_weapon_active(FistWeaponTypes weapon_type);
        void speed_flames(float scale);

        int adj(int val);
        float adj(float val);
        double adj(double val);

        int neg(int val);
        float neg(float val);
        double neg(double val);

        void update(float dt);

        bool is_shaky_running;
        bool has_played_charge_sound;

        Clock* punch_clock;
        Clock* block_shake_clock;
        void reset_pos();

        float punch_dmg;
        float get_punch_dmg();

        Clock* charging_clock;

        struct Defaults {
            float x, y;
            float punch_dmg;
            float scale;
            float charge_threshold;
        } defaults;
};

class FistSprite : public cocos2d::Sprite
{
    public:
        Fist* fist;

        cocos2d::Vec2 flames_pos;

        cocos2d::ParticleSystemQuad* get_weapon_particlesys(FistWeaponTypes weapon_type);
        cocos2d::ParticleSystemQuad* flames_part;
        cocos2d::ParticleSystemQuad* psionic_part;
        cocos2d::ParticleSystemQuad* frost_part;

        void prep(Fist* fist, cocos2d::Vec2 flames_pos);

        //if I dont override and cast a Sprite to a FistSprite, memory chokes up
        static FistSprite* createWithSpriteFrame(cocos2d::SpriteFrame *spriteFrame);
        static FistSprite* createWithSpriteFrameName(const std::string& spriteFrameName);
};

class PunchLog
{
    public:
        PunchLog();
        std::deque<PunchLogItem*>* log_items;

        unsigned long long punch_count;

        void add_punch(FistHands hand);

        bool check_one(FistHands hand, int index, std::deque<PunchLogItem*> log);
        bool check_combo(Combo* combo);
};

struct PunchLogItem
{
    public:
        FistHands hand;

        PunchLogItem(FistHands hand);
};


#endif
