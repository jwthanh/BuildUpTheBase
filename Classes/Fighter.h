#pragma once
#ifndef FIGHTER_H
#define FIGHTER_H

#include "constants.h"
#include "Nameable.h"
#include "Updateable.h"

#include "ui/CocosGUI.h"

class Experience;
class AttributeContainer;
class Village;
class Building;
class Combat;
class ProgressBar;

class Fighter : public Nameable, public Updateable
{
    public:
        enum TeamsType {
            TeamOne = 0, TeamTwo, TeamThree, TeamFour,
            TeamFive, TeamSix, TeamSeven, TeamEight,
            TeamNine, TeamTen
        } team;

        //TODO use these bools someway
        bool has_sword = false;
        bool has_armor = false;
        bool has_shield = false;

        Experience* xp;
        AttributeContainer* attrs;
        Village* city;
        Combat* combat;

        std::string sprite_name = "weapon_gauntlet.png";

        Fighter(Village* city, std::string name);
        void update(float dt);

        std::string get_stats();
};

class FighterNode : public cocos2d::ui::Widget
{
    public:
        CREATE_FUNC(FighterNode);
        static FighterNode* create(spFighter fighter);

        void update(float dt);

        void set_fighter(spFighter fighter);
        void clear_fighter();

        spFighter fighter;

        cocos2d::ui::ImageView* sprite;
        void load_new_sprite(std::string name);

        ProgressBar* hp_bar;
        ProgressBar* xp_bar;
};

class Battle : public Updateable
{
    public:
        Buildup* buildup;

        vsFighter combatants;
        vsFighter combatants_by_team(Fighter::TeamsType team);

        Battle(Buildup* buildup) : buildup(buildup), Updateable() {
            this->combatants = vsFighter();
        };

        void update(float dt) {
            Updateable::update(dt);
            this->do_battle();
        }

        void fight(spFighter left, spFighter right);
        void do_battle();
        void distribute_exp(spFighter dead_fighter);
};
#endif
