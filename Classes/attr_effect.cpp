
#include "attr_effect.h"
#include "attribute_container.h"
#include "combat.h"

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
    this->armor = NULL;

    this->damage_regen_rate = 0;
    this->damage_regen_interval = 0;
    this->damage = NULL;

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
    // // gaining way too much attrs for ever.
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

    //string_vec.push_back("DCV: "+std::to_string((long double)this->damage->get_raw_total()));
    //string_vec.push_back("DMV: "+std::to_string((long double)this->damage->get_raw_total()));
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

    return "UNIMPLEMENTED 2";

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

    //string_vec.push_back(this->small_convert("DCV: ", (int)this->damage->get_raw_total()));
    string_vec.push_back(this->small_convert("DRR: ", (int)this->damage_regen_rate));
    string_vec.push_back(this->small_convert("DRI: ", (int)this->damage_regen_interval));

    return "UNIMPLEMENTED";

};

