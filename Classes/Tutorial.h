#ifndef TUTORIAL_H
#define TUTORIAL_H

#include <vector>
#include <string>
#include <memory>
#include <functional>

class Tutorial;
class TutorialStep;

namespace cocos2d
{
    class Node;

    namespace ui
    {
        class Button;
        class Layout;
        class LoadingBar;
        class Text;
    };
};

///class to track
/// the players progress
//  through the game and reveal
//  ui sections as it goes
class Tutorial
{
    private:
        static Tutorial* _instance;
        bool _show_sidebar;
        bool _show_building_buttons;
        bool _show_player_info;
        bool _show_progress_panel;
        bool _show_building_info;

    public:
        Tutorial();
        static Tutorial* getInstance();

        std::shared_ptr<TutorialStep> current_step;
        std::shared_ptr<TutorialStep> next_step;

        std::vector<std::shared_ptr<TutorialStep>> steps;

        void load_step(int step_index);

        //preps the game's ui for the first time
        void first_start(cocos2d::Node* parent);

        bool get_show_sidebar();
        void set_show_sidebar(bool is_visible);

        bool get_show_building_buttons();
        void set_show_building_buttons(bool is_visible);

        bool get_show_player_info();
        void set_show_player_info(bool is_visible);

        bool get_show_building_info();
        void set_show_building_info(bool is_visible);

        bool get_show_progress_panel();
        void set_show_progress_panel(bool is_visible);

        void hide_game_ui();
};

///represents a step in the tutorial, what state the UI will be in
/// ideally it'd contain the next tutorial step to load
class TutorialStep
{
    private:
        cocos2d::Node* parent;

        std::string title;
        std::string body;

        std::function<void(float)> _scheduled_func;

    public:

        cocos2d::ui::Layout* tutorial_progress_panel;
        cocos2d::ui::Text* tutorial_progress_lbl;
        cocos2d::ui::LoadingBar* tutorial_loadingbar;

        cocos2d::ui::Text* tutorial_title_lbl;
        cocos2d::ui::Text* tutorial_lbl;
        cocos2d::ui::Button* next_tutorial_step_btn;

        bool _has_celebrated; //TODO make getter/setters
        TutorialStep(cocos2d::Node* parent, std::string title, std::string body);
        //loads up nodes in the parent
        void load_step();

        int step_index;
        void set_scheduled_func(std::function<void(float)> scheduled_func);
        void run_scheduled_func();

        void celebrate();
        void reset();
};

#endif
