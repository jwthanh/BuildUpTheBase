#ifndef ACHIEVEMENT_H
#define ACHIEVEMENT_H

#include <string>
#include <memory>

#include "constants.h"

class BaseAchievement;
class AchievementManager;


enum class AchievementType {
    TotalTaps = 0
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

    public:
        static AchievementManager* getInstance();
        std::shared_ptr<BaseAchievement> getAchievement(AchievementType achv_type);
};

///Represents something the user does
/// * stored on a stats screen
/// * potentially unlocks something
class BaseAchievement
{
    private:
        bool _completed;
        std::string _icon_path;

        //actual incrementing function called by ::increment
        virtual void do_increment() = 0;

    public:
        BaseAchievement();

        //boolean properties
        bool get_completed();
        void set_completed(bool completed);

        ///check and update _completed
        virtual void validate_completion() = 0;

        std::string get_icon_path();
        void increment() { this->do_increment(); this->validate_completion(); };
};

///CountAchievement checks a simple int type, like count of ingredient
/// and saves it
class CountAchievement : public BaseAchievement
{
    private:
        res_count_t _current_count;
        res_count_t _target_count;

        virtual void do_increment() override;

    public:
        CountAchievement(res_count_t current, res_count_t target);
        virtual void validate_completion() override;

};
#endif
