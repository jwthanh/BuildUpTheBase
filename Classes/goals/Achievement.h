#ifndef ACHIEVEMENT_H
#define ACHIEVEMENT_H

#include <string>

class Achievement;
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

    public:
        static AchievementManager* getInstance();
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
};
//
// ///IntAchievement checks a simple int type, like count of ingredient
// /// and saves it
// class IntAchievement : public BaseAchievement
// {
//     public:
//         void save() override;
// };
#endif
