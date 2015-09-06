#pragma once
#ifndef QUESTS_H
#define QUESTS_H

#include <vector>
#include <functional>
#include <string>

class Beatup;


typedef std::function<bool(Beatup*)> BoolObjFunc;

class Unlock
{
    public:
        BoolObjFunc win_func;

        std::string description;

        Unlock(){};
        Unlock(std::string description, BoolObjFunc win_func);

        std::string to_string();
};

class Objective
{
    public:
        std::string name;
        Objective(Beatup* beatup, std::string name, BoolObjFunc obj_func)
            : beatup(beatup),
            obj_func(obj_func),
            name(name)
            {};

        Beatup* beatup;

        BoolObjFunc obj_func;

        bool get_is_satisfied();
};

class Quest
{
    public:
        Beatup* beatup;

        std::vector<Objective*>* objectives;

        std::string name;
        std::string id_string;

        bool get_is_satisfied();
        bool get_is_completed();

        Unlock unlock;
        void do_unlock();

        Quest();
        static Quest* create_quest(Beatup* beatup, unsigned int level);
};

#endif
