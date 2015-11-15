#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__
#include "cocos2d.h"

#include "GameLayer.h"
#include "constants.h"

#include "proj.win32/HouseBuilding/HouseBuilding.h"

class Shop;

class Face;
class FacialFeature;
class Shield;
class Fist;
class Clock;

class Coin;
class Gore;

class PunchLog;
struct PunchLogItem;

class Combo;

class ProgressBar;
class RocketWeapon;
class GrenadeWeapon;
class FistWeapon;

class WeaponButton;

class Quest;
class Level;


class Beatup : public GameLayer
{

    private:
        void prep_face(Face* face);
        void prep_fists();
        void prep_other();

        Face* target_face;

    public:
        cocos2d::Sprite* background;

        static cocos2d::Scene* main_menu_scene;

        Buildup* buildup;

        bool level_been_won;
        bool level_been_lost;
        bool get_level_over();
        void prep_level(int level);

        void check_quest();
        void win_level();
        void lose_level();
        void hide_ui();

        bool has_been_initialized = false;
        static cocos2d::Scene* createScene();
        void onKeyReleased(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event *pEvent);

        virtual bool init();
        void update(float dt);
        void update_buildup(float dt);
        void update_coins(float dt);
        void reset();

        PunchLog* punch_log;

        void shake_screen(int intensity);
        void shake_color(GLuint opacity);


        cocos2d::LayerColor* color_layer;

        cocos2d::Sprite* shop_lbl;
        cocos2d::Sprite* shop_banner;

        RocketWeapon* rocket;
        GrenadeWeapon* grenade;

        FistWeapon* fist_flame;
        WeaponButton* flames_button;
        FistWeapon* fist_psionic;
        WeaponButton* psionic_button;
        FistWeapon* fist_frost;
        WeaponButton* frost_button;
        void set_visible_weapon_button(FistWeaponTypes weapon_type, bool val);

        ProgressBar* progress_bar;

        bool is_comboing;
        Clock* flurry_switch_clock;
        bool flurry_with_left_hand;

        std::vector<Combo*>* combos;
        void detect_combo(FistHands hand);
        unsigned int get_activation_count();

        void reset_hits();

        bool should_shake();
        Clock* shake_clock;

        unsigned int block_count;

        bool is_blocking = false;
        Clock* block_clock;
        void toggle_is_blocking();
        void set_is_blocking(bool val);

        Clock* spawn_coin_clock;

        std::vector<cocos2d::Sprite*>* slist;

        cocos2d::NodeGrid* node_grid;

        float punches_to_coin; //1 punch to 1 coin 
        unsigned int punch_count;

        void flash_color_on_face(float total_duration, cocos2d::Color3B flash_color, cocos2d::Color3B reset_color = cocos2d::Color3B::WHITE);
        void set_target_face(Face* target);
        Face* get_target_face();

        std::vector<Coin*>* coins;
        std::vector<Gore*>* gores;
        unsigned int coins_banked;

        std::vector<Face*>* faces;
        void cycle_next_face();
        void remove_all_faces();

        cocos2d::NodeGrid* left_fist_node_grid;
        cocos2d::NodeGrid* right_fist_node_grid;
        Fist* left_fist;
        Fist* right_fist;

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

        void add_total_hit(int x);
        int get_total_hits();
        static std::string total_hit_key;

        unsigned int temp_coins;
        void add_total_coin(int x);
        unsigned get_total_coins();
        static std::string total_coin_key;

        cocos2d::Label* title_lbl;
        cocos2d::Label* coins_lbl;
        void update_coins_lbl(bool do_bump = true, unsigned int total_coins = -321);

        void reload_resources();
        void apply_flash_shader();

        void set_background(BackgroundTypes bg_type);

        Level* level;
        Quest* quest;
        bool quest_completed;

        unsigned int temp_charges;
        void create_quest_alert();

        void setup_commands();

        int player_hp;
        int player_total_hp;
        void deal_player_dmg(int dmg);

        ProgressBar* player_hp_bar;
        void update_player_hp_bar();

        // implement the "static create()" method manually
        CREATE_FUNC(Beatup);
        static cocos2d::Scene* createScene(int level);
        // static cocos2d::Scene* create_scene_level(Levels level);

        void back_to_menu();

        void onEnter();

        virtual bool onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event);
        void onTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event) ;

        void swipe_face_horizontal(float dt);
        void swipe_face_vertical(float dt);

        void onSwipeLeft(float dt);
        void onSwipeUp(float dt);
        void onSwipeRight(float dt);
        void onSwipeDown(float dt);

        Shop* shop;
        cocos2d::Scene* shop_scene;
        void switch_to_shop();

        cocos2d::MotionStreak* motion;

        bool tutorials_enabled();
        static std::string tutorial_id_key;
};

void test_shader_stuff(Beatup* beatup);
#endif // __HELLOWORLD_SCENE_H__
