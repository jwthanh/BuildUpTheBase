#ifndef ACHIEVEMENT_H
#define ACHIEVEMENT_H

#include <string>
#include <memory>

#include "constants.h"

class BaseAchievement;
class AchievementManager;


enum class AchievementType {
    None = -1,
    TotalTaps = 0,
    TotalCoins = 1,
    TotalKills = 2,
    TotalDepth = 3,
    TotalItems = 4,
    TotalItemsPlaced = 5,
    TotalHealthRegenerated = 6,
    TotalUndeadScavenging = 7,
    TotalCityInvestments = 8,
    TotalRecipesCrafted = 9,
};


///Tracks all the achievements without hitting the filesystem
/// over and over again. This should control all the achievements
/// at once.
class AchievementManager
{
    protected:
        static AchievementManager* _instance;

        using cache_map_t = std::map<AchievementType, std::shared_ptr<BaseAchievement>>;
        cache_map_t cached_achievements;

        //iterates over infomap and creates the achievement instances
        void fill_achievement_cache();

    public:
        static const std::map<AchievementType, std::pair<std::string, std::string>> info_map;
        AchievementManager();
        static AchievementManager* getInstance();

        std::shared_ptr<BaseAchievement> getAchievement(AchievementType achv_type);
        std::vector<std::shared_ptr<BaseAchievement>> getAchievements();
};

///Represents something the user does
/// * stored on a stats screen
/// * potentially unlocks something
class BaseAchievement
{
    protected:
        bool _completed;
        bool _celebrated;
        std::string _icon_path;

        //actual incrementing function called by ::increment
        virtual void do_increment() = 0;
        virtual void do_increment_by_n(res_count_t n) = 0;

    public:
        using SubType = AchievementType;
        static const std::map<SubType, std::string> type_map;
        static std::string type_to_string(BaseAchievement::SubType type);
        static BaseAchievement::SubType string_to_type(std::string string_type);

        const AchievementType achievement_type;
        BaseAchievement(AchievementType achievement_type);

        //boolean properties
        virtual bool satisfied_completion() = 0;
        bool get_completed();
        void set_completed(bool completed);
        void celebrate();

        std::string get_name();
        std::string get_description();

        ///check and update _completed
        virtual void validate_completion();

        std::string get_icon_path();
        void increment() { this->do_increment(); this->validate_completion(); };
        void increment_by_n(res_count_t n) { this->do_increment_by_n(n); this->validate_completion(); };
};

///CountAchievement checks a simple int type, like count of ingredient
/// and saves it
class CountAchievement : public BaseAchievement
{
    protected:
        res_count_t _current_count;
        res_count_t _target_count;

        virtual void do_increment() override;
        virtual void do_increment_by_n(res_count_t n) override;

    public:
        CountAchievement(AchievementType achievement_type, res_count_t current, res_count_t target);
        virtual bool satisfied_completion() override;

        res_count_t get_current_count() { return _current_count; };
        res_count_t get_target_count() { return _target_count; };

        friend class AchievementSerializer;

};
#endif
