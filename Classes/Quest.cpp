#include "Quest.h"
#include <algorithm>

#include "cocos2d.h"

#include "Beatup.h"
#include "Combo.h"
#include "Weapons.h"
#include <cctype>
#include "DataManager.h"

USING_NS_CC;

bool obj_free(Beatup* beatup)
{
    return true;
};

bool obj_total_hits(Beatup* beatup)
{
    int minimum_hits = 25;
    return beatup->get_total_hits() > minimum_hits;
};

bool obj_total_hits_200(Beatup* beatup)
{
    int minimum_hits = 200;
    return beatup->get_total_hits() > minimum_hits;
};

bool unl_charged_fist(Beatup* beatup)
{
    DataManager::set_bool_from_data("charging_enabled", true);
    return false;
};

bool unlock_flame_fist(Beatup* beatup)
{
    return false;
};

bool unlock_frost_fist(Beatup* beatup)
{
    return false;
};

bool unlock_psionic_fist(Beatup* beatup)
{
    return false;
};

bool unlock_rocket(Beatup* beatup)
{
    return false;
};

bool unlock_grenade(Beatup* beatup)
{
    return false;
};

bool obj_block_once(Beatup* beatup)
{
    int minimum_blocks = 1;
    return beatup->block_count >= minimum_blocks;
};

bool obj_earn_coins(Beatup* beatup)
{
    int minimum_coins = 5;
    return beatup->temp_coins >= minimum_coins;
};

bool obj_charged_punches(Beatup* beatup)
{
    int charges = 3;
    return beatup->temp_charges >= charges;
};

bool Objective::get_is_satisfied()
{
    bool result =  this->obj_func(this->beatup);
    return result;
};

bool Quest::get_is_completed()
{
    return DataManager::get_bool_from_data(this->id_string.c_str(), false);
};

bool Quest::get_is_satisfied()
{
    std::vector<bool> completions;
    for (Objective* obj : *this->objectives)
    {
        bool result = obj->get_is_satisfied();
        completions.push_back(result);
    };

    return std::all_of(
        completions.begin(),
        completions.end(),
        [](bool b){return b;}
    );
};

void Quest::do_unlock()
{
    if (!this->get_is_completed())
    {
        this->unlock.win_func(this->beatup);

        CCLOG("doing unlock for %s", this->id_string.c_str());
        DataManager::set_bool_from_data(this->id_string.c_str(), true);
    }
    else
    {
        CCLOG("already unlocked %s", this->id_string.c_str());
    }
};

Quest::Quest()
{
    this->beatup = NULL;
    this->unlock = {
        "Unset unlock",
        [](Beatup*){return false; }
    };
    this->name = "Unnamed quest";
    this->id_string = "Unnamed_quest";
    this->objectives = NULL;
};

Quest* Quest::create_quest(Beatup* beatup, int level)
{
    std::vector<Objective*>* objectives = new std::vector<Objective*>();

    struct ObjectiveSetup {
        std::string name;
        BoolObjFunc obj_func;
        BoolObjFunc win_func = NULL;

        ObjectiveSetup(std::string name, BoolObjFunc obj_func, BoolObjFunc win_func = [](Beatup*){return false; }) :
            name(name), obj_func(obj_func), win_func(win_func) {};
    };

    struct QuestSetup {
        std::string quest_name;
        std::vector<ObjectiveSetup> obj_setups;
        Unlock unlock;
    };

    QuestSetup qs = {
        "Unset quest",
        {
            {},
        },
        { "Unset unlocks", [](Beatup*){return false; } }
    };

    std::vector<QuestSetup> quest_setups = {

        { //1
            "Your first challenge!",
            {
                { "Each level has an objective\n you must complete to\n unlock the next level:\n\nHit the face 25 times", obj_total_hits },
            },
            { "Unlocks your first combo",
                [=](Beatup* bu) {
                    return false;
                }
            }

        },
        { //2
            "Putting up a fight!",
            {
                { "Block one attack, by tapping\n your health bar.\n\n You'll be blocking for a short while \n before going back to fighting\n be careful", obj_block_once },
            },
            { "Unlocks the charged punch", [](Beatup* bu){return unl_charged_fist(bu); } }
        },
        { //3
            "Chain 'em together",
            {
                { "Do two combos\n\nTap the combo cooldown bar on the right\n to see the order you need to\n punch to activate a combo", 
                [](Beatup* bu){return false; } },
            },
            { "Unlocks the grenade\n (costs coins to use)", [](Beatup* bu){return unlock_grenade(bu); } }
        },
        { //4
            "Makin' money",
            {
                { "Earn 5 coins", obj_earn_coins },
            },
            { "Unlocks your second combo", [](Beatup* bu){return false; } }
        },
        { //5
            "It's heating up!",
            {
                { "Do three charged punches", obj_charged_punches }
            },
            { "Unlocks the flaming fists\n(higher damage, uses more stamina)", [](Beatup* bu){return unlock_flame_fist(bu); } }
        },
        { //6
            "Bonus!",
            {
                { "Enjoy this!", obj_free },
            },
            { "Unlocks the third combo", [](Beatup* bu){return false; } }
        },
        { //7
            "Salad Bar",
            {
                { "Hit the face 200 times", obj_total_hits_200 },
            },
            { "Unlocks the rocket launcher\n (costs more coins)", [](Beatup* bu){return unlock_rocket(bu); } }
        },
        { //8
            "Raining hellfire",
            {
                { "Earn 5 coins", obj_earn_coins },
            },
            { "Unlock the fourth combo", [](Beatup* bu){return false; } }
        },
        { //9
            "Do you feel that?",
            {
                { "Earn 5 coins", obj_earn_coins },
            },
            { "Unlocks the frozen fist\n (Blocking reduces all damage\n but slows stamina regeneration)", [](Beatup* bu){return unlock_frost_fist(bu); } }
        },
        { //10
            "It's getting cold in here",
            {
                { "Earn 5 coins", obj_earn_coins },
            },
            { "Unlocks the fifth combo", [](Beatup* bu){return false; } }
        },
        { //11
            "Feelin' weird",
            {
                { "Earn 5 coins", obj_earn_coins },
            },
            { "Unlocks the psionic fist\n (Confuses the face,\n taking longer to attack you)", [](Beatup* bu){return unlock_psionic_fist(bu); } }
        },
        { //12
            "Look at what I can do",
            {
                { "Earn 5 coins", obj_earn_coins },
            },
            { "Unlocks the sixth combo", [](Beatup* bu){return false; } }
        },
        { //13
            "Fully loaded",
            {
                { "Earn 5 coins", obj_earn_coins },
            },
            { "Unlocks the seventh combo", [](Beatup* bu){return false; } }
        },
        { //14
            "Just you and me, bub",
            {
                { "Earn 5 coins", obj_earn_coins },
            },
            { "No unlocks", [](Beatup* bu){return false; } }
        },
        { //15
            "I smell peanut butter",
            {
                { "Earn 5 coins", obj_earn_coins },
            },
            { "No unlocks", [](Beatup* bu){return false; } }
        },
        { //16
            "The longest yard",
            {
                { "Earn 5 coins", obj_earn_coins },
            },
            { "No unlocks", [](Beatup* bu){return false; } }
        },
        { //17
            "Feeling faint",
            {
                { "Earn 5 coins", obj_earn_coins },
            },
            { "No unlocks", [](Beatup* bu){return false; } }
        },
        { //18
            "Are you dead yet?",
            {
                { "Earn 5 coins", obj_earn_coins },
            },
            { "No unlocks", [](Beatup* bu){return false; } }
        },
        { //19
            "We're almost there",
            {
                { "Earn 5 coins", obj_earn_coins },
            },
            { "No unlocks", [](Beatup* bu){return false; } }
        },
        { //20
            "I just know it",
            {
                { "Earn 5 coins", obj_earn_coins },
            },
            { "No unlocks", [](Beatup* bu){return false; } }
        },
        { //21
            "FINAL BOSS?",
            {
                { "Earn 5 coins", obj_earn_coins },
            },
            { "No unlocks", [](Beatup* bu){return false; } }
        }
    };

    try {
        // CCLOG("setting up quest %d", level);
        qs = quest_setups.at(level-1);
    } 
    catch (const std::out_of_range&) {

        qs = {
            "No setup quest",
            {},
            { "No unlocks", [](Beatup*){return false;} }
        };
    }

    Quest* quest = new Quest();
    quest->name = qs.quest_name;
    std::string s = quest->name;
    s.erase(std::remove_if(s.begin(), s.end(), [](char c){ return !std::isalnum(c);}), s.end()); //remove all non alpha numeric chars
    quest->id_string = s;
    quest->unlock = qs.unlock;

    for (auto os : qs.obj_setups)
    {
        objectives->push_back(new Objective(beatup, os.name, os.obj_func));
    };
    quest->objectives = objectives;

    quest->beatup = beatup;

    return quest;
};

Unlock::Unlock(std::string description, BoolObjFunc win_func) : description(description), win_func(win_func)
{

};

std::string Unlock::to_string()
{
    std::stringstream ss;
    ss << "Unlocks: ";

    ss << this->description;

    return ss.str();
};
