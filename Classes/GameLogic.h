#pragma once
#ifndef GAMELOGIC_H
#define GAMELOGIC_H

#include "constants.h"
class Beatup;
class Buildup;


///Handles the game's logic, most of it will handle here and
/// be sent to the Layers to update visual stuff as needed
//
/// The goal is to have all the logic in one set of files MVC style and 
///  the display of it somewhere else

class GameLogic
{
    protected:
        static GameLogic* _instance;
    public:
        GameLogic();
        bool init();

        //handles last login and passive harvesting
        void post_load();

        Clock* coin_save_clock;
        Clock* coin_rate_per_sec_clock;
        void update(float dt);

        static GameLogic* getInstance();

        Beatup* beatup;
        Buildup* buildup;

        static void save_all();
        static void load_all();

        void add_total_harvests(int value);
        int get_total_harvests();

        void add_total_kills(int value);
        int get_total_kills();
};

class GameDirector
{
    public:
        static void switch_to_building_menu();
};

#endif
