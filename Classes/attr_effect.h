#pragma once
#ifndef attr_effect_H
#define attr_effect_H

#include <vector>
#include <iostream>
#include <enums/attributes_t.h>

const int NO_DURATION = -1;

class Actor;
class Person;
class Combat;
class Person;
class Item;
class Inventory;
class Tile;
class Item;
class Attribute;
class Damage;
class Armor;

struct type_applied_s 
{
    bool current_val;
    bool max_val;
    bool regen_rate;
    bool interval;
    bool all;
};

struct applied_to_s 
{
    type_applied_s health;
    type_applied_s mana;
    type_applied_s armor;
    type_applied_s damage;
    type_applied_s hunger;
    type_applied_s speed;
    Actor* actor;
};

class AttrEffect;

class TimedEffect
{
    public:
        AttrEffect* effect;
        long double turn_applied;

        bool is_expired(long double turn_count);
};

std::string buffer_color(std::string input, int val, TCODColor color);

class AttrEffect
{
    public:
        int duration;
        bool is_timed();

        int health_current_val;
        int health_max_val;
        int health_regen_rate;
        int health_regen_interval; 

        int mana_current_val;
        int mana_max_val;
        int mana_regen_rate;
        int mana_regen_interval; 

        Armor* armor;
        int armor_current_val;
        int armor_max_val;
        int armor_regen_rate;
        int armor_regen_interval; 

        Damage* damage;
        int damage_regen_rate;
        int damage_regen_interval; 

        int hunger_current_val;
        int hunger_max_val;
        int hunger_regen_rate;
        int hunger_regen_interval; 

        int speed_current_val;
        int speed_max_val;
        int speed_regen_rate;
        int speed_regen_interval; 

        applied_to_s  applied_to;
        std::vector<applied_to_s*>* actors_applied_to;

        AttrEffect();
        ~AttrEffect();

        void ApplyAllEffects(Actor* actor, std::vector<attribute_types_t>* exceptions = NULL);
        void ApplyHealthEffects(Actor* actor);
        void ApplyManaEffects(Actor* actor);
        void ApplyArmorEffects(Actor* actor);
        void ApplyDamageEffects(Actor* actor);
        void ApplyHungerEffects(Actor* actor);
        void ApplySpeedEffects(Actor* actor);

        void RemoveAllEffects(Actor* actor);
        void RemoveHealthEffects(Actor* actor);
        void RemoveManaEffects(Actor* actor);
        void RemoveArmorEffects(Actor* actor);
        void RemoveDamageEffects(Actor* actor);
        void RemoveHungerEffects(Actor* actor);
        void RemoveSpeedEffects(Actor* actor);

        bool already_applied_all(Actor* actor);
        bool already_applied_health(Actor* actor);
        bool already_applied_mana(Actor* actor);
        bool already_applied_armor(Actor* actor);
        bool already_applied_damage(Actor* actor);
        bool already_applied_hunger(Actor* actor);
        bool already_applied_speed(Actor* actor);

        void mark_applied_all(Actor* actor);
        void mark_applied_health(Actor* actor);
        void mark_applied_mana(Actor* actor);
        void mark_applied_armor(Actor* actor);
        void mark_applied_damage(Actor* actor);
        void mark_applied_hunger(Actor* actor);
        void mark_applied_speed(Actor* actor);

        void set_rng_health(TCODRandom* rng, int min, int max, int med);
        void set_rng_mana(TCODRandom* rng, int min, int max, int med);
        int set_rng_armor(TCODRandom* rng, int min, int max, int med);
        int set_rng_damage(TCODRandom* rng, int min, int max, int med);
        int set_rng_hunger(TCODRandom* rng, int min, int max, int med);
        int set_rng_speed(TCODRandom* rng, int min, int max, int med);

        void unmark_applied_all(Actor* actor);

        std::string AttrEffect::small_convert(std::string prefix, int val);
        std::string full_str();
        std::string oneline_str();
        std::string oneline_str_colorless();
        // std::vector<TCODColor> oneline_str_colours();
        std::vector<TCOD_colctrl_t> oneline_str_colours();

        void set_all_vals_to(int new_val);
        void set_health_vals_to(int new_val);
        void set_mana_vals_to(int new_val);
        void set_armor_vals_to(int new_val);
        void set_damage_vals_to(int new_val);
        void set_hunger_vals_to(int new_val);
        void set_speed_vals_to(int new_val);
};

#endif
