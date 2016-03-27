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

        void update(float dt);

        static GameLogic* getInstance();

        Beatup* beatup;
        Buildup* buildup;

        void add_total_harvests(int value);
        int get_total_harvests();
};

class GameDirector
{
    public:
        static void switch_to_shop();
        static void switch_to_building_menu();
        static void switch_to_city_menu();
        static void switch_to_inventory_menu();
        static void switch_to_test();
        static void switch_to_character_menu(spFighter fighter);
};

#endif
