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

GameProgress::GameProgress()
{
    this->_building_tab_map = {};
}


