#include "Tutorial.h"

#include "ui/UIText.h"
#include "ui/UILoadingBar.h"
#include "2d/CCActionProgressTimer.h"
#include "2d/CCActionEase.h"

#include "constants.h"
#include "GameLogic.h"
#include "HouseBuilding.h"
#include "2d/CCParticleExamples.h"
#include <sstream>
#include "Util.h"
#include "ui/UILayout.h"

Tutorial* Tutorial::_instance = nullptr;

Tutorial::Tutorial()
{
    //TODO serialize this
    this->_show_sidebar = true;
    this->_show_building_buttons = true;
    this->_show_player_info = true;
    this->_show_building_info = true;

    this->has_celebrated = false;
};

Tutorial* Tutorial::getInstance()
{
    if (!Tutorial::_instance)
    {
        Tutorial::_instance = new Tutorial();
    };

    return Tutorial::_instance;
}

void Tutorial::first_start(cocos2d::Node* parent)
{

    this->set_show_sidebar(false);
    this->set_show_building_buttons(false);
    this->set_show_player_info(false);
    this->set_show_building_info(false);
    this->set_show_progress_panel(false);

    BUILDUP->set_target_building(BUILDUP->city->building_by_name("The Farm"));

    //FIXME i wish there was a way to name these better
    auto tutorial_sidebar_panel = parent->getChildByName("tutorial_sidebar_panel")->getChildByName("tutorial_sidebar_panel");
    tutorial_sidebar_panel->setVisible(true);

    cocos2d::ui::Text* tutorial_lbl = dynamic_cast<cocos2d::ui::Text*>(tutorial_sidebar_panel->getChildByName("tutorial_lbl"));
    tutorial_lbl->setString("You're going to want to gather some resources.\nTap the farm and gather a few grains.");

    //progress panel
    cocos2d::ui::Layout* tutorial_progress_panel = dynamic_cast<cocos2d::ui::Layout*>(tutorial_sidebar_panel->getChildByName("progress_panel"));
    cocos2d::ui::LoadingBar* tutorial_loadingbar = dynamic_cast<cocos2d::ui::LoadingBar*>(tutorial_progress_panel->getChildByName("loading_bar"));
    cocos2d::ui::Text* tutorial_progress_lbl = dynamic_cast<cocos2d::ui::Text*>(tutorial_progress_panel->getChildByName("label"));

    auto celebrate = [this](cocos2d::Node* parent){
        if (this->has_celebrated == false){
            auto parts = cocos2d::ParticleFireworks::create();
            parts->setPosition(200, 100); //TODO fix hardcoded pos
            parent->addChild(parts);
            this->has_celebrated = true;
        };
    };

    auto check_grain = [tutorial_sidebar_panel, celebrate, tutorial_loadingbar, tutorial_progress_lbl, tutorial_lbl](float dt){
        //update progress bar
        res_count_t target_total_grain = building_storage_limit.at(1);
        res_count_t grain_count = BUILDUP->count_ingredients(Ingredient::SubType::Grain);
        res_count_t satisfied_percentage = grain_count/target_total_grain*100;
        tutorial_loadingbar->setPercent((float)satisfied_percentage);

        //update progress label
        res_count_t remaining_grain = target_total_grain - grain_count;
        std::stringstream progress_ss;
        progress_ss << beautify_double(remaining_grain) << " grain to harvest";
        tutorial_progress_lbl->setString(progress_ss.str());

        if (remaining_grain < 1) {
            //launch fireworks particle
            celebrate(tutorial_sidebar_panel);

            //change text to complete
            tutorial_lbl->setString("Complete!");
        }
    };
    tutorial_loadingbar->schedule(check_grain, SHORT_DELAY, "check_grain");
    check_grain(0);
};

bool Tutorial::get_show_sidebar()
{
    return this->_show_sidebar;
};

void Tutorial::set_show_sidebar(bool is_visible)
{
    this->_show_sidebar = is_visible;
};

bool Tutorial::get_show_building_buttons()
{
    return this->_show_building_buttons;
};

void Tutorial::set_show_building_buttons(bool is_visible)
{
    this->_show_building_buttons = is_visible;
};

bool Tutorial::get_show_player_info()
{
    return this->_show_player_info;
};

void Tutorial::set_show_player_info(bool is_visible)
{
    this->_show_player_info = is_visible;
};

bool Tutorial::get_show_progress_panel()
{
    return this->_show_progress_panel;
};

void Tutorial::set_show_progress_panel(bool is_visible)
{
    this->_show_progress_panel = is_visible;
};

bool Tutorial::get_show_building_info()
{
    return this->_show_building_info;
};

void Tutorial::set_show_building_info(bool is_visible)
{
    this->_show_building_info = is_visible;
};
