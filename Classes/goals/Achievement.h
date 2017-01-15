#ifndef ACHIEVEMENT_H
#define ACHIEVEMENT_H

#include <string>

class Achievement;
class AchievementManager;


///Tracks all the achievements without hitting the filesystem
/// over and over again. This should control all the achievements
/// at once.
class AchievementManager
{

};

///Represents something the user does
/// * stored on a stats screen
/// * potentially unlocks something
class Achievement
{
    private:
        bool _completed;
        std::string _icon_path;

    public:
        Achievement();

        bool get_completed();
        void set_completed(bool completed);

        std::string get_icon_path();
};
#endif
