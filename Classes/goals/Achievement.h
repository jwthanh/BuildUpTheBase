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

    public:
        BaseAchievement();

        bool get_completed();
        void set_completed(bool completed);

        // virtual void save() = 0;

        std::string get_icon_path();
        virtual void increment() = 0;
};

///CountAchievement checks a simple int type, like count of ingredient
/// and saves it
class CountAchievement : public BaseAchievement
{
    private:
        res_count_t _count;

    public:
        CountAchievement();

        virtual void increment() override;
};
#endif
