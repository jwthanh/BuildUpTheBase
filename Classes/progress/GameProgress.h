#ifndef GAMEPROGRESS_H
#define GAMEPROGRESS_H

#include <map>
#include <memory>

class Building;
class City;
enum class TabTypes;

using TabMap = std::map<TabTypes, bool>;
using BuildingTabMap = std::map<std::shared_ptr<Building>, TabMap>;
using BuildingUnlockMap = std::map<std::shared_ptr<Building>, bool>;


//Tracks a players progress through the game.
// Should manage which buildings and tabs are unlocked
// TODO potential to read from file or some other tool to organize and validate progress
class GameProgress
{
    private:
        static GameProgress* _instance;

        BuildingTabMap _building_tab_map;
        BuildingUnlockMap _building_unlock_map;

    public:
        static GameProgress* getInstance();

        //sets all buildings in the city to locked by default
        // will need to set the progress elsewhere I think
        void init(City* city);
        void update(float dt, City* city);

        GameProgress();

        const BuildingTabMap& get_building_tab_map() const;
        const BuildingUnlockMap& get_building_unlock_map() const;

};
#endif
