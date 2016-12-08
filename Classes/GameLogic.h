#pragma once
#ifndef GAMELOGIC_H
#define GAMELOGIC_H

#include "constants.h"
class Beatup;
class Buildup;

class PopupPanel;


///Handles the game's logic, most of it will handle here and
/// be sent to the Layers to update visual stuff as needed
//
/// The goal is to have all the logic in one set of files MVC style and 
///  the display of it somewhere else

class GameLogic
{
    protected:
        bool is_loaded;
        static GameLogic* _instance;

        bool _can_vibrate;

    public:
        GameLogic();
        bool init();

        //handles last login and passive harvesting
        void post_load();
        std::string new_player_load();
        std::string existing_player_load();

        spClock coin_save_clock;
        spClock coin_rate_per_sec_clock;
        spClock power_sell_all_cooldown;
        void update(float dt);

        static GameLogic* getInstance();

        Beatup* beatup;
        Buildup* buildup;

        static void save_all();
        static void load_all();
        static void load_all_as_cheater();

        static std::shared_ptr<PopupPanel> get_popup_panel();

        void add_total_harvests(int value);
        int get_total_harvests();

        double city_investment;
        void add_city_investment(double value);
        double get_city_investment();

        double appeasements;
        void add_appeasements(double value);
        double get_appeasements();

        void add_total_kills(int value);
        int get_total_kills();

        void set_can_vibrate(bool can_vibrate);
        bool get_can_vibrate();
};

class GameDirector
{
    public:
        static void switch_to_building_menu();
        static void switch_to_city_menu();
        static void switch_to_miner_menu();
        static void switch_to_items_menu();
        static void switch_to_item_altar_menu();
        static void switch_to_item_equip_menu();

        static void switch_to_equipment_menu();
};

#endif
