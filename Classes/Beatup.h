#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__
#include "constants.h"

#include <chrono>


class Beatup
{

    private:
        bool _visible_inventory = false;

    public:

        virtual bool init();

        Buildup* buildup;
        bool has_been_initialized = false;

        Clock* spawn_coin_clock;

        struct Defaults {
            float stamina_punch_cost;
            float stamina_regen_rate;
        } defaults;

        static std::string total_coin_key;

        res_count_t _total_coins;
        res_count_t _last_total_coins;
        void add_total_coins(res_count_t x);
        res_count_t get_total_coins();

        res_count_t get_max_coin_storage();
        res_count_t get_coin_storage_left();

        static std::string last_login_key;
        void set_last_login();
        std::chrono::duration<double, std::ratio<3600>> hours_since_last_login();

        void apply_flash_shader();

        bool tutorials_enabled();
        static std::string tutorial_id_key;
};

void test_shader_stuff(Beatup* beatup);
#endif // __HELLOWORLD_SCENE_H__
