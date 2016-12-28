#ifndef HARVESTABLEMANAGER_H
#define HARVESTABLEMANAGER_H

#include <memory>

class Fighter;
class Item;

enum class HarvestableTypes
{
    //TODO list them out, and then implement them
};

///manages metadata about the harvestables
/// like current progress through the dump
/// eventually I'd like to make it manage the
/// creating and desctruction of the nodes themselves
class HarvestableManager
{
    private:
        //fighter harvestable
        int enemy_cur_hp;//TODO make doubles, so we can get higher than 32 000 hp
        int enemy_max_hp;//TODO make doubles, so we can get higher than 32 000 hp
        int enemy_damage;//TODO make doubles, so we can get higher than 32 000 hp

        //dumpster harvestable
        std::shared_ptr<Item> stored_item;


    public:
        HarvestableManager();
        ~HarvestableManager();


        //fighter harvestable
        bool is_fighter_stored; // true if we want to load an existing fighter
        void store_fighter(std::shared_ptr<Fighter> fighter);
        void load_fighter(std::shared_ptr<Fighter> fighter);
        void reset_fighter();

        //dumpster harvestable
        bool is_item_stored;
        int stored_dumpster_clicks; //TODO use doubles for clicks
        void store_item(std::shared_ptr<Item> item);
        void load_item(std::shared_ptr<Item>& item);
        void reset_item();
};
#endif
