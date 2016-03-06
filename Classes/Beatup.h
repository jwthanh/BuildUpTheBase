#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__
#include "cocos2d.h"

#include "GameLayer.h"
#include "constants.h"

#include "HouseBuilding.h"

class BaseMenu;

class Face;
class FacialFeature;
class Shield;
class Fist;
class Clock;

class Coin;
class Gore;

class PunchLog;
struct PunchLogItem;

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

        bool _visible_inventory = false;

    public:
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
        void prep_level(int level);

        void check_quest();
        void win_level();
        void lose_level();
        void hide_ui();

        bool has_been_initialized = false;
        static cocos2d::Scene* createScene();
        void onKeyPressed(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* pEvent);
        void onKeyReleased(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event *pEvent);

        virtual bool init();
        void update(float dt);
        void update_buildup(float dt);
        void update_coins(float dt);
        void reset();

        PunchLog* punch_log;

        void shake_screen(int intensity, bool left_angle);
        void shake_color(GLuint opacity);


        cocos2d::LayerColor* color_layer;

        cocos2d::Sprite* shop_lbl;
        cocos2d::Sprite* shop_banner;


        ProgressBar* progress_bar;

        int get_activation_count();

        void reset_hits();

        bool should_shake();
        Clock* shake_clock;

        int block_count;

        bool is_blocking = false;
        Clock* block_clock;
        void toggle_is_blocking();
        void set_is_blocking(bool val);

        Clock* spawn_coin_clock;

        std::vector<cocos2d::Sprite*>* slist;

        cocos2d::NodeGrid* node_grid;

        float punches_to_coin; //1 punch to 1 coin 
        int punch_count;

        void flash_color_on_face(float total_duration, cocos2d::Color3B flash_color, cocos2d::Color3B reset_color = cocos2d::Color3B::WHITE);

        void print_inventory();
        void hide_inventory();

        void set_target_building(spBuilding target);
        spBuilding get_target_building();

        void set_target_face(Face* target);
        Face* get_target_face();

        cocos2d::Node* face_fight_node;

        std::vector<Coin*>* coins;
        std::vector<Gore*>* gores;
        int coins_banked;

        std::vector<Face*>* faces;
        void cycle_next_face();
        void remove_all_faces();

        void cycle_next_building(bool reverse = false);

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

        int temp_coins;
        void add_total_coin(int x);
        int get_total_coins();
        static std::string total_coin_key;

        cocos2d::Label* title_lbl;

        void reload_resources();
        void apply_flash_shader();

        void set_background(BackgroundTypes bg_type);

        Level* level;
        Quest* quest;
        bool quest_completed;

        int temp_charges;
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

        BaseMenu* shop;
        cocos2d::Scene* shop_scene;
        void switch_to_shop();
        void switch_to_building_menu();
        void switch_to_city_menu();
        void switch_to_inventory_menu();
        void switch_to_test();
        void switch_to_character_menu(spFighter fighter);


        cocos2d::MotionStreak* motion;

        bool tutorials_enabled();
        static std::string tutorial_id_key;

        void view_army();
};

void test_shader_stuff(Beatup* beatup);
#endif // __HELLOWORLD_SCENE_H__
