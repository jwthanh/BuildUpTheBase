#include "GameProgress.h"

#include "HouseBuilding.h"
#include "enums/tab_types.h"
#include "house_building/BuildingTypes.h"

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

        TabMap& tab_map = this->_building_tab_map[building];

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
    { BuildingTypes::TheMine, BuildingTypes::TheArena },
    { BuildingTypes::TheArena, BuildingTypes::TheWorkshop },
    { BuildingTypes::TheWorkshop, BuildingTypes::TheUnderscape },
    { BuildingTypes::TheUnderscape, BuildingTypes::TheDump },
    { BuildingTypes::TheDump, BuildingTypes::TheFarm },
};

void GameProgress::update(float dt, City* city)
{
    //check all the buildings against their requirements for unlocking
    for (auto& pair : city->buildings)
    {
        spBuilding& building = pair.second;
        bool is_unlocked = false;
        if (building->name == "The Farm") {
            is_unlocked = true;
        } else {
            spBuilding matched_building = city->building_by_type(upgrade_prerequisites.at(building->type));
            const int MIN_LEVEL_REQUIRED = 4;
            is_unlocked = matched_building->get_building_level() >= MIN_LEVEL_REQUIRED;
        };

        this->_building_unlock_map[building] = is_unlocked;

        //add the unlocking conditions for specific buildings and their tabs
        if (building->type == BuildingTypes::TheWorkshop) {
            if (building->get_building_level() >= 1) {
                this->_building_tab_map[building][TabTypes::DetailTab] = true;
            };
        } else if (building->type == BuildingTypes::TheFarm) {
            if (this->_building_unlock_map[city->building_by_type(BuildingTypes::TheWorkshop)] == true) {
                this->_building_tab_map[building][TabTypes::DetailTab] = true;
            }
        } else if (building->type == BuildingTypes::TheUnderscape) {
            if (this->_building_unlock_map[city->building_by_type(BuildingTypes::TheArena)] == true) {
                this->_building_tab_map[building][TabTypes::DetailTab] = true;
            }
        } else if (building->type == BuildingTypes::TheMine) {
            if (this->_building_unlock_map[city->building_by_type(BuildingTypes::TheForest)] == true) {
                this->_building_tab_map[building][TabTypes::DetailTab] = true;
            }
        } else if (building->type == BuildingTypes::TheForest) {
            this->_building_tab_map[building][TabTypes::DetailTab] = true;
        } else if (building->type == BuildingTypes::TheGraveyard) {
            this->_building_tab_map[building][TabTypes::DetailTab] = true;
        } else if (building->type == BuildingTypes::TheMarketplace) {
            this->_building_tab_map[building][TabTypes::DetailTab] = true;
        } else if (building->type == BuildingTypes::TheArena) {
            this->_building_tab_map[building][TabTypes::DetailTab] = true;
        };
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
