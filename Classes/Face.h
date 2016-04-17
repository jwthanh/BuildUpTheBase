#pragma once
#ifndef FACE_H
#define FACE_H

#include "cocos2d.h"
#include "constants.h"

class Beatup;
class Fist;
class Shield;
class ProgressBar;
class Clock;
class FacialFeature;
class ShatterSprite;
class Level;

class Hittable
{
    public:
        int hits_taken;
        int hits_to_kill;
        float get_hits_percent();
};

class Face : public cocos2d::Node, public Hittable
{
    private:
        const std::string default_sound_path = "sounds\\old\\dsnoway.mp3";

    public:

        struct Defaults {
            float charge_dmg;
            float last_attack_threshold;
        } defaults;

        bool removed;
        void remove();
        void kill();

        cocos2d::ParticleSystemQuad* charging_part;

        bool long_enough_since_last_attack();
        Clock* last_attack_clock;
        float charge_dmg;

        bool is_fully_charged();
        Clock* charge_clock;

        bool get_percent_charged(float percent);
        void start_charging_attack();
        void do_charge_attack();
        void end_charge_attack();

        enum FeatureType {
            Glasses,
            Hat,
            Moustache,
            Beard,
            Monocle
        };

        std::string hit_sound_path;

        Beatup* beatup;
        Fist* attacker;
        Shield* shield;

        bool has_shield_active();
        void add_shield();
        void remove_shield();
        void destroy_shield();
        void disable_shield();

        float width;


        void spawn_dmg_lbl(int damage);

        float center_x, center_y;
        void set_center(cocos2d::Vec2);
        void set_center(float x, float y);
        void swap_center(Face* other);

        cocos2d::Rect get_swipable_rect();

        cocos2d::Sprite* attacking_sprite;
        cocos2d::Sprite* recovering_sprite;
        std::vector<cocos2d::Sprite*>* sprite_list;
        std::vector<cocos2d::Sprite*>* hurt_sprite_list;

        float get_spawnable_width();

        Face();

        ProgressBar* health_bar;
        void update_health_bar(float dt);

        bool hit(int punch_dmg);
        bool take_damage(int damage);

        void reset_hits();
        bool is_crit_hit;
        void get_punched(Fist* attacker, bool is_crit_hit, float rotation);

        bool should_shake();
        Clock* shake_clock;

        cocos2d::Sprite* get_sprite();
        int get_sprite_index();
        void update_sprite();
        void update(float dt);

        void add_feature(Face::FeatureType ftype);
        std::vector<FacialFeature*>* features;

        void break_all_features();
        void remove_all_features();

        void reset_color();
};

class FacialFeature
{
    public:
        FacialFeature(Face* face, std::string sprite_name, int hit_limit);
        ~FacialFeature();

        bool should_break(int hits_taken);
        void do_break();

        unsigned int hit_limit;

        Face* face;
        ShatterSprite* sprite;
};

class Shield : public Hittable
{
    public:
        ShieldTypes type;
        static bool first_time_human;
        static bool first_time_brick;

        Shield();
        ~Shield();
        Face* face;

        bool hit(int punch_dmg);

        cocos2d::MotionStreak* tutorial_swipe;

        float width;
        float center_x, center_y;
        void set_center(cocos2d::Vec2);
        void set_center(float x, float y);

        std::vector<cocos2d::Sprite*>* sprite_list;
        cocos2d::Sprite* get_sprite();
        int get_sprite_index();
        void update_sprite();

        void play_destroy();
        void play_disable();
        void play_hit();

        void prep_human();
        void prep_brickwall();
};

class Coin
{
    public:
        Coin(float full_width, Beatup* beatup);

        static Coin* spawn(Face* face, Fist* attacker, bool use_particles);

        static bool _has_spin_anim;
        static cocos2d::RepeatForever* _spin_animation;
        static cocos2d::RepeatForever* get_spin_animation();

        void init(Face* face, Fist* attacker, bool use_particles);

        static cocos2d::ValueMap particle_map;
        virtual cocos2d::ParticleSystemQuad* get_particles();

        cocos2d::Sequence* get_movement_action();

        bool move_to_coins_lbl;


        Beatup* beatup;

        bool should_remove;

        float height_extra;

        Clock* time_alive_clock;
        float expiry_time;

        float angle;
        float scale;

        cocos2d::Sprite* sprite;

        bool update(float dt);

};

class Gore : public Coin
{

    public:
        Gore(float full_width, Beatup* beatup);
        static Coin* spawn(Face* face, Fist* attacker, bool use_particles);

        static cocos2d::ValueMap particle_map;
        cocos2d::ParticleSystemQuad* get_particles() override;

        //static bool _has_blood_parts;
        //static cocos2d::ParticleSystemQuad* __blood_parts;
        //static cocos2d::ParticleSystemQuad* get_blood_parts();

};

#endif

