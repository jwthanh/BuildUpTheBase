#include "Achievement.h"

#include "base/CCConsole.h"


AchievementManager* AchievementManager::_instance = NULL;

AchievementManager* AchievementManager::getInstance()
{
    if (AchievementManager::_instance == NULL){
        AchievementManager::_instance = new AchievementManager();
    }
    return AchievementManager::_instance;
}

std::shared_ptr<BaseAchievement> AchievementManager::getAchievement(AchievementType achv_type)
{
    cache_map_t::const_iterator cached = this->cached_achievements.find(achv_type);
    if (cached != this->cached_achievements.end())
    {
        return cached->second;
    }

    std::shared_ptr<BaseAchievement> new_achievement = NULL;
    if (achv_type == AchievementType::TotalTaps)
    {
        new_achievement = std::make_shared<CountAchievement>(0.0, 15.0);
    }
    else if (achv_type == AchievementType::TotalKills)
    {
        new_achievement = std::make_shared<CountAchievement>(0.0, 2.0);
    }
    else if (achv_type == AchievementType::TotalCoins)
    {
        new_achievement = std::make_shared<CountAchievement>(0.0, 1.0);
    }
    else if (achv_type == AchievementType::TotalDepth)
    {
        new_achievement = std::make_shared<CountAchievement>(0.0, 5.0);
    }
    else if (achv_type == AchievementType::TotalItems)
    {
        new_achievement = std::make_shared<CountAchievement>(0.0, 5.0);
    }
    else if (achv_type == AchievementType::TotalItemsPlaced)
    {
        new_achievement = std::make_shared<CountAchievement>(0.0, 5.0);
    }
    else
    {
        CCLOG("failed trying to getAchievement: unrecognized achievement type");
    }

    this->cached_achievements[achv_type] = new_achievement;

    return new_achievement;
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

CountAchievement::CountAchievement(res_count_t current, res_count_t target)
{
    //TODO load from file
    this->_current_count = current;
    this->_target_count = target;
};

void CountAchievement::do_increment()
{
    this->do_increment_by_n(1.0);
};

void CountAchievement::do_increment_by_n(long double n)
{
    this->_current_count += n;
};

void CountAchievement::validate_completion()
{
    this->set_completed(this->_current_count >= this->_target_count);
};
