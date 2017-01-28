#include "Achievement.h"

#include "base/CCConsole.h"


AchievementManager* AchievementManager::_instance = NULL;

AchievementManager* AchievementManager::getInstance()
{
    if (AchievementManager::_instance == NULL){
        AchievementManager::_instance = new AchievementManager();
    }
    return AchievementManager::_instance;
};

BaseAchievement::BaseAchievement()
    : _completed(false), _icon_path("")
{
};

bool BaseAchievement::get_completed()
{
    return this->_completed;
};

void BaseAchievement::set_completed(bool completed)
{
    this->_completed = completed;
};

std::string BaseAchievement::get_icon_path()
{
    return this->_icon_path;
};

// void IntAchievement::save()
// {
//     CCLOG("Placeholder IntAchievement::save");
// };
