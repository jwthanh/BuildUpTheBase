#pragma once
#ifndef COMBAT_H
#define COMBAT_H
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
        Fighter* master;

        bool _is_dead;
        bool was_attacked;

        Combat(std::string name, spFighter master);

        void attack(Combat* Combat_target, Damage* dmg);
        void take_damage(spCombat Combat_attacker, Damage* dmg);
        res_count_t adjust_damage_to_armor(Damage* dmg);

        void try_to_die();
        bool is_dead();
        void die();

        void assign_to_master(spFighter master);

        void give_exp(res_count_t exp_to_gain);

        void level_up(res_count_t levels = 1);
        void level_up_stats(res_count_t levels);
        void level_up_skills(res_count_t levels);
};

class Damage
{
    public:
        res_count_t normal;
        res_count_t fire;
        res_count_t water;
        res_count_t death;
        res_count_t life;
        res_count_t crystal;
        res_count_t spectre;

        Damage();

        Damage operator+(const Damage& other);

        res_count_t get_raw_total();

};

class Armor
{
    public:
        res_count_t normal;
        res_count_t fire;
        res_count_t water;
        res_count_t death;
        res_count_t life;
        res_count_t crystal;
        res_count_t spectre;

        Armor();
        res_count_t get_raw_total();

};

class Experience
{
    public:
        /* if you're level 9 with 960 total experience and it'd take 1000 for level
         * 10, total would be 960, this_level would be 60, required_to_lvlup would be
         * 100 */
        res_count_t lvl;
        res_count_t total;
        res_count_t this_level;
        res_count_t required_to_lvlup;

        res_count_t value;

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
