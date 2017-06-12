#include "GameProgress.h"

#include "HouseBuilding.h"
#include "enums/tab_types.h"

GameProgress* GameProgress::_instance = nullptr;

GameProgress* GameProgress::getInstance()
{
    if (GameProgress::_instance == nullptr)
    {
        GameProgress::_instance = new GameProgress();
    }

    return GameProgress::_instance;
}

void GameProgress::init(City* city)
{
    for (auto& pair : city->buildings)
    {
        spBuilding building = pair.second;
        this->_building_unlock_map[building] = false;

        auto& tab_map = this->_building_tab_map[building];

        tab_map[TabTypes::WorkerTab] = true;
        tab_map[TabTypes::DetailTab] = false;
        tab_map[TabTypes::BuildingTab] = true;
        tab_map[TabTypes::MenuTab] = true;
    }
}

//building name to its name of building that it requires to unlock
// ie The Farm needs to be level 3 before the Arena unlocks:
// {"The Farm" : "The Arena"}
const std::map<BuildingTypes, BuildingTypes> upgrade_prerequisites = {
    { BuildingTypes::TheForest, BuildingTypes::TheGraveyard },
    { BuildingTypes::TheGraveyard, BuildingTypes::TheMarketplace },
    { BuildingTypes::TheMarketplace, BuildingTypes::TheMine },
    { BuildingTypes::TheMine, BuildingTypes::TheUnderscape },
    { BuildingTypes::TheUnderscape, BuildingTypes::TheWorkshop },
    { BuildingTypes::TheWorkshop, BuildingTypes::TheArena },
    { BuildingTypes::TheArena, BuildingTypes::TheDump },
    { BuildingTypes::TheDump, BuildingTypes::TheFarm },
};

void GameProgress::update(float dt, City* city)
{
    //check all the buildings against their requirements for unlocking
    for (auto& pair : city->buildings)
    {
        spBuilding building = pair.second;
        bool is_unlocked = false;
        if (building->name == "The Farm") {
            is_unlocked = true;
        } else {
            spBuilding matched_building = city->building_by_type(upgrade_prerequisites.at(building->type));
            const int MIN_LEVEL_REQUIRED = 4;
            is_unlocked = matched_building->building_level >= MIN_LEVEL_REQUIRED;
        };

        this->_building_unlock_map[building] = is_unlocked;
    }
}

GameProgress::GameProgress()
{
    this->_building_tab_map = BuildingTabMap();
    this->_building_unlock_map = BuildingUnlockMap();
}

const BuildingTabMap& GameProgress::get_building_tab_map() const
{
    return this->_building_tab_map;
}

const BuildingUnlockMap& GameProgress::get_building_unlock_map() const
{
    return this->_building_unlock_map;
}
