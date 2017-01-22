#include <sstream>

#include "combat.h"
#include "attribute_container.h"
#include "attribute.h"
#include "attr_effect.h"

#include "Fighter.h"

void Combat::assign_to_master(spFighter master)
{
    this->master = master.get();
};

Combat::Combat(std::string name, spFighter master)
{
    this->master = master.get();
    this->_is_dead = false;

    this->was_attacked = false;

}

void Combat::level_up(res_count_t levels)
{
    //reset, accounting for left over exp
    res_count_t i = 0;
    while (i < levels)
    {
        this->master->xp->lvl+=1;
        printj("NEW LEVEL IS: " << this->master->xp->lvl << std::endl);
        //log(lvl * 10) * 100
        // this->master->xp_required_to_lvlup = std::floor(std::log(this->master->xp->lvl *10.0f) * 100.0f);

        res_count_t level = this->master->xp->lvl;
        res_count_t lvl10 = level*10;
        // //((x*10)^(x*10)/200))*100
        // res_count_t result = std::pow((lvl10),(lvl10)/200)*100;

        //regen to max health
        this->master->attrs->health->current_val = this->master->attrs->health->current_val;

        // log(x^x)*(x*100)+200
        res_count_t result = std::log(std::pow(level, level)) * (level*100) + 200;
        this->master->xp->required_to_lvlup = std::floor(result);
        res_count_t left = this->master->xp->this_level - this->master->xp->required_to_lvlup;
        res_count_t right = 0.0L;
        this->master->xp->this_level = std::max(left, right);

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

void Combat::level_up_skills(res_count_t levels)
{
    // if (this->master->actor_class == NULL)
    // {
    // }
    // else
    // {
    //     this->master->actor_class->LevelUpSkills(levels);
    // }
};

void Combat::level_up_stats(res_count_t levels)
{
    // if (this->master->actor_class == NULL)
    // {
    //     FighterClass fighter_class = FighterClass();
    //     fighter_class.master = this->master;
    //     fighter_class.LevelUpStats(levels);
    // }
    // else
    // {
    //     this->master->actor_class->LevelUpStats(levels);
    // }
};

void Combat::give_exp(res_count_t exp_to_gain)
{
    this->master->xp->total += exp_to_gain;
    this->master->xp->this_level += exp_to_gain;
    // calc if level up
    if (this->master->xp->this_level >= this->master->xp->required_to_lvlup)
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

    //std::stringstream ss;
    //std::string sneak_msg  = this->master->is_sneaking ? " sneak" : "";
    //ss << this->master->name.c_str();
    //ss << sneak_msg << " attacks " << combat_target->master->name.c_str();
    //ss << " for " << dmg->get_raw_total() << ".";

    //new Message(Ui::msg_handler_main, message_types_t::DAMAGE_GIVEN_MSG, ss.str().c_str());
    combat_target->take_damage(this->shared_from_this(), dmg);

    bool is_target_dead = combat_target->is_dead();
    if (is_target_dead)
    {
        //get opponents exp value
        res_count_t exp_to_gain = combat_target->master->xp->value;
        //add it to the master's exp
        this->give_exp(exp_to_gain);
    };
};

void Combat::die()
{
    printj("I've died!\n");
    //make position unblocked
    if (master != NULL)
    {
        //master->die();
        printj("so has my master");
    }
    else if (master == NULL)
    {
        printj("I've no master so he's not going to die, is he?\n");
    };

};

void Combat::try_to_die()
{
    if (this->is_dead())
    {
        this->die();
    };
}

res_count_t Combat::adjust_damage_to_armor(Damage* dmg)
{
    res_count_t total_damage = 0;
    total_damage += (res_count_t)(dmg->normal - this->master->attrs->armor->current_val);
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

void Combat::take_damage(spCombat combat_attacker, Damage* dmg)
{
    if (dmg >= 0 )
    {
        res_count_t adjusted_dmg = this->adjust_damage_to_armor(dmg);

        adjusted_dmg = std::max(adjusted_dmg, 1.0L);
        this->master->attrs->health->current_val -= adjusted_dmg;

        std::cout << this->master->name;
        std::cout << " took " << adjusted_dmg << " damage! ";
        std::cout << "leaving " << this->master->attrs->health->current_val << "hp left.";
        std::cout << std::endl;

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

bool Combat::is_dead()
{
    if (this->master->attrs->health->current_val <= 0  && !_is_dead)
    {
        _is_dead = true;
    }
    return _is_dead;
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

res_count_t Damage::get_raw_total()
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

res_count_t Armor::get_raw_total()
{
    return this->normal +
        this->fire +
        this->water +
        this->death +
        this->life +
        this->crystal +
        this->spectre;
};

