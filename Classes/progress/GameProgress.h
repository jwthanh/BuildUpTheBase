#ifndef GAMEPROGRESS_H
#define GAMEPROGRESS_H

#include <map>
#include <memory>

class Building;
enum class TabTypes;

using TabMap = std::map<TabTypes, bool>;
using BuildingTabMap = std::map<std::shared_ptr<Building>, TabMap>;


//Tracks a players progress through the game.
// Should manage which buildings and tabs are unlocked
// TODO potential to read from file or some other tool to organize and validate progress
class GameProgress
{
    private:
        static GameProgress* _instance;

        std::shared_ptr<BuildingTabMap> _building_tab_map;

    public:
        static GameProgress* getInstance();

        GameProgress();

        std::shared_ptr<BuildingTabMap> get_building_tab_map;

};
#endif
