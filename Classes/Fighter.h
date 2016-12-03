#pragma once
#ifndef FIGHTER_H
#define FIGHTER_H

#include "constants.h"
#include "Nameable.h"
#include "Updateable.h"
#include "ui/UIWidget.h"

class Experience;
class AttributeContainer;
class Village;
class Building;
class Combat;
class ProgressBar;

namespace cocos2d
{
    namespace ui
    {
        class ImageView;
    }
}

class Fighter : public Nameable, public Updateable, public std::enable_shared_from_this<Fighter>
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

        spExperience xp;
        spAttributeContainer attrs;
        spVillage city;
        spCombat combat;

        std::string sprite_name = "weapon_gauntlet.png";

        Fighter(std::string name);
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

        cocos2d::ui::ImageView* img_view;
        void load_new_sprite(std::string name);

        spProgressBar hp_bar;
        spProgressBar xp_bar;
};

class Battle : public Updateable
{
    public:
        vsFighter combatants;
        vsFighter combatants_by_team(Fighter::TeamsType team);

        Battle() : Updateable() {
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
