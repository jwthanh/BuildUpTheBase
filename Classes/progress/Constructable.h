#ifndef CONSTRUCTABLE_H
#define CONSTRUCTABLE_H

#include <chrono>
#include <unordered_map>

#include "constants.h"

class HarvesterShopNuItem;
class SalesmanShopNuItem;
class UpgradeBuildingShopNuItem;

///represents a progress towards building an unlock, ie a new harvestable level 1
class Constructable
{
    private:

        bool _has_celebrated;
        VoidFunc _celebrate_func;

    public:
        res_count_t total_in_queue;
        //when constructable is completed
        TimePoint base_end_time;
        //base_end_time after being modified by abilities or manual
        TimePoint current_end_time;

        spBlueprint blueprint;

    Constructable(
        VoidFunc celebrate_func,
        spBlueprint blueprint);

        void init_end_time();
        void update(float dt);

        //whether now is currently passed this->current_end_time
        bool passed_threshold() const;
        //from 1-100%, get how far along the current time is to the end time
        float get_current_percent() const;

        //whether the constructable has been announced completed
        bool get_has_celebrated() const { return this->_has_celebrated; };
        void set_has_celebrated(bool has_celebrated) { this->_has_celebrated = has_celebrated; };
        //tries to call this->celebrate_func if this->get_has_celebrated() is false;
        void try_to_celebrate();
};


///oversees the constructables, responsible for checking them all each frame
class ConstructableManager
{
    private:
        static ConstructableManager* _instance;
    public:
        static ConstructableManager* getInstance();

        std::unordered_map<std::string, spConstructable> constructables;

        void update(float dt) const;

        ///returns the matching constructable, new or existing
        spConstructable add_blueprint_to_queue(spBlueprint blueprint, VoidFunc celebration_func);
        spConstructable get_constructable_from_blueprint(spBlueprint blueprint, VoidFunc celebration_func);
};

///Blueprint is a class that builds a map key id for the given object to be used with the ConstructableManager
// to track the constructables in progress. This'll reduce the risk of errors on generating unique keys
class Blueprint
{
    public:
        Duration base_duration;
        Blueprint(): base_duration(0) {};

        virtual std::string build_map_id() = 0;
};

class HarvesterShopNuItemBlueprint : public Blueprint
{
    private:
        HarvesterShopNuItem* nuitem;

    public:
        HarvesterShopNuItemBlueprint(HarvesterShopNuItem* nuitem)
        {
            this->base_duration = Duration(1);
            this->nuitem = nuitem;
        };

        std::string build_map_id() override;
};

class SalesmanShopNuItemBlueprint : public Blueprint
{
    private:
        SalesmanShopNuItem* nuitem;

    public:
        SalesmanShopNuItemBlueprint(SalesmanShopNuItem* nuitem)
        {
            this->base_duration = Duration(1);
            this->nuitem = nuitem;
        };

        std::string build_map_id() override;
};

class UpgradeBuildingShopNuItemBlueprint : public Blueprint
{
    private:
        UpgradeBuildingShopNuItem* nuitem;

    public:
        UpgradeBuildingShopNuItemBlueprint(UpgradeBuildingShopNuItem* nuitem)
        {
            this->base_duration = Duration(1);
            this->nuitem = nuitem;
        };

        std::string build_map_id() override;
};

#endif
