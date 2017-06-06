#include "GameProgress.h"

#include "HouseBuilding.h"

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
    for (auto& building : city->buildings)
    {
        this->_building_unlock_map[building] = false;
    }
}

//building name to its name of building that it requires to unlock
// ie The Farm needs to be level 3 before the Arena unlocks:
// {"The Farm" : "The Arena"}
const std::map<std::string, std::string> upgrade_prerequisites = {
    { "The Forest", "The Graveyard" },
    { "The Graveyard", "The Marketplace" },
    { "The Marketplace", "The Mine" },
    { "The Mine", "The Underscape" },
    { "The Underscape", "The Workshop" },
    { "The Workshop", "The Arena" },
    { "The Arena", "The Dump" },
    { "The Dump", "The Farm" },
    { "The Farm", "" }
};

void GameProgress::update(float dt, City* city)
{
    //check all the buildings against their requirements for unlocking
    for (auto& building : city->buildings)
    {
        bool is_unlocked = false;
        if (building->name == "The Farm") {
            is_unlocked = true;
        } else {
            spBuilding matched_building = city->building_by_name(upgrade_prerequisites.at(building->name));
            const int MIN_LEVEL_REQUIRED = 5;
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


