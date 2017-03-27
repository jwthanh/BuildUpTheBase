#include "HarvestableManager.h"
#include "Fighter.h"
#include "attribute_container.h"
#include "attribute.h"
#include "HarvestScene.h"
#include "Harvestable.h"

#include "base/CCDirector.h"

HarvestableManager* HarvestableManager::_instance = NULL;

HarvestableManager::HarvestableManager()
{
    this->is_fighter_stored = false;
    this->enemy_cur_hp = 0;
    this->enemy_max_hp = 0;

    this->is_item_stored = false;
    this->stored_item = NULL;
    this->stored_dumpster_clicks = 0;

    this->queued_scavengers = 10;
    this->current_scavenger_uses = 0;
    this->max_uses_per_scavenger = 50;
};

HarvestableManager* HarvestableManager::getInstance()
{
    if (HarvestableManager::_instance == NULL) {
        HarvestableManager::_instance = new HarvestableManager();
    };

    return HarvestableManager::_instance;
}

Harvestable* HarvestableManager::get_active_harvestable()
{
    Harvestable* active_harvestable = NULL;
    cocos2d::Scene* root_scene = cocos2d::Director::getInstance()->getRunningScene();
    auto harvest_scene = dynamic_cast<HarvestScene*>(root_scene->getChildByName("HarvestScene"));
    if (harvest_scene) {
        active_harvestable = dynamic_cast<Harvestable*>(harvest_scene->getChildByName("harvestable"));
    };

    return active_harvestable;
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
