#include "stdafx.h"

#include "attr_effect.h"
#include "attribute_container.h"
#include "attribute.h"
#include "utils.h"
#include "combat.h"
#include <enums/elements_t.h>
#include <enums/attributes_t.h>

bool TimedEffect::is_expired(long double turn_count)
{
    return (this->effect->duration + this->turn_applied) <= turn_count;
};

AttrEffect::AttrEffect()
{
    this->health_current_val = 100;
    this->health_max_val = 0;
    this->health_regen_rate = 0;
    this->health_regen_interval = 0; 

    this->mana_current_val = 0;
    this->mana_max_val = 0;
    this->mana_regen_rate = 0;
    this->mana_regen_interval = 0; 

    this->armor_current_val = 0;
    this->armor_max_val = 0;
    this->armor_regen_rate = 0;
    this->armor_regen_interval = 0; 
    this->armor = new Armor();

    this->damage_regen_rate = 0;
    this->damage_regen_interval = 0; 
    this->damage = new Damage();

    this->hunger_current_val = 0;
    this->hunger_max_val = 0;
    this->hunger_regen_rate = 0;
    this->hunger_regen_interval = 0; 

    this->speed_current_val = 0;
    this->speed_max_val = 0;
    this->speed_regen_rate = 0;
    this->speed_regen_interval = 0; 


    this->duration = -1;

    this->actors_applied_to = new std::vector<applied_to_s*>;
};

AttrEffect::~AttrEffect()
{
    delete this->armor;
    delete this->damage;
    delete this->actors_applied_to;
};

bool AttrEffect::is_timed()
{
    return this->duration != NO_DURATION;
};

void AttrEffect::set_all_vals_to(int new_val)
{
    this->set_health_vals_to(new_val);
    this->set_mana_vals_to(new_val);
    this->set_armor_vals_to(new_val);
    this->set_damage_vals_to(new_val);
    this->set_hunger_vals_to(new_val);
    this->set_speed_vals_to(new_val);
};

void AttrEffect::set_rng_health(TCODRandom* rng, int min, int max, int med)
{
    int val = rng->getInt(min, max, med);
    this->health_current_val = val;
    this->health_max_val = val;
};

void AttrEffect::set_rng_mana(TCODRandom* rng, int min, int max, int med)
{
    int val = rng->getInt(min, max, med);
    this->mana_current_val = val;
    this->mana_max_val = val;
};

int AttrEffect::set_rng_armor(TCODRandom* rng, int min, int max, int med)
{
    int val = rng->getInt(min, max, med);
    this->armor_current_val = val;
    this->armor_max_val = val;
    return val;
};

int AttrEffect::set_rng_damage(TCODRandom* rng, int min, int max, int med)
{
    int val = rng->getInt(min, max, med);
    this->damage->normal = val;
    return val;
};

int AttrEffect::set_rng_hunger(TCODRandom* rng, int min, int max, int med)
{
    int val = rng->getInt(min, max, med);
    this->hunger_current_val = val;
    this->hunger_max_val = val;
    return val;
};

int AttrEffect::set_rng_speed(TCODRandom* rng, int min, int max, int med)
{
    int val = rng->getInt(min, max, med);
    this->speed_current_val = val;
    this->speed_max_val = val;
    return val;
};


void AttrEffect::set_health_vals_to(int new_val)
{
    this->health_current_val = new_val;
    this->health_max_val = new_val;
    this->health_regen_rate = new_val;
    this->health_regen_interval = new_val; 
};

void AttrEffect::set_mana_vals_to(int new_val)
{
    this->mana_current_val = new_val;
    this->mana_max_val = new_val;
    this->mana_regen_rate = new_val;
    this->mana_regen_interval = new_val; 
};

void AttrEffect::set_armor_vals_to(int new_val)
{
    this->armor_current_val = new_val;
    this->armor_max_val = new_val;
    this->armor_regen_rate = new_val;
    this->armor_regen_interval = new_val; 
};

void AttrEffect::set_damage_vals_to(int new_val)
{
    this->damage->normal = new_val;
    this->damage_regen_rate = new_val;
    this->damage_regen_interval = new_val; 
};

void AttrEffect::set_hunger_vals_to(int new_val)
{
    this->hunger_current_val = new_val;
    this->hunger_max_val = new_val;
    this->hunger_regen_rate = new_val;
    this->hunger_regen_interval = new_val; 
};

void AttrEffect::set_speed_vals_to(int new_val)
{
    this->speed_current_val = new_val;
    this->speed_max_val = new_val;
    this->speed_regen_rate = new_val;
    this->speed_regen_interval = new_val; 
};

bool AttrEffect::already_applied_health(Actor* actor)
{

    //loop through the actors_applied_to vector and find one with the actor
    //passed in
    std::vector<applied_to_s*>::iterator it = this->actors_applied_to->begin();
    for (it; it != this->actors_applied_to->end(); ++it)
    {
        if ((*it)->actor == actor)
        {
            return ((*it)->health.all);
        };

    };
    return false;
};

bool AttrEffect::already_applied_mana(Actor* actor)
{

    //loop through the actors_applied_to vector and find one with the actor
    //passed in
    std::vector<applied_to_s*>::iterator it = this->actors_applied_to->begin();
    for (it; it != this->actors_applied_to->end(); ++it)
    {
        if ((*it)->actor == actor)
        {
            return (*it)->mana.all;
        };

    };
    return false;
};

bool AttrEffect::already_applied_armor(Actor* actor)
{

    //loop through the actors_applied_to vector and find one with the actor
    //passed in
    std::vector<applied_to_s*>::iterator it = this->actors_applied_to->begin();
    for (it; it != this->actors_applied_to->end(); ++it)
    {
        if ((*it)->actor == actor)
        {
            return ((*it)->armor.all);
        };

    };
    return false;
};

bool AttrEffect::already_applied_damage(Actor* actor)
{

    //loop through the actors_applied_to vector and find one with the actor
    //passed in
    std::vector<applied_to_s*>::iterator it = this->actors_applied_to->begin();
    for (it; it != this->actors_applied_to->end(); ++it)
    {
        if ((*it)->actor == actor)
        {
            return ((*it)->damage.all);
        };

    };
    return false;
};

bool AttrEffect::already_applied_hunger(Actor* actor)
{

    //loop through the actors_applied_to vector and find one with the actor
    //passed in
    std::vector<applied_to_s*>::iterator it = this->actors_applied_to->begin();
    for (it; it != this->actors_applied_to->end(); ++it)
    {
        if ((*it)->actor == actor)
        {
            return ((*it)->hunger.all);
        };

    };
    return false;
};

bool AttrEffect::already_applied_speed(Actor* actor)
{

    //loop through the actors_applied_to vector and find one with the actor
    //passed in
    std::vector<applied_to_s*>::iterator it = this->actors_applied_to->begin();
    for (it; it != this->actors_applied_to->end(); ++it)
    {
        if ((*it)->actor == actor)
        {
            return ((*it)->speed.all);
        };

    };
    return false;
};

void AttrEffect::unmark_applied_all(Actor* actor)
{

    //loop through the actors_applied_to vector and find one with the actor
    //passed in
    std::vector<applied_to_s*>::iterator it = this->actors_applied_to->begin();
    for (it; it != this->actors_applied_to->end(); ++it)
    {
        if ((*it)->actor == actor)
        {
            (*it)->health.all = false;
            (*it)->mana.all = false;
            (*it)->armor.all  = false;
            (*it)->damage.all = false;
            (*it)->hunger.all = false;
            (*it)->speed.all = false;
        };

    };
};
bool AttrEffect::already_applied_all(Actor* actor)
{

    //loop through the actors_applied_to vector and find one with the actor
    //passed in
    std::vector<applied_to_s*>::iterator it = this->actors_applied_to->begin();
    for (it; it != this->actors_applied_to->end(); ++it)
    {
        if ((*it)->actor == actor)
        {
            return ((*it)->health.all && (*it)->mana.all && (*it)->armor.all  && (*it)->damage.all && (*it)->hunger.all && (*it)->speed.all );
        };

    };

    //if actor in list of people this item's already applied to, no reason to do
    // //it again
    // auto it = std::find(this->actors_applied_to->begin(), this->actors_applied_to->end(), actor);

    // return  it != this->actors_applied_to->end();

    // // The thing is that I need to sort out how to apply the item's effects to
    // // the actor once, because if it gets reapplied they might be invincible or
    // // gaining way too much health for ever.
    // //TODO TODO TODO

    return false;
};


void AttrEffect::mark_applied_health(Actor* actor)
{
    //loop through the actors_applied_to vector and find one with the actor
    //passed in
    std::vector<applied_to_s*>::iterator it = this->actors_applied_to->begin();
    for (it; it != this->actors_applied_to->end(); ++it)
    {
        if ((*it)->actor == actor)
        {
            ((*it)->health.all = true);
        };

    };
};


void AttrEffect::mark_applied_mana(Actor* actor)
{
    //loop through the actors_applied_to vector and find one with the actor
    //passed in
    std::vector<applied_to_s*>::iterator it = this->actors_applied_to->begin();
    for (it; it != this->actors_applied_to->end(); ++it)
    {
        if ((*it)->actor == actor)
        {
            ((*it)->mana.all = true);
        };

    };
};

void AttrEffect::mark_applied_armor(Actor* actor)
{
    //loop through the actors_applied_to vector and find one with the actor
    //passed in
    std::vector<applied_to_s*>::iterator it = this->actors_applied_to->begin();
    for (it; it != this->actors_applied_to->end(); ++it)
    {
        if ((*it)->actor == actor)
        {
            ((*it)->armor.all = true);
        };

    };
};

void AttrEffect::mark_applied_damage(Actor* actor)
{
    //loop through the actors_applied_to vector and find one with the actor
    //passed in
    std::vector<applied_to_s*>::iterator it = this->actors_applied_to->begin();
    for (it; it != this->actors_applied_to->end(); ++it)
    {
        if ((*it)->actor == actor)
        {
            ((*it)->damage.all = true);
        };

    };
};

void AttrEffect::mark_applied_hunger(Actor* actor)
{
    //loop through the actors_applied_to vector and find one with the actor
    //passed in
    std::vector<applied_to_s*>::iterator it = this->actors_applied_to->begin();
    for (it; it != this->actors_applied_to->end(); ++it)
    {
        if ((*it)->actor == actor)
        {
            ((*it)->hunger.all = true);
        };

    };
};

void AttrEffect::mark_applied_speed(Actor* actor)
{
    //loop through the actors_applied_to vector and find one with the actor
    //passed in
    std::vector<applied_to_s*>::iterator it = this->actors_applied_to->begin();
    for (it; it != this->actors_applied_to->end(); ++it)
    {
        if ((*it)->actor == actor)
        {
            ((*it)->speed.all = true);
        };

    };
};

void AttrEffect::mark_applied_all(Actor* actor)
{
    //loop through the actors_applied_to vector and find one with the actor
    //passed in
    std::vector<applied_to_s*>::iterator it = this->actors_applied_to->begin();
    for (it; it != this->actors_applied_to->end(); ++it)
    {
        if ((*it)->actor == actor)
        {
            ((*it)->health.all = true);
            ((*it)->mana.all = true);
            ((*it)->armor.all = true);
            ((*it)->damage.all = true);
            (*it)->hunger.all = true;
        };

    };
};


void AttrEffect::ApplyAllEffects(Actor* actor, std::vector<attribute_types_t>* exceptions)
{
    if (exceptions == NULL)
    {
    this->ApplyHealthEffects(actor);
    this->ApplyManaEffects(actor);
    this->ApplyArmorEffects(actor);
    this->ApplyDamageEffects(actor);
    this->ApplyHungerEffects(actor);
    this->ApplySpeedEffects(actor);
    }
    else
    {
        if ( std::find(exceptions->begin(), exceptions->end(), attribute_types_t::HealthAttrType)==exceptions->end())
        {
            this->ApplyHealthEffects(actor);
        };
        if ( std::find(exceptions->begin(), exceptions->end(), attribute_types_t::ManaAttrType)==exceptions->end())
        {
            this->ApplyManaEffects(actor);
        };
        if ( std::find(exceptions->begin(), exceptions->end(), attribute_types_t::ArmorAttrType)==exceptions->end())
        {
            this->ApplyArmorEffects(actor);
        };
        if ( std::find(exceptions->begin(), exceptions->end(), attribute_types_t::DamageAttrType)==exceptions->end())
        {
            this->ApplyDamageEffects(actor);
        };
        if ( std::find(exceptions->begin(), exceptions->end(), attribute_types_t::HungerAttrType)==exceptions->end())
        {
            this->ApplyHungerEffects(actor);
        };
        if ( std::find(exceptions->begin(), exceptions->end(), attribute_types_t::SpeedAttrType)==exceptions->end())
        {
            this->ApplySpeedEffects(actor);
        };
    };
    if (actor->combat!= NULL)
        actor->combat->try_to_die();
    else
        std::cout << "this thing has no way to fight back... why are you a dick?" << std::endl;
};

void AttrEffect::ApplyHealthEffects(Actor* actor)
{
    if (! this->already_applied_health(actor))
    {
        actor->attrs->health->add_to_max_val(this->health_max_val);
        actor->attrs->health->add_to_current_val(this->health_current_val);
        actor->attrs->health->add_to_regen_rate(this->health_regen_rate);
        actor->attrs->health->add_to_regen_interval(this->health_regen_interval);
        this->mark_applied_health(actor);
    }
};

void AttrEffect::ApplyManaEffects(Actor* actor)
{
    if (! this->already_applied_mana(actor))
    {
        actor->attrs->mana->add_to_max_val(this->mana_max_val);
        actor->attrs->mana->add_to_current_val(this->mana_current_val);
        actor->attrs->mana->add_to_regen_rate(this->mana_regen_rate);
        actor->attrs->mana->add_to_regen_interval(this->mana_regen_interval);
        this->mark_applied_mana(actor);
    }
};

void AttrEffect::ApplyArmorEffects(Actor* actor)
{
    if (! this->already_applied_armor(actor))
    {
        actor->attrs->armor->add_to_max_val(this->armor_max_val);
        actor->attrs->armor->add_to_current_val(this->armor_current_val);
        actor->attrs->armor->add_to_regen_rate(this->armor_regen_rate);
        actor->attrs->armor->add_to_regen_interval(this->armor_regen_interval);
        this->mark_applied_armor(actor);
    }
};

void AttrEffect::ApplyDamageEffects(Actor* actor)
{
    if (! this->already_applied_damage(actor))
    {
        actor->attrs->damage->add_to_max_val(this->damage->get_raw_total());
        actor->attrs->damage->add_to_current_val(this->damage->get_raw_total());
        actor->attrs->damage->add_to_regen_rate(this->damage_regen_rate);
        actor->attrs->damage->add_to_regen_interval(this->damage_regen_interval);
        this->mark_applied_damage(actor);
    }
};

void AttrEffect::ApplyHungerEffects(Actor* actor)
{
    if (! this->already_applied_hunger(actor))
    {
        actor->attrs->hunger->add_to_max_val(this->hunger_max_val);
        actor->attrs->hunger->add_to_current_val(this->hunger_current_val);
        actor->attrs->hunger->add_to_regen_rate(this->hunger_regen_rate);
        actor->attrs->hunger->add_to_regen_interval(this->hunger_regen_interval);
        this->mark_applied_hunger(actor);
    }
};

void AttrEffect::ApplySpeedEffects(Actor* actor)
{
    if (! this->already_applied_speed(actor))
    {
        actor->attrs->speed->add_to_max_val(this->speed_max_val);
        actor->attrs->speed->add_to_current_val(this->speed_current_val);
        actor->attrs->speed->add_to_regen_rate(this->speed_regen_rate);
        actor->attrs->speed->add_to_regen_interval(this->speed_regen_interval);
        this->mark_applied_speed(actor);
    }
};

void AttrEffect::RemoveAllEffects(Actor* actor)
{
    this->RemoveHealthEffects(actor);
    this->RemoveManaEffects(actor);
    this->RemoveArmorEffects(actor);
    this->RemoveDamageEffects(actor);
    this->RemoveHungerEffects(actor);
    this->RemoveSpeedEffects(actor);
};

void AttrEffect::RemoveHealthEffects(Actor* actor)
{
    actor->attrs->health->remove_from_current_val(this->health_current_val);
    actor->attrs->health->remove_from_max_val(this->health_max_val);
    actor->attrs->health->remove_from_regen_rate(this->health_regen_rate);
    actor->attrs->health->remove_from_regen_interval(this->health_regen_interval);
};

void AttrEffect::RemoveManaEffects(Actor* actor)
{
    actor->attrs->mana->remove_from_current_val(this->mana_current_val);
    actor->attrs->mana->remove_from_max_val(this->mana_max_val);
    actor->attrs->mana->remove_from_regen_rate(this->mana_regen_rate);
    actor->attrs->mana->remove_from_regen_interval(this->mana_regen_interval);
};

void AttrEffect::RemoveArmorEffects(Actor* actor)
{
    actor->attrs->armor->remove_from_current_val(this->armor_current_val);
    actor->attrs->armor->remove_from_max_val(this->armor_max_val);
    actor->attrs->armor->remove_from_regen_rate(this->armor_regen_rate);
    actor->attrs->armor->remove_from_regen_interval(this->armor_regen_interval);
};

void AttrEffect::RemoveDamageEffects(Actor* actor)
{
    actor->attrs->damage->remove_from_current_val(this->damage->get_raw_total());
    actor->attrs->damage->remove_from_max_val(this->damage->get_raw_total());
    actor->attrs->damage->remove_from_regen_rate(this->damage_regen_rate);
    actor->attrs->damage->remove_from_regen_interval(this->damage_regen_interval);
};

void AttrEffect::RemoveHungerEffects(Actor* actor)
{
    actor->attrs->hunger->remove_from_current_val(this->hunger_current_val);
    actor->attrs->hunger->remove_from_max_val(this->hunger_max_val);
    actor->attrs->hunger->remove_from_regen_rate(this->hunger_regen_rate);
    actor->attrs->hunger->remove_from_regen_interval(this->hunger_regen_interval);
};

void AttrEffect::RemoveSpeedEffects(Actor* actor)
{
    actor->attrs->speed->remove_from_current_val(this->speed_current_val);
    actor->attrs->speed->remove_from_max_val(this->speed_max_val);
    actor->attrs->speed->remove_from_regen_rate(this->speed_regen_rate);
    actor->attrs->speed->remove_from_regen_interval(this->speed_regen_interval);
};


std::string AttrEffect::full_str()
{

    std::string result = "";
    std::vector<std::string> string_vec;

    string_vec.push_back("HCV: "+std::to_string((long double)this->health_current_val));
    string_vec.push_back("HMV: "+std::to_string((long double)this->health_max_val));
    string_vec.push_back("HRR: "+std::to_string((long double)this->health_regen_rate));
    string_vec.push_back("HRI: "+std::to_string((long double)this->health_regen_interval));

    string_vec.push_back("MCV: "+std::to_string((long double)this->mana_current_val));
    string_vec.push_back("MMV: "+std::to_string((long double)this->mana_max_val));
    string_vec.push_back("MRR: "+std::to_string((long double)this->mana_regen_rate));
    string_vec.push_back("MRI: "+std::to_string((long double)this->mana_regen_interval));

    string_vec.push_back("ACV: "+std::to_string((long double)this->armor_current_val));
    string_vec.push_back("AMV: "+std::to_string((long double)this->armor_max_val));
    string_vec.push_back("ARR: "+std::to_string((long double)this->armor_regen_rate));
    string_vec.push_back("ARI: "+std::to_string((long double)this->armor_regen_interval));

    string_vec.push_back("DCV: "+std::to_string((long double)this->damage->get_raw_total()));
    string_vec.push_back("DMV: "+std::to_string((long double)this->damage->get_raw_total()));
    string_vec.push_back("DRR: "+std::to_string((long double)this->damage_regen_rate));
    string_vec.push_back("DRI: "+std::to_string((long double)this->damage_regen_interval));

    string_vec.push_back("FCV: "+std::to_string((long double)this->hunger_current_val));
    string_vec.push_back("FMV: "+std::to_string((long double)this->hunger_max_val));
    string_vec.push_back("FRR: "+std::to_string((long double)this->hunger_regen_rate));
    string_vec.push_back("FRI: "+std::to_string((long double)this->hunger_regen_interval));

    string_vec.push_back("SCV: "+std::to_string((long double)this->speed_current_val));
    string_vec.push_back("SMV: "+std::to_string((long double)this->speed_max_val));
    string_vec.push_back("SRR: "+std::to_string((long double)this->speed_regen_rate));
    string_vec.push_back("SRI: "+std::to_string((long double)this->speed_regen_interval));


    if (string_vec.size() != 0)
        string_vec.push_back("%c");

    return StringJoin(string_vec, '\n', false);

};

std::string AttrEffect::small_convert(std::string prefix, int val)
{
    if (val != 0)
    {
        return prefix + std::to_string((long double)val);
    }
    else
    {
        return "";
    };

};

// std::vector<TCOD_colctrl_t> AttrEffect::oneline_str_colours_FIXED()
// {
// };



std::string AttrEffect::oneline_str()
{

    std::string result = "";
    std::vector<std::string> string_vec;

    TCODColor health_color = HealthAttribute::attribute_color;
    TCODColor mana_color = ManaAttribute::attribute_color;
    TCODColor armor_color = ArmorAttribute::attribute_color;
    TCODColor damage_color = DamageAttribute::attribute_color;
    TCODColor hunger_color = HungerAttribute::attribute_color;
    TCODColor speed_color = SpeedAttribute::attribute_color;

    std::stringstream ss;

    ss << buffer_color("HCV", this->health_current_val, health_color);
    ss << buffer_color("HMV", this->health_max_val, health_color);
    ss << buffer_color("HRR", this->health_regen_rate, health_color);
    ss << buffer_color("HRI", this->health_regen_interval, health_color);

    ss << buffer_color("MCV", this->mana_current_val, mana_color);
    ss << buffer_color("MMV", this->mana_max_val, mana_color);
    ss << buffer_color("MRR", this->mana_regen_rate, mana_color);
    ss << buffer_color("MRI", this->mana_regen_interval, mana_color);

    ss << buffer_color("ACV", this->armor_current_val, armor_color);
    ss << buffer_color("AMV", this->armor_max_val, armor_color);
    ss << buffer_color("ARR", this->armor_regen_rate, armor_color);
    ss << buffer_color("ARI", this->armor_regen_interval, armor_color);

    // ss << buffer_color("DCV", this->damage->normal, damage_color);
    // ss << buffer_color("DMV", this->damage->normal, damage_color);
    // ss << buffer_color("DRR", this->damage_regen_rate, damage_color);
    // ss << buffer_color("DRI", this->damage_regen_interval, damage_color);

    ss << buffer_color("NRM", this->damage->normal, damage_color);
    ss << buffer_color("FIR", this->damage->fire, get_element_color(FireElement));
    ss << buffer_color("WTR", this->damage->water, get_element_color(WaterElement));
    ss << buffer_color("LIF", this->damage->life, get_element_color(LifeElement));
    ss << buffer_color("DTH", this->damage->death, get_element_color(DeathElement));
    ss << buffer_color("CTL", this->damage->crystal, get_element_color(CrystalElement));
    ss << buffer_color("SPR", this->damage->spectre, get_element_color(SpectreElement));

    ss << buffer_color("FCV", this->hunger_current_val, hunger_color);
    ss << buffer_color("FMV", this->hunger_max_val, hunger_color);
    ss << buffer_color("FRR", this->hunger_regen_rate, hunger_color);
    ss << buffer_color("FRI", this->hunger_regen_interval, hunger_color);

    ss << buffer_color("SCV", this->speed_current_val, speed_color);
    ss << buffer_color("SMV", this->speed_max_val, speed_color);
    ss << buffer_color("SRR", this->speed_regen_rate, speed_color);
    ss << buffer_color("SRI", this->speed_regen_interval, speed_color);


    return ss.str();
};

std::vector<TCOD_colctrl_t> AttrEffect::oneline_str_colours()
{
    TCODColor health_color = TCODColor::desaturatedGreen;
    TCODConsole::setColorControl(TCOD_COLCTRL_1, health_color, TCODColor::black);
    TCODColor mana_color = TCODColor::desaturatedBlue;
    TCODConsole::setColorControl(TCOD_COLCTRL_2, mana_color, TCODColor::black);
    TCODColor armor_color = TCODColor::lightGrey;
    TCODConsole::setColorControl(TCOD_COLCTRL_3, armor_color, TCODColor::black);
    TCODColor damage_color = TCODColor::desaturatedRed;
    TCODConsole::setColorControl(TCOD_COLCTRL_4, damage_color, TCODColor::black);
    std::vector<TCOD_colctrl_t>  color_vector;   
    //for each attr in this item effect that isn't 0, return a attr specific
    //color
    if (this->health_current_val != 0)
        color_vector.push_back(TCOD_COLCTRL_1);
    if (this->health_max_val != 0)
        color_vector.push_back(TCOD_COLCTRL_1);
    if (this->health_regen_rate != 0)
        color_vector.push_back(TCOD_COLCTRL_1);
    if (this->health_regen_interval != 0)
        color_vector.push_back(TCOD_COLCTRL_1);

    if (this->mana_current_val != 0)
        color_vector.push_back(TCOD_COLCTRL_2);
    if (this->mana_max_val != 0)
        color_vector.push_back(TCOD_COLCTRL_2);
    if (this->mana_regen_rate != 0)
        color_vector.push_back(TCOD_COLCTRL_2);
    if (this->mana_regen_interval != 0)
        color_vector.push_back(TCOD_COLCTRL_2);

    if (this->armor_current_val != 0)
        color_vector.push_back(TCOD_COLCTRL_3);
    if (this->armor_max_val != 0)
        color_vector.push_back(TCOD_COLCTRL_3);
    if (this->armor_regen_rate != 0)
        color_vector.push_back(TCOD_COLCTRL_3);
    if (this->armor_regen_interval != 0)
        color_vector.push_back(TCOD_COLCTRL_3);

    if (this->damage->get_raw_total() != 0)
        color_vector.push_back(TCOD_COLCTRL_4);
    if (this->damage_regen_rate != 0)
        color_vector.push_back(TCOD_COLCTRL_4);
    if (this->damage_regen_interval != 0)
        color_vector.push_back(TCOD_COLCTRL_4);

    //color_vector.push_back(TCOD_COLCTRL_STOP);
    return color_vector;

};

std::string AttrEffect::oneline_str_colorless()
{

    std::string result = "";
    std::vector<std::string> string_vec;

    string_vec.push_back(this->small_convert("HCV: ", (int)this->health_current_val));
    string_vec.push_back(this->small_convert("HMV: ", (int)this->health_max_val));
    string_vec.push_back(this->small_convert("HRR: ", (int)this->health_regen_rate));
    string_vec.push_back(this->small_convert("HRI: ", (int)this->health_regen_interval));

    string_vec.push_back(this->small_convert("MCV: ", (int)this->mana_current_val));
    string_vec.push_back(this->small_convert("MMV: ", (int)this->mana_max_val));
    string_vec.push_back(this->small_convert("MRR: ", (int)this->mana_regen_rate));
    string_vec.push_back(this->small_convert("MRI: ", (int)this->mana_regen_interval));

    string_vec.push_back(this->small_convert("ACV: ", (int)this->armor_current_val));
    string_vec.push_back(this->small_convert("AMV: ", (int)this->armor_max_val));
    string_vec.push_back(this->small_convert("ARR: ", (int)this->armor_regen_rate));
    string_vec.push_back(this->small_convert("ARI: ", (int)this->armor_regen_interval));

    string_vec.push_back(this->small_convert("DCV: ", (int)this->damage->get_raw_total()));
    string_vec.push_back(this->small_convert("DRR: ", (int)this->damage_regen_rate));
    string_vec.push_back(this->small_convert("DRI: ", (int)this->damage_regen_interval));

    return StringJoin(string_vec, ' ', true);

};

