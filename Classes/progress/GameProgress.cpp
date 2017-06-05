#include "GameProgress.h"

GameProgress* GameProgress::_instance = nullptr;

GameProgress* GameProgress::getInstance()
{
    if (GameProgress::_instance == nullptr)
    {
        GameProgress::_instance = new GameProgress();
    }

    return GameProgress::_instance;
}


