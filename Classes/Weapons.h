#pragma once
#ifndef WEAPONS_H
#define WEAPONS_H

#include "cocos2d.h"
#include "constants.h"
#include "ui/CocosGUI.h"
#include "Buyable.h"
#include "Clock.h"

class Face;
class Fist;
class ShopItem;
class Beatup;
class Combo;

class FistSprite;


class Weapon : public Buyable
{
    public:
        Weapon(Beatup* beatup, std::string id_key, std::string name);

        std::string name = "Unnamed Weapon";

        unsigned int damage;

        void update(float dt);

        virtual unsigned int get_damage() = 0;

};

class ThrowableWeapon : public Weapon
{
    public:
        ThrowableWeapon(Beatup* beatup, std::string id_key, std::string name);

        unsigned int coin_cost;
        void change_cost(unsigned int new_cost);

        void init(std::string sprite_path);

        cocos2d::Sprite* sprite;
        cocos2d::Sprite* background;

        std::string sprite_path;
        cocos2d::Label* label;
        void change_label(std::string msg);

        virtual void init_position() = 0;

        virtual void fire();
        virtual std::string get_fire_mp3_path() = 0;

        virtual void do_animate() = 0;
        virtual void do_fire();

        Clock* cooldown;
        void update(float dt);
};

class RocketWeapon : public ThrowableWeapon
{
    public:
        RocketWeapon(Beatup* beatup, std::string id_key, std::string name) : ThrowableWeapon(beatup, id_key, name) {};
        virtual void init_position() override;

        virtual unsigned int get_damage();
        virtual std::string get_fire_mp3_path();
        virtual void do_animate();
};

class GrenadeWeapon : public RocketWeapon
{
    public:
        GrenadeWeapon(Beatup* beatup, std::string id_key, std::string name) : RocketWeapon(beatup, id_key, name) {};
        void init_position() override;

        virtual unsigned int get_damage();
        virtual std::string get_fire_mp3_path();

        virtual void do_animate();
};

class FistWeapon : public Weapon
{
    public:
        void scale_face_color(Face* face);
        void update(float dt);
        GLuint r_scale;
        GLuint g_scale;
        GLuint b_scale;

        bool is_active;
        void call_on_activated();
        VoidFuncNoArgs on_activated;
        void call_on_deactivated();
        VoidFuncNoArgs on_deactivated;

        FistWeaponTypes type;

        FistWeapon(Beatup* beatup, std::string id_key, std::string name, FistWeaponTypes type) : Weapon(beatup, id_key, name), type(type), is_active(false) {};
        cocos2d::ParticleSystemQuad* parts;

        void init(std::initializer_list<FistSprite*> fist_sprites);
        void init_flames(FistSprite* fs);
        void init_psionic(FistSprite* fs);
        void init_frost(FistSprite* fs);

        unsigned int get_damage() { return 10;  };

};

class WeaponButton : public cocos2d::ui::Button
{
    public:
        Beatup* beatup;
        FistWeapon* fist_weapon;

        cocos2d::Sprite* icon;

        std::string on_activate_sound_path = "";

        void prep(Beatup* beatup, FistWeapon* fist_weapon, cocos2d::Vec2 pos, std::string on_activate_sound_path, std::string icon_path);

        static WeaponButton* create(std::string normal, std::string selected, std::string disabled);

};

#endif
