#ifndef TUTORIAL_H
#define TUTORIAL_H

///class to track
/// the players progress
//  through the game and reveal
//  ui sections as it goes
class Tutorial
{
    private:
        static Tutorial* _instance;

    public:
        Tutorial();
        static Tutorial* getInstance();

        bool get_show_sidebar();
        bool get_show_building_buttons();
        bool get_show_player_info();
        bool get_show_building_info();
};
#endif
