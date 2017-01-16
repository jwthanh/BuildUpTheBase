#include "Tutorial.h"

#include <sstream>

#include "ui/UIText.h"
#include "ui/UILoadingBar.h"
#include "ui/UILayout.h"
#include "2d/CCActionProgressTimer.h"
#include "2d/CCActionEase.h"
#include "2d/CCParticleExamples.h"

#include "constants.h"
#include "Beatup.h"
#include "GameLogic.h"
#include "HouseBuilding.h"
#include "Util.h"
#include "MiscUI.h"
#include "NuMenu.h"
#include "DataManager.h"

Tutorial* Tutorial::_instance = nullptr;

Tutorial::Tutorial()
{
    //TODO serialize this
    this->_show_sidebar = true;
    this->_show_building_buttons = true;
    this->_show_player_info = true;
    this->_show_building_info = true;
    this->_show_progress_panel = true;

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

void Tutorial::load_step(int step_index)
{
    std::function<bool(std::shared_ptr<TutorialStep>)> matches_step_index = [this, step_index](std::shared_ptr<TutorialStep> step)-> bool
    {
        return step->step_index == step_index && this->current_step->step_index != step_index;
    };

    auto step_it = std::find_if(this->steps.begin(), this->steps.end(), matches_step_index);

    if (step_it != this->steps.end())
    {
        (*step_it)->load_step();
        this->current_step = *step_it;
    }
    else
    {
        //assume the tutorial is over
        std::string tutorial_key = "tutorial_v1_complete";
        DataManager::set_bool_from_data(tutorial_key, true);

        CCLOG("couldn't find matching step for %i", step_index);
        this->show_game_ui();
    }
};

void Tutorial::first_start(cocos2d::Node* parent)
{
    this->hide_game_ui();

    //default to The Farm, just in case
    BUILDUP->set_target_building(BUILDUP->city->building_by_name("The Farm"));

    auto tutorial_sidebar_panel = parent->getChildByName("tutorial_sidebar_panel")->getChildByName("tutorial_sidebar_panel"); //FIXME i wish there was a way to name these better to reduce repetition

    //handles prepping this steps ui
    auto first_step = std::make_shared<TutorialStep>(
        tutorial_sidebar_panel,
        "Welcome to\n Build Up The Base",
        "You're going to want to gather some resources.\nTap the farm and gather a few grains.",
        "  Grain Harvested!"
    );
    first_step->step_index = 0;
    this->steps.push_back(first_step);

    this->current_step = first_step;


    auto check_grain = [first_step](float dt){
        //update progress bar
        res_count_t target_total_grain = building_storage_limit.at(1);
        res_count_t grain_count = BUILDUP->count_ingredients(Ingredient::SubType::Grain);
        res_count_t satisfied_percentage = grain_count/target_total_grain*100;
        first_step->tutorial_loadingbar->setPercent((float)satisfied_percentage);

        //update progress label
        res_count_t remaining_grain = target_total_grain - grain_count;
        std::stringstream progress_ss;
        res_count_t adjusted_remaining = std::max(remaining_grain, 0.0L);
        progress_ss << beautify_double(adjusted_remaining) << " grain to harvest";
        first_step->tutorial_progress_lbl->setString(progress_ss.str());

        if (remaining_grain < 1) {
            //add fireworks, change text to complete, show next button etc
            first_step->celebrate();
        } else {
            first_step->reset();
        };
    };
    first_step->set_scheduled_func(check_grain);

    //setup next_step
    auto second_step = std::make_shared<TutorialStep>(
        tutorial_sidebar_panel,
        "Makin' some money",
        "Now you've gathered some resources, it's time to spend it. Gather and sell enough grain to make 150$.\n\nTap the grain icon along the bottom and choose a quantity to sell.",
        "   You're loaded."
    );
    second_step->step_index = 1;
    this->steps.push_back(second_step);

    auto check_money = [this, second_step](float dt){
        //update progress bar
        res_count_t target_coins = 150.0;
        res_count_t total_coins = BEATUP->get_total_coins();
        res_count_t satisfied_percentage = total_coins/target_coins*100;
        second_step->tutorial_loadingbar->setPercent((float)satisfied_percentage);

        //update progress label
        res_count_t remaining_coins = target_coins - total_coins;
        std::stringstream progress_ss;
        res_count_t adjusted_remaining = std::max(remaining_coins, 0.0L);
        progress_ss << beautify_double(adjusted_remaining) << " coins to earn";
        second_step->tutorial_progress_lbl->setString(progress_ss.str());

        if (remaining_coins < 1) {
            //add fireworks, change text to complete, show next button etc
            second_step->celebrate();
        } else {
            second_step->reset();
        };
    };
    second_step->set_scheduled_func(check_money);

    std::vector<std::pair<bool Tutorial::*, bool>> switch_map;
    switch_map.push_back(
        std::make_pair<bool Tutorial::*, bool>(&Tutorial::_show_player_info, true)
    );
    second_step->set_switch_map(switch_map);

    //setup third step
    auto third_step = std::make_shared<TutorialStep>(
        tutorial_sidebar_panel,
        "Spendin' that money",
        "You're a resourceful person, better ask a friend to help you out.\n\nBuy 5 grain harvesters to harvest a little bit of grain while you wait.",
        "   You're a supervisor now."
    );
    third_step->step_index = 2;
    this->steps.push_back(third_step);

    auto target_building = BUILDUP->get_target_building();


    //typedef std::pair<Worker::SubType, Ingredient::SubType> work_ing_t;

    auto check_harvesters = [this, third_step, target_building](float dt){
        //update progress bar
        res_count_t target_count = 5.0;
        res_count_t _def = 0;
        work_ing_t one_grain_key = {Worker::SubType::One, target_building->punched_sub_type};
        res_count_t current_count = map_get(target_building->harvesters, one_grain_key, _def);
        res_count_t satisfied_percentage = current_count/target_count*100;
        third_step->tutorial_loadingbar->setPercent((float)satisfied_percentage);

        //update progress label
        res_count_t remaining_coins = target_count - current_count;
        std::stringstream progress_ss;
        res_count_t adjusted_remaining = std::max(remaining_coins, 0.0L);
        progress_ss << beautify_double(adjusted_remaining) << " harvesters to hire";
        third_step->tutorial_progress_lbl->setString(progress_ss.str());

        if (remaining_coins < 1) {
            //add fireworks, change text to complete, show next button etc
            third_step->celebrate();
        } else {
            third_step->reset();
        };
    };
    third_step->set_scheduled_func(check_harvesters);


    auto panel = cocos2d::ui::Layout::create();
    tutorial_sidebar_panel->addChild(panel);
    panel->setScale(0.5f);
    panel->setAnchorPoint(cocos2d::Vec2::ANCHOR_MIDDLE);
    panel->setPosition(tutorial_sidebar_panel->getChildByName("nuitem_pos")->getPosition());

    auto harvester_nuitem = HarvesterShopNuItem::create(panel, target_building);
    harvester_nuitem->button->setVisible(false);

    auto check_third_step = [harvester_nuitem, third_step](float dt)
    {
        auto tutorial = Tutorial::getInstance();
        bool is_third_step = tutorial->current_step->step_index == third_step->step_index;
        bool has_celebrated = third_step->_has_celebrated;
        if (is_third_step && harvester_nuitem->button->isVisible() == false && has_celebrated == false)
        {
            float orig_scale = harvester_nuitem->button->getScale();
            auto scale_to = cocos2d::ScaleTo::create(0.15f, orig_scale);
            harvester_nuitem->button->setScale(0.0f);
            harvester_nuitem->button->runAction(cocos2d::EaseBackOut::create(scale_to));
        };
        harvester_nuitem->button->setVisible(is_third_step && has_celebrated == false);
    };
    harvester_nuitem->button->schedule(check_third_step, AVERAGE_DELAY, "check_third_step");
    check_third_step(0);

    harvester_nuitem->my_init(Worker::SubType::One, target_building->punched_sub_type);


    //empty the switch map and fill it up again
    switch_map.erase(switch_map.begin(), switch_map.end());
    switch_map.push_back(
        std::make_pair<bool Tutorial::*, bool>(&Tutorial::_show_player_info, true)
    );
    third_step->set_switch_map(switch_map);

    //setup fourth step
    auto fourth_step = std::make_shared<TutorialStep>(
        tutorial_sidebar_panel,
        "Spendin' that money",
        "You're a resourceful person, better ask a friend to help you out.\n\nBuy 5 grain harvesters to harvest a little bit of grain while you wait.",
        "   You're a sales lead now."
    );
    fourth_step->step_index = 3;
    this->steps.push_back(fourth_step);


    auto check_salesmen = [this, fourth_step, target_building](float dt){
        //update progress bar
        res_count_t target_count = 5.0;
        res_count_t _def = 0;
        work_ing_t one_grain_key = {Worker::SubType::One, target_building->punched_sub_type};
        res_count_t current_count = map_get(target_building->salesmen, one_grain_key, _def);
        res_count_t satisfied_percentage = current_count/target_count*100;
        fourth_step->tutorial_loadingbar->setPercent((float)satisfied_percentage);

        //update progress label
        res_count_t remaining_coins = target_count - current_count;
        std::stringstream progress_ss;
        res_count_t adjusted_remaining = std::max(remaining_coins, 0.0L);
        progress_ss << beautify_double(adjusted_remaining) << " salesmen to hire";
        fourth_step->tutorial_progress_lbl->setString(progress_ss.str());

        if (remaining_coins < 1) {
            //add fireworks, change text to complete, show next button etc
            fourth_step->celebrate();
        } else {
            fourth_step->reset();
        };
    };
    fourth_step->set_scheduled_func(check_salesmen);


    auto salesmen_nuitem = SalesmanShopNuItem::create(panel, target_building);
    salesmen_nuitem->button->setVisible(false);
    auto check_fourth_step = [salesmen_nuitem, fourth_step](float dt)
    {
        auto tutorial = Tutorial::getInstance();
        bool is_fourth_step = tutorial->current_step->step_index == fourth_step->step_index;
        bool has_celebrated = fourth_step->_has_celebrated;
        if (is_fourth_step && salesmen_nuitem->button->isVisible() == false && has_celebrated == false)
        {
            float orig_scale = salesmen_nuitem->button->getScale();
            auto scale_to = cocos2d::ScaleTo::create(0.15f, orig_scale);
            salesmen_nuitem->button->setScale(0.0f);
            salesmen_nuitem->button->runAction(cocos2d::EaseBackOut::create(scale_to));
        };
        salesmen_nuitem->button->setVisible(is_fourth_step && has_celebrated == false);
    };
    salesmen_nuitem->schedule(check_fourth_step, AVERAGE_DELAY, "check_fourth_step");
    check_fourth_step(0);

    salesmen_nuitem->my_init(Worker::SubType::One, target_building->punched_sub_type);


    //empty the switch map and fill it up again
    switch_map.erase(switch_map.begin(), switch_map.end());
    switch_map.push_back(
        std::make_pair<bool Tutorial::*, bool>(&Tutorial::_show_player_info, true)
    );
    fourth_step->set_switch_map(switch_map);


    //setup fifth step
    auto fifth_step = std::make_shared<TutorialStep>(
        tutorial_sidebar_panel,
        "Upgradin' the building",
        "Frustrated with the 25 resource limit?.\n\nUpgrade The Farm so you can fit more grain in its storage \n\nBuy 5 grain harvesters to harvest a little bit of grain while you wait.",
        "   You're building an empire now."
    );
    fifth_step->step_index = 4;
    this->steps.push_back(fifth_step);


    auto check_building_level = [this, fifth_step, target_building](float dt){
        //update progress bar for coins required for level
        res_count_t target_level = 2.0;
        res_count_t target_coins = scale_number(10.0L, ((res_count_t)target_level)-1.0L, 10.5L);

        res_count_t total_coins = BEATUP->get_total_coins();
        res_count_t satisfied_percentage = total_coins/target_coins*100;
        fifth_step->tutorial_loadingbar->setPercent((float)satisfied_percentage);

        //update progress label
        res_count_t remaining_coins = target_coins - total_coins;
        std::stringstream progress_ss;
        res_count_t adjusted_remaining = std::max(remaining_coins, 0.0L);
        progress_ss << beautify_double(adjusted_remaining) << " coins to go";
        fifth_step->tutorial_progress_lbl->setString(progress_ss.str());

        if (remaining_coins < 1 && target_building->building_level == target_level) {
            //add fireworks, change text to complete, show next button etc
            fifth_step->celebrate();
        } else {
            fifth_step->reset();
        };
    };
    fifth_step->set_scheduled_func(check_building_level);


    auto upgrade_building_nuitem = UpgradeBuildingShopNuItem::create(panel, target_building);
    upgrade_building_nuitem->button->setVisible(false);
    auto check_fifth_step = [upgrade_building_nuitem, fifth_step](float dt)
    {
        auto tutorial = Tutorial::getInstance();
        bool is_fifth_step = tutorial->current_step->step_index == fifth_step->step_index;
        bool has_celebrated = fifth_step->_has_celebrated;
        if (is_fifth_step && upgrade_building_nuitem->button->isVisible() == false && has_celebrated == false)
        {
            float orig_scale = upgrade_building_nuitem->button->getScale();
            auto scale_to = cocos2d::ScaleTo::create(0.15f, orig_scale);
            upgrade_building_nuitem->button->setScale(0.0f);
            upgrade_building_nuitem->button->runAction(cocos2d::EaseBackOut::create(scale_to));
        };
        upgrade_building_nuitem->button->setVisible(is_fifth_step && has_celebrated == false);
    };
    upgrade_building_nuitem->schedule(check_fifth_step, AVERAGE_DELAY, "check_fifth_step");
    check_fifth_step(0);

    upgrade_building_nuitem->my_init(2);


    //empty the switch map and fill it up again
    switch_map.erase(switch_map.begin(), switch_map.end());
    switch_map.push_back(
        std::make_pair<bool Tutorial::*, bool>(&Tutorial::_show_player_info, true)
    );
    fifth_step->set_switch_map(switch_map);

    //setup last step
    auto last_step = std::make_shared<TutorialStep>(
        tutorial_sidebar_panel,
        "Build Up Your Base",
        "From here, you can add your username to compete in the Leaderboards.\n\nScavenge for Items at The Dump.\n\nDig deep into the tunnels.\n\nFight in The Arena.\n\nAnd much more!",
        "Unused"
    );
    last_step->step_index = 5;

    last_step->set_scheduled_func([last_step](float dt){
        last_step->tutorial_progress_panel->setVisible(false);
        last_step->next_tutorial_step_btn->setVisible(true);
        last_step->next_tutorial_step_btn->setTitleText("Start Game!");
    });

    //empty the switch map and fill it up again
    switch_map.erase(switch_map.begin(), switch_map.end());
    switch_map.push_back(
        std::make_pair<bool Tutorial::*, bool>(&Tutorial::_show_player_info, true)
    );
    last_step->set_switch_map(switch_map);

    this->steps.push_back(last_step);

    this->load_step(1);
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

bool Tutorial::get_show_player_hp_lbl()
{
    return this->_show_player_hp_lbl;
};

void Tutorial::set_show_player_hp_lbl(bool is_visible)
{
    this->_show_player_hp_lbl = is_visible;
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

void Tutorial::show_game_ui()
{
    this->set_show_sidebar(true);
    this->set_show_building_buttons(true);
    this->set_show_player_info(true);
    this->set_show_player_hp_lbl(true);
    this->set_show_building_info(true);
    this->set_show_progress_panel(true);
};

void Tutorial::hide_game_ui()
{
    this->set_show_sidebar(false);
    this->set_show_building_buttons(false);
    this->set_show_player_info(false);
    this->set_show_player_hp_lbl(false);
    this->set_show_building_info(false);
    this->set_show_progress_panel(false);
};


TutorialStep::TutorialStep(
        cocos2d::Node* parent, std::string title, std::string body, std::string completion_message
    ) : parent(parent), title(title), body(body),
    completion_message(completion_message), _has_celebrated(false),
    step_index(-1), _scheduled_func([](float dt){})
{
};

void TutorialStep::load_step()
{
    auto tutorial_sidebar_panel = this->parent;

    this->tutorial_title_lbl = dynamic_cast<cocos2d::ui::Text*>(tutorial_sidebar_panel->getChildByName("tutorial_title_lbl"));
    this->tutorial_lbl = dynamic_cast<cocos2d::ui::Text*>(tutorial_sidebar_panel->getChildByName("tutorial_lbl"));

    this->next_tutorial_step_btn = dynamic_cast<cocos2d::ui::Button*>(tutorial_sidebar_panel->getChildByName("next_tutorial_step_btn"));
    bind_touch_ended(
        this->next_tutorial_step_btn,
        [this](){
            auto tutorial = Tutorial::getInstance();
            tutorial->load_step(this->step_index+1);
        }
    );


    this->tutorial_progress_panel = dynamic_cast<cocos2d::ui::Layout*>(tutorial_sidebar_panel->getChildByName("progress_panel"));
    this->tutorial_progress_lbl = dynamic_cast<cocos2d::ui::Text*>(tutorial_progress_panel->getChildByName("label"));
    this->tutorial_loadingbar = dynamic_cast<cocos2d::ui::LoadingBar*>(tutorial_progress_panel->getChildByName("loading_bar"));

    //make tutorial panel visible
    tutorial_sidebar_panel->setVisible(true);

    this->reset();

    this->run_scheduled_func();
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
        this->tutorial_lbl->setString(this->completion_message);

        //animate showing button
        this->next_tutorial_step_btn->setVisible(true);
        auto scale_to = cocos2d::ScaleTo::create(0.15f, 1.0f);
        this->next_tutorial_step_btn->setScale(0.0f);
        this->next_tutorial_step_btn->runAction(cocos2d::EaseBackOut::create(scale_to));

        this->_has_celebrated = true;
    };
};

void TutorialStep::set_switch_map(std::vector<std::pair<bool Tutorial::* , bool>> switch_map)
{
    this->_switch_map = switch_map;
};

void TutorialStep::set_scheduled_func(std::function<void(float)> scheduled_func)
{
    this->_scheduled_func = scheduled_func;
};

void TutorialStep::run_scheduled_func()
{
    this->parent->unschedule("scheduled_func");

    if (!this->_scheduled_func)
    {
        CCLOG("this step %i, needs an scheduled func", this->step_index);
        return;
    };

    this->parent->schedule(this->_scheduled_func, SHORT_DELAY, "scheduled_func");
    this->_scheduled_func(0);
};

void TutorialStep::reset()
{
    this->_has_celebrated = false;

    //setup title
    this->tutorial_title_lbl->setString(this->title);
    this->tutorial_title_lbl->unscheduleAllCallbacks();

    //setup body
    this->tutorial_lbl->setString(this->body);
    this->tutorial_lbl->unscheduleAllCallbacks();

    //setup buttons
    prep_button(next_tutorial_step_btn);
    this->next_tutorial_step_btn->setVisible(false);
    this->next_tutorial_step_btn->unscheduleAllCallbacks();

    auto parts = this->parent->getChildByName("celebration_particles");
    if (parts) parts->removeFromParent();


    this->tutorial_progress_lbl->unscheduleAllCallbacks();
    this->tutorial_progress_panel->unscheduleAllCallbacks();
    this->tutorial_loadingbar->unscheduleAllCallbacks();

    //for each of the bool members of tutorial (ideally _show_player_info etc),
    // set it to the bool that matches the switch
    Tutorial* tutorial = Tutorial::getInstance();
    for (std::vector<std::pair<bool Tutorial::*, bool>>::iterator tutorial_member = this->_switch_map.begin(); tutorial_member != this->_switch_map.end(); tutorial_member++)
    {
        //bool data member on tutorial
        bool Tutorial::* tutorial_flag = tutorial_member->first;
        tutorial->*tutorial_flag = tutorial_member->second;
    };
};
