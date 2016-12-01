#pragma once
#ifndef COMBAT_H
#define COMBAT_H
#include <sstream>
#include <vector>
// #include <actors/actor.h>
// #include <actors\Person.h>
#include "constants.h"

class Person; 
class Fighter; 
class Game;
class Damage;

class Combat : public std::enable_shared_from_this<Combat>
{
    private:
        std::string default_name;
    public:
        // std::string name;
        spFighter master;

        bool _is_dead;
        bool was_attacked;
        spFighter last_victim;
        vsCombat attackers;

        Combat(std::string name, spFighter master);

        void attack(Combat* Combat_target, Damage* dmg);
        void take_damage(spCombat Combat_attacker, Damage* dmg);
	spCombat get_last_attacker();
        int adjust_damage_to_armor(Damage* dmg);
        void remember_attacker(spCombat Combat_attacker, bool mark_the_attk);

        void try_to_die();
        bool is_dead();
        void die();

        void assign_to_master(spFighter master);

        void give_exp(int exp_to_gain);

        void level_up(int levels = 1);
        void level_up_stats(int levels);
        void level_up_skills(int levels);
};

class Damage 
{
    public:
        int normal;
        int fire;
        int water;
        int death;
        int life;
        int crystal;
        int spectre;

        Damage();

        Damage operator+(const Damage& other);

        int get_raw_total();

};

class Armor
{
    public:
        int normal;
        int fire;
        int water;
        int death;
        int life;
        int crystal;
        int spectre;

        Armor();
        int get_raw_total();

};

class Experience
{
    public:
        /* if you're level 9 with 960 total experience and it'd take 1000 for level 
         * 10, total would be 960, this_level would be 60, required_to_lvlup would be
         * 100 */
        unsigned int lvl;
        unsigned int total; 
        unsigned int this_level;
        unsigned int required_to_lvlup;

        unsigned int value;

        Experience(): 
            total(0),
            this_level(0),
            required_to_lvlup(100),
            lvl(1),
            value(10)
        {}

        float get_progress_percentage()
        {
            return static_cast<float>(this->this_level) / static_cast<float>(this->required_to_lvlup);
        }
};
#endif
