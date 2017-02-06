#include "Achievement.h"

#include "base/CCConsole.h"


AchievementManager* AchievementManager::_instance = NULL;
const std::map<AchievementType, std::pair<std::string, std::string>> AchievementManager::info_map = {
    { AchievementType::TotalTaps, {"Total Taps", "Total harvestables tapped" }},
    { AchievementType::TotalCoins, {"Total Coins", "Total coins gathered" }},
    { AchievementType::TotalKills, {"Total Kills", "Total kills in The Arena" }},
    { AchievementType::TotalDepth, {"Total Depth", "Total depth reached digging" }},
    { AchievementType::TotalItems, {"Total Items Scavenged", "Total Items found by scavenging The Dump" }},
    { AchievementType::TotalItemsPlaced, {"Total Items Upgraded", "Total Items placed on the altar" }},
    { AchievementType::TotalRecipesCrafted, {"Total Recipes Crafted", "Total recipes crafted at The Workshop"}}
};

const std::map<BaseAchievement::SubType, std::string> BaseAchievement::type_map = {
    { AchievementType::TotalTaps, "total_taps"},
    { AchievementType::TotalCoins, "total_coins"},
    { AchievementType::TotalKills, "total_kills"},
    { AchievementType::TotalDepth, "total_depths"},
    { AchievementType::TotalItems, "total_items_scavenged"},
    { AchievementType::TotalItemsPlaced, "total_items_upgraded"},
    { AchievementType::TotalRecipesCrafted, "total_recipes_crafted"}
};

void AchievementManager::fill_achievement_cache()
{
    for (auto& type_pair : BaseAchievement::type_map)
    {
        this->getAchievement(type_pair.first);
    };
};

AchievementManager::AchievementManager()
{
    this->fill_achievement_cache();
};

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
        new_achievement = std::make_shared<CountAchievement>(achv_type, 0.0, 150.0);
    }
    else if (achv_type == AchievementType::TotalKills)
    {
        new_achievement = std::make_shared<CountAchievement>(achv_type, 0.0, 10.0);
    }
    else if (achv_type == AchievementType::TotalCoins)
    {
        new_achievement = std::make_shared<CountAchievement>(achv_type, 0.0, 100.0);
    }
    else if (achv_type == AchievementType::TotalDepth)
    {
        new_achievement = std::make_shared<CountAchievement>(achv_type, 0.0, 5.0);
    }
    else if (achv_type == AchievementType::TotalItems)
    {
        new_achievement = std::make_shared<CountAchievement>(achv_type, 0.0, 5.0);
    }
    else if (achv_type == AchievementType::TotalItemsPlaced)
    {
        new_achievement = std::make_shared<CountAchievement>(achv_type, 0.0, 5.0);
    }
    else
    {
        CCLOG("failed trying to getAchievement: unrecognized achievement type");
    }

    this->cached_achievements[achv_type] = new_achievement;

    return new_achievement;
};

std::string BaseAchievement::type_to_string(BaseAchievement::SubType type)
{
    std::string result = "none";
    for (auto pair : BaseAchievement::type_map)
    {
        if (type == pair.first) { return pair.second; }
    }
    return result;
};

BaseAchievement::SubType BaseAchievement::string_to_type(std::string string_type)
{
    BaseAchievement::SubType result = BaseAchievement::SubType::None;
    std::transform(string_type.begin(), string_type.end(), string_type.begin(), ::tolower);
    for (auto& pair : BaseAchievement::type_map)
    {
        if (pair.second == string_type)
        {
            return pair.first;
        }
    }

    assert(false && "unknown type"); return result;
};

std::vector<std::shared_ptr<BaseAchievement>> AchievementManager::getAchievements()
{
    std::vector<std::shared_ptr<BaseAchievement>> achievements;
    for (auto& type_to_ach : this->cached_achievements)
    {
        achievements.push_back(type_to_ach.second);
    };

    return achievements;
};

BaseAchievement::BaseAchievement(AchievementType achievement_type)
    : achievement_type(achievement_type),
    _completed(false), _celebrated(false), _icon_path("")
{
};

std::string BaseAchievement::get_name()
{
    return AchievementManager::info_map.at(this->achievement_type).first;
};

std::string BaseAchievement::get_description()
{
    return AchievementManager::info_map.at(this->achievement_type).second;
};

bool BaseAchievement::get_completed()
{
    return this->_completed;
};

void BaseAchievement::set_completed(bool completed)
{
    this->_completed = completed;
};

void BaseAchievement::validate_completion()
{
    this->set_completed(this->satisfied_completion());

    if (this->_celebrated == false){
        if (this->_completed == true){
            this->celebrate();
        };
    };
};

void BaseAchievement::celebrate()
{
    this->_celebrated = true;
    CCLOG("YAY COMPLETED ACHIEVEMENT!");
};

std::string BaseAchievement::get_icon_path()
{
    return this->_icon_path;
};

CountAchievement::CountAchievement(AchievementType achievement_type, res_count_t current, res_count_t target)
    : BaseAchievement(achievement_type)
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

bool CountAchievement::satisfied_completion()
{
    return this->_current_count >= this->_target_count;
};
