#include "HarvestableManager.h"
#include "Fighter.h"
#include "attribute_container.h"
#include "attribute.h"

HarvestableManager::HarvestableManager()
{
    this->fighter_exists = false;
    this->enemy_cur_hp = 0;
    this->enemy_max_hp = 0;
};

HarvestableManager::~HarvestableManager()
{
};

void HarvestableManager::store_fighter(spFighter fighter)
{
    this->enemy_cur_hp = (int)fighter->attrs->health->current_val;
    this->enemy_max_hp = (int)fighter->attrs->health->max_val;
    this->enemy_damage = (int)fighter->attrs->damage->current_val;
    this->fighter_exists = true;

    CCLOG("stored fighter");
};

void HarvestableManager::load_fighter(spFighter fighter)
{
    fighter->attrs->health->current_val = (double)this->enemy_cur_hp;
    fighter->attrs->health->max_val = (double)this->enemy_max_hp;
    fighter->attrs->damage->current_val = (double)this->enemy_damage;

    CCLOG("loaded fighter");
};
