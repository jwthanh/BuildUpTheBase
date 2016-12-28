#include "HarvestableManager.h"
#include "Fighter.h"
#include "attribute_container.h"
#include "attribute.h"

HarvestableManager::HarvestableManager()
{
    this->is_fighter_stored = false;
    this->enemy_cur_hp = 0;
    this->enemy_max_hp = 0;

    this->is_item_stored = false;
    this->stored_item = NULL;
    this->stored_dumpster_clicks = 0;
};

void HarvestableManager::reset_fighter()
{
    this->enemy_cur_hp = 0;
    this->enemy_max_hp = 0;
    this->enemy_damage = 0;
    this->is_fighter_stored = false;

    CCLOG("reset fighter");
};

void HarvestableManager::store_fighter(spFighter fighter)
{
    this->enemy_cur_hp = (int)fighter->attrs->health->current_val;
    this->enemy_max_hp = (int)fighter->attrs->health->max_val;
    this->enemy_damage = (int)fighter->attrs->damage->current_val;
    this->is_fighter_stored = true;

    CCLOG("stored fighter");
};

void HarvestableManager::load_fighter(spFighter fighter)
{
    fighter->attrs->health->current_val = (double)this->enemy_cur_hp;
    fighter->attrs->health->max_val = (double)this->enemy_max_hp;
    fighter->attrs->damage->current_val = (double)this->enemy_damage;

    CCLOG("loaded fighter");
};

void HarvestableManager::store_item(std::shared_ptr<Item> item)
{
    this->stored_item = item;
    this->is_item_stored = true;
};

void HarvestableManager::load_item(std::shared_ptr<Item>& item)
{
    item = this->stored_item;
};

void HarvestableManager::reset_item()
{
    this->is_item_stored = false;
    this->stored_item = NULL;
    this->stored_dumpster_clicks = 0;

    CCLOG("reset item");
};
