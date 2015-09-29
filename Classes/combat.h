#pragma once
#ifndef COMBAT_H
#define COMBAT_H
#include <sstream>
#include <vector>
// #include <actors/actor.h>
// #include <actors\Person.h>

class Person; 
class Actor; 
class Game;
class Damage;

class Combat 
{
    private:
        std::string default_name;
    public:
        // std::string name;
        Actor* master;

        bool is_dead;
        bool was_attacked;
        Actor* last_victim;
        std::vector<Combat*> * attackers;

        Combat (std::string name, int max_hp, Person* master, char representation = 'p' );

        void attack(Combat* Combat_target, Damage* dmg);
        void take_damage(Combat* Combat_attacker, Damage* dmg);
        Combat* get_last_attacker();
        int adjust_damage_to_armor(Damage* dmg);
        void remember_attacker(Combat* Combat_attacker, bool mark_the_attk);

        void try_to_die();
        bool check_death();
        void die();

        void assign_to_master(Person* master);

        void give_exp(int exp_to_gain);

        void level_up(int levels = 1);
        void level_up_stats(int levels);
        void level_up_skills(int levels);

        void printout();
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
#endif
