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
#include "MiscUI.h"

Tutorial* Tutorial::_instance = nullptr;

Tutorial::Tutorial()
{
    //TODO serialize this
    this->_show_sidebar = true;
    this->_show_building_buttons = true;
    this->_show_player_info = true;
    this->_show_building_info = true;

    this->current_step = NULL;
    this->next_step = NULL;

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
    this->hide_game_ui();

    //default to The Farm, just in case
    BUILDUP->set_target_building(BUILDUP->city->building_by_name("The Farm"));

    auto tutorial_sidebar_panel = parent->getChildByName("tutorial_sidebar_panel")->getChildByName("tutorial_sidebar_panel"); //FIXME i wish there was a way to name these better to reduce repetition

    //handles prepping this steps ui
    this->current_step = std::make_shared<TutorialStep>(
        tutorial_sidebar_panel,
        "Welcome to\n Build Up The Base",
        "You're going to want to gather some resources.\nTap the farm and gather a few grains."
    );

    //progress panel
    cocos2d::ui::Layout* tutorial_progress_panel = dynamic_cast<cocos2d::ui::Layout*>(tutorial_sidebar_panel->getChildByName("progress_panel"));
    cocos2d::ui::LoadingBar* tutorial_loadingbar = dynamic_cast<cocos2d::ui::LoadingBar*>(tutorial_progress_panel->getChildByName("loading_bar"));
    cocos2d::ui::Text* tutorial_progress_lbl = dynamic_cast<cocos2d::ui::Text*>(tutorial_progress_panel->getChildByName("label"));

    auto check_grain = [this, tutorial_loadingbar, tutorial_progress_lbl](float dt){
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
            //add fireworks, change text to complete, show next button etc
            this->current_step->celebrate();
        } else {
            this->current_step->reset();
        };
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

void Tutorial::hide_game_ui()
{
    this->set_show_sidebar(false);
    this->set_show_building_buttons(false);
    this->set_show_player_info(false);
    this->set_show_building_info(false);
    this->set_show_progress_panel(false);
};


TutorialStep::TutorialStep(
        cocos2d::Node* parent, std::string title, std::string body
    ) : parent(parent), title(title), body(body), _has_celebrated(false)
{
    this->init_sidebar_panel();
};

void TutorialStep::init_sidebar_panel()
{
    auto tutorial_sidebar_panel = this->parent;

    //make tutorial panel visible
    tutorial_sidebar_panel->setVisible(true);

    this->reset();
};

void TutorialStep::celebrate()
{
    if (this->_has_celebrated == false)
    {
        auto parts = cocos2d::ParticleFireworks::create();
        parts->setName("celebration_particles");
        parts->setPosition(200, 100); //TODO fix hardcoded pos
        this->parent->addChild(parts);

        //change text to complete
        cocos2d::ui::Text* tutorial_lbl = dynamic_cast<cocos2d::ui::Text*>(this->parent->getChildByName("tutorial_lbl"));
        tutorial_lbl->setString("    Complete!");

        //animate showing button
        cocos2d::ui::Button* next_tutorial_step_btn = dynamic_cast<cocos2d::ui::Button*>(this->parent->getChildByName("next_tutorial_step_btn"));
        next_tutorial_step_btn->setVisible(true);
        auto scale_to = cocos2d::ScaleTo::create(0.15f, 1.0f);
        next_tutorial_step_btn->setScale(0.0f);
        next_tutorial_step_btn->runAction(cocos2d::EaseBackOut::create(scale_to));

        this->_has_celebrated = true;
    };
};

void TutorialStep::reset()
{

    this->_has_celebrated = false;

    //setup title
    cocos2d::ui::Text* tutorial_title_lbl = dynamic_cast<cocos2d::ui::Text*>(this->parent->getChildByName("tutorial_title_lbl"));
    tutorial_title_lbl->setString(this->title);

    //setup body
    cocos2d::ui::Text* tutorial_lbl = dynamic_cast<cocos2d::ui::Text*>(this->parent->getChildByName("tutorial_lbl"));
    tutorial_lbl->setString(this->body);

    //setup buttons
    cocos2d::ui::Button* next_tutorial_step_btn = dynamic_cast<cocos2d::ui::Button*>(this->parent->getChildByName("next_tutorial_step_btn"));
    prep_button(next_tutorial_step_btn);
    next_tutorial_step_btn->setVisible(false);

    auto parts = this->parent->getChildByName("celebration_particles");
    if (parts) parts->removeFromParent();
};
