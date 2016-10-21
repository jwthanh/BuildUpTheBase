#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__
#include "cocos2d.h"

#include "GameLayer.h"
#include "constants.h"

#include <chrono>

class Beatup : public GameLayer
{

    private:
        bool _visible_inventory = false;

    public:
        CREATE_FUNC(Beatup);
        static cocos2d::Scene* createScene();
        static cocos2d::Scene* createScene(int level);

        void onKeyPressed(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* pEvent);
        void onKeyReleased(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event *pEvent);

        virtual bool init();

        std::map<cocos2d::EventKeyboard::KeyCode, bool> key_state;
        cocos2d::Sprite* background;

        static cocos2d::Scene* main_menu_scene;

        Buildup* buildup;

        // vsFighter enemy_nodes;
        FighterNode* enemy_node;

        FighterNode* fighter_node;
        FighterNode* brawler_node;

        bool level_been_won;
        bool level_been_lost;
        bool get_level_over();

        void win_level();
        void hide_ui();

        bool has_been_initialized = false;
        void update_buildup(float dt);
        void reset();

        void shake_screen(int intensity, bool left_angle);
        void shake_color(GLuint opacity);


        cocos2d::LayerColor* color_layer;


        ProgressBar* progress_bar;

        bool should_shake();
        Clock* shake_clock;

        Clock* spawn_coin_clock;

        cocos2d::NodeGrid* node_grid;

        void set_target_building(spBuilding target);
        spBuilding get_target_building();

        cocos2d::Node* face_fight_node;

        void cycle_next_building(bool reverse = false);

        Clock* stamina_clock;
        ProgressBar* stamina_prog;
        float stamina_regen_rate;
        float stamina_punch_cost;
        float stamina_count;
        float stamina_max;
        void add_to_stamina(float val);

        struct Defaults {
            float stamina_punch_cost;
            float stamina_regen_rate;
        } defaults;

        static std::string total_hit_key;
        void add_total_hit(int x);
        int get_total_hits();

        res_count_t _total_coins;
        res_count_t _last_total_coins;
        static std::string total_coin_key;
        void add_total_coin(res_count_t x);
        double get_total_coins();

        static std::string last_login_key;
        void set_last_login();
        std::chrono::duration<double, std::ratio<3600>> hours_since_last_login();

        void reload_resources();
        void apply_flash_shader();

        void setup_commands();

        int player_hp;
        int player_total_hp;
        void deal_player_dmg(int dmg);

        void back_to_menu();

        void onEnter();

        cocos2d::MotionStreak* motion;

        bool tutorials_enabled();
        static std::string tutorial_id_key;

        void view_army();
};

void test_shader_stuff(Beatup* beatup);
#endif // __HELLOWORLD_SCENE_H__
