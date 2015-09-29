#include "stdafx.h"
#include <sstream>
#include <typeinfo>
#include <cstdlib>

#include <libtcod.hpp>

#include "combat.h"
#include "attribute_container.h"
#include <actors\Person.h>
#include <actors\actor.h>
#include "attribute.h"
#include "tile.h"
#include <Representation.h>
#include "ui.h"
#include "messages.h"
#include "spells.h"
#include "attr_effect.h"
#include "class.h"
#include "thinker.h"


void Combat::printout(){

    std::cout << "\nCombat PRINTOUT" << std::endl;
    //cout << "NAME: " << name << endl;
    std::cout << "MAX HP: " << this->master->attrs->health->max_val << std::endl;

};


void Combat::assign_to_master(Person* master)
{
    this->master = master;
};

Combat::Combat(std::string name, int max_hp, Person* master, char representation)
{
    representation = representation;

    this->master = NULL;
    this->is_dead = false;

    this->attackers = new std::vector<Combat*>;
    this->was_attacked = false;

    this->last_victim = NULL;

};

void Combat::level_up(int levels)
{
    //reset, accounting for left over exp
    int i = 0;
    while (i < levels)
    {


        this->master->level+=1;
        std::cout << "NEW LEVEL IS: " << this->master->level << std::endl;
        if (this->master == Game::player)
        {
            std::stringstream msg;
            msg <<  "You are now level " << this->master->level << ".";
            new Message(Ui::msg_handler_main, NOTYPE_MSG, colfg(TCODColor::lightBlue, msg.str()));
        };
        //log(lvl * 10) * 100
        // this->master->xp_required_to_lvlup = std::floor(std::log(this->master->level *10.0f) * 100.0f);

        double level = this->master->level;
        double lvl10 = level*10;
        // //((x*10)^(x*10)/200))*100
        // double result = std::pow((lvl10),(lvl10)/200)*100;

        // log(x^x)*(x*100)+200
        double result = std::log(std::pow(level, level)) * (level*100) + 200;
        this->master->xp_required_to_lvlup = std::floor(result);
        this->master->xp_this_level = std::max((double)this->master->xp_this_level - this->master->xp_required_to_lvlup, 0.0); 

        this->level_up_stats(1);
        this->level_up_skills(1);
        i++;
    }

    //animate
    // Tile* tile = this->master->my_tile;
    // tile_vec_t* adj_tiles = tile->getAdjacentTiles(2);
    // for (tile_vec_t::iterator it = adj_tiles->begin(); it != adj_tiles->end(); ++it)
    // {
    //     (*it)->tile->representation->temp_bg_color = (TCODColor*)(&TCODColor::gold);
    // };
    // std::cout << "just exploded with color for a frame" << std::endl;
};

void Combat::level_up_skills(int levels)
{
    if (this->master->actor_class == NULL)
    {
    }
    else
    {
        this->master->actor_class->LevelUpSkills(levels);
    }
};

void Combat::level_up_stats(int levels)
{
    if (this->master->actor_class == NULL)
    {
        FighterClass fighter_class = FighterClass();
        fighter_class.master = this->master;
        fighter_class.LevelUpStats(levels);
    }
    else
    {
        this->master->actor_class->LevelUpStats(levels);
    }
};

void Combat::give_exp(int exp_to_gain)
{
    this->master->xp += exp_to_gain;
    this->master->xp_this_level += exp_to_gain;
    // calc if level up
    if (this->master->xp_this_level >= this->master->xp_required_to_lvlup)
    {
        this->level_up();
    };
};

void Combat::attack(Combat* combat_target, Damage* dmg)
{
    //cout << "About to attack " << (*combat_target).name << endl;

    if (combat_target == NULL)
    {
        printf("target had no combat\n");
        return;
    }

    std::stringstream ss;
    std::string sneak_msg  = this->master->is_sneaking ? " sneak" : "";
    ss << this->master->name.c_str();
    ss << sneak_msg << " attacks " << combat_target->master->name.c_str();
    ss << " for " << dmg->get_raw_total() << ".";

    new Message(Ui::msg_handler_main, message_types_t::DAMAGE_GIVEN_MSG, ss.str().c_str());
    combat_target->take_damage(this, dmg);
    this->last_victim = combat_target->master;

    bool is_target_dead = combat_target->check_death();
    if (is_target_dead)
    {
        //get opponents exp value
        int exp_to_gain = combat_target->master->xp_value;
        //add it to the master's exp
        this->give_exp(exp_to_gain);
    };
};

void Combat::remember_attacker(Combat* combat_attacker, bool mark_the_attk=true)
{
    if (mark_the_attk == true) 
    {
        was_attacked = true;
        if (this->master != Game::player)
        {
            this->master->thinker->target = combat_attacker->master;
        }
    };

    if(std::find(attackers->begin(), attackers->end(), combat_attacker) != attackers->end()) 
    {
        // printf("I've already been attacked by you.\n");
    }
    else 
    {
        attackers->push_back(combat_attacker);
        // printf("Oh hi, you're new.\n");
    }

};

void Combat::die()
{
    printf("I've died!\n");
    //make position unblocked
    if (master != NULL)
    {
        master->die();
    }
    else if (master == NULL)
    {
        printf("I've no master so he's not going to die, is he?\n");
    };

};

Combat* Combat::get_last_attacker()
{
    // std::cout << "*** Retaliation ***" << std::endl;
    Combat * assailant;
    if (attackers->empty()) { return NULL; }
    assailant = attackers->back();
    // cout << "attacker: " << assailant->name << endl;

    return assailant;
};

void Combat::try_to_die()
{
    if (this->check_death())
    {
        this->die();
    };
}

int Combat::adjust_damage_to_armor(Damage* dmg)
{
    int total_damage = 0;
    total_damage += dmg->normal - this->master->attrs->armor->current_val;
    //TODO handle other armor types, need to find a place to store them on the
    //combat or actor
    total_damage += dmg->fire;
    total_damage += dmg->water;
    total_damage += dmg->death;
    total_damage += dmg->life;
    total_damage += dmg->crystal;
    total_damage += dmg->spectre;
    return total_damage;
};

void Combat::take_damage(Combat* combat_attacker, Damage* dmg)
{
    if (dmg >= 0 ) 
    {
        // if (this->master->thinker != NULL && this->master->thinker->is_ally) { return; }; //ally invincible

        TCODRandom* rng = Game::stat_rolls_rng;
        int dodge_chance = 15;
        int dodge_result = rng->get(0, 100);


        if (dodge_result < dodge_chance) 
        {
            new Message(Ui::msg_handler_main, DAMAGE_TAKEN_MSG, colfg(TCODColor::lightAmber, this->master->name+" dodged the attack!."));
            return;
        };

        int adjusted_dmg = this->adjust_damage_to_armor(dmg);
        if (combat_attacker->master == Game::player) { Game::stats->damage_dealt += adjusted_dmg; };
        if (this->master == Game::player) { Game::stats->damage_taken += adjusted_dmg; };

        if (this->master->is_sneaking)
        {
            adjusted_dmg *= 1.6;
        };

        if (this->master->is_defending)
        {
            adjusted_dmg = adjusted_dmg - this->master->attrs->armor->current_val; // effectively double armor
            new Message(Ui::msg_handler_main, DAMAGE_TAKEN_MSG, colfg(TCODColor::lightAmber, this->master->name+" deflected some damage!."));
        };

        int original_adjusted_dmg = adjusted_dmg;
        adjusted_dmg = std::max(adjusted_dmg, 1);
        if (this->master->soullinked_to != NULL)
        {
            Actor* poor_soul = this->master->soullinked_to;
            //poor_soul->combat->take_damage(combat_attacker, adjusted_dmg/2); //TODO: figure out how to multiply/divide Damage
            this->master->attrs->health->current_val -= adjusted_dmg/2;
            poor_soul->attrs->health->current_val -= adjusted_dmg/2;
            std::cout << poor_soul->name << " burned from soullink to " << this->master->name << std::endl;
        }
        else
        {
            this->master->attrs->health->current_val -= adjusted_dmg;
        }
        if (this->master == Game::player)
        {
            if (adjusted_dmg > 15)
            {
                printf("what the f?\n");
            };
        };

        std::cout << this->master->name;
        std::cout << " took " << adjusted_dmg << " damage! ";
        std::cout << "leaving " << this->master->attrs->health->current_val << "hp left.";
        std::cout << std::endl;

        //save attacker in history
        this->remember_attacker(combat_attacker);

        if (dmg != 0)
        {
            this->try_to_die();
        }
    }
    else
    {
        (this->master->attrs->health->current_val)-= dmg->normal;

        std::cout << this->master->name;
        std::cout << " gained " << (-dmg->normal) << " health! ";
        std::cout << "with " << this->master->attrs->health->current_val << "hp left.";
        std::cout << std::endl;
    };
};

bool Combat::check_death()
{
    if (this->master->attrs->health->current_val <= 0  && !is_dead)
    {
        is_dead = true;
    }
    return is_dead;
};

Damage::Damage()
{
    this->normal = 0;
    this->fire = 0;
    this->water = 0;
    this->death = 0;
    this->life = 0;
    this->crystal = 0;
    this->spectre = 0;
};

Damage Damage::operator+(const Damage& other)
{
    Damage result = Damage(*this);

    result.normal+=other.normal;
    result.fire+=other.fire;
    result.water+=other.water;
    result.death+=other.death;
    result.life+=other.life;
    result.crystal+=other.crystal;
    result.spectre+=other.spectre;

    return result;
};

int Damage::get_raw_total()
{
    return this->normal +
        this->fire +
        this->water +
        this->death +
        this->life +
        this->crystal +
        this->spectre;
};


Armor::Armor()
{
    this->normal = 0;
    this->fire = 0;
    this->water = 0;
    this->death = 0;
    this->life = 0;
    this->crystal = 0;
    this->spectre = 0;
};

int Armor::get_raw_total()
{
    return this->normal +
        this->fire +
        this->water +
        this->death +
        this->life +
        this->crystal +
        this->spectre;
};

