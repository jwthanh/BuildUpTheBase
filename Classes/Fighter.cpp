#include "Fighter.h"
#include "attribute_container.h"
#include "attribute.h"
#include "combat.h"
#include "ProgressBar.h"
#include "FShake.h"

#include "2d/CCSprite.h"
#include "2d/CCProgressTimer.h"
#include "ui/UIImageView.h"
#include "ui/UIScale9Sprite.h"
#include <sstream>
#include "2d/CCLabel.h"

//#include "cocos2d.h"

USING_NS_CC;

Fighter::Fighter(std::string name) : Nameable(name), Updateable() {
    this->attrs = std::make_shared<AttributeContainer>();
    this->xp = std::make_shared<Experience>();
	//TODO FIXME need to init combat after ctor because shared_from_this doesnt work in ctr
    //this->combat = std::make_shared<Combat>("unnamed combat", this->shared_from_this());
	this->combat = NULL;
    this->team = TeamTwo; //default 2 for enemy
}

void Fighter::update(float dt)
{
    Updateable::update(dt);
};

std::string Fighter::get_stats()
{
    std::stringstream ss;
    ss << this->name << " ";
    double effective_dmg = this->attrs->damage->current_val;
    ss << this->attrs->health->current_val << "/" << this->attrs->health->max_val << "/" << effective_dmg;
    // for (std::string attr : this->attrs->PrettyVector())
    // {
    //     ss << attr;
    // }

    return ss.str();
}


FighterNode* FighterNode::create(spFighter fighter)
{

    FighterNode* node = FighterNode::create();
    node->setTouchEnabled(true);
    node->setSwallowTouches(false);

    node->fighter = fighter;

    node->img_view = ui::ImageView::create(fighter ? fighter->sprite_name : "townsmen8x8.png", TextureResType::PLIST);
    node->img_view->setScale(8);
    node->img_view->setPosition(Vec2(0, 100));
    dynamic_cast<cocos2d::ui::Scale9Sprite*>(node->img_view->getVirtualRenderer())->getSprite()->getTexture()->setAliasTexParameters();
    node->addChild(node->img_view);

    node->img_view->setTouchEnabled(true);
    node->img_view->setSwallowTouches(false);
    node->img_view->addTouchEventListener([node](Ref*, TouchEventType type)
    {
        if (type == ui::Widget::TouchEventType::ENDED)
        {
            //GameDirector::switch_to_character_menu(node->fighter);
        };
    });

    //hp bar
    node->hp_bar = std::make_shared<ProgressBar>("enemy_healthbar_bar.png", "enemy_healthbar_bar_white.png");
    node->hp_bar->back_timer->setVisible(false);
    node->hp_bar->setPosition(Vec2(0, 0));
    node->hp_bar->setAnchorPoint(Vec2(0.5, 0.5));
    node->hp_bar->setScale(2);
    node->hp_bar->base_node->removeFromParent();
    if (fighter != NULL) {
        node->hp_bar->set_percentage(fighter->attrs->health->get_val_percentage());
    };
    node->addChild(node->hp_bar->base_node);

    //xp bar
    node->xp_bar = std::make_shared<ProgressBar>("enemy_healthbar_bar.png", "enemy_healthbar_bar_white.png");
    node->xp_bar->back_timer->setVisible(false);
    node->xp_bar->setPosition(Vec2(0, -25));
    node->xp_bar->setAnchorPoint(Vec2(0.5, 0.5));
    node->xp_bar->setScale(2);
    node->xp_bar->base_node->removeFromParent();
    node->xp_bar->front_timer->setColor(Color3B::BLUE);
    node->addChild(node->xp_bar->base_node);

    node->setAnchorPoint(Vec2(0.5, 0.5));

    node->setName("fighter_node");

    float AVERAGE_DELAY = 0.1f;
    node->schedule(CC_CALLBACK_1(FighterNode::update, node), AVERAGE_DELAY, "fighter_node_update");

    return node;
};

std::stringstream hp_ss;
void FighterNode::update(float dt)
{
    if (this->fighter == NULL) return;

    auto hp_percentage = this->fighter->attrs->health->get_val_percentage();

    hp_ss.str("");
    hp_ss << this->fighter->attrs->health->current_val;
    this->hp_bar->lbl->setString(hp_ss.str());

    if (this->hp_bar->target_percentage != hp_percentage)
    {
        int shake_tag = 10;
        if (this->hp_bar->front_timer->getActionByTag(shake_tag) != NULL)
        {
            this->hp_bar->front_timer->getActionByTag(shake_tag)->stop();
            this->hp_bar->back_timer->getActionByTag(shake_tag+1)->stop();
            this->hp_bar->background->getActionByTag(shake_tag+2)->stop();
        }
        else if (this->hp_bar->front_timer->getActionByTag(shake_tag) == NULL)
        {
            FShake* shake = FShake::actionWithDuration(0.1f, 1.0f);
            shake->setTag(shake_tag);
            this->hp_bar->front_timer->runAction(shake);

            auto back_shake = shake->clone();
            back_shake->setTag(shake_tag + 1);
            this->hp_bar->back_timer->runAction(back_shake);

            auto bg_shake = shake->clone();
            bg_shake->setTag(shake_tag + 2);
            this->hp_bar->background->runAction(bg_shake);
        }

    }

    this->hp_bar->set_percentage(hp_percentage);
    this->xp_bar->set_percentage(this->fighter->xp->get_progress_percentage()*100);

    if (this->fighter->attrs->health->is_empty()){
        this->hp_bar->setVisible(false);
        this->xp_bar->setVisible(false);
        this->load_new_sprite("bones.png");
    };
};

void FighterNode::load_new_sprite(std::string name)
{

    this->img_view->loadTexture(name, TextureResType::PLIST);

    //set aliasing on the new texture
	cocos2d::ui::Scale9Sprite* scale9_sprite = dynamic_cast<cocos2d::ui::Scale9Sprite*>(this->img_view->getVirtualRenderer());
	Sprite* sprite = scale9_sprite->getSprite();
    sprite->getTexture()->setAliasTexParameters();
};

void FighterNode::set_fighter(spFighter fighter)
{
	this->fighter = NULL;
    this->fighter = fighter;


    this->load_new_sprite(fighter->sprite_name);


    if (fighter != NULL) {
        this->hp_bar->set_percentage(fighter->attrs->health->get_val_percentage());
        this->xp_bar->set_percentage(fighter->xp->get_progress_percentage());
    };

};

void FighterNode::clear_fighter()
{
    this->fighter = NULL;

    this->load_new_sprite("back_button.png"); //placeholder

    this->hp_bar->set_percentage(0);
    this->xp_bar->set_percentage(0);
};

vsFighter Battle::combatants_by_team(Fighter::TeamsType team)
{
    vsFighter team_combatants = vsFighter();

    for (auto fighter : this->combatants)
    {
        if (fighter->team == team)
        {
            team_combatants.push_back(fighter);
        }
    }
    return team_combatants;
};

void Battle::fight(spFighter left, spFighter right)
{
    if (left->attrs->health->current_val < 1 || right->attrs->health->current_val < 1) {
        printj("someone is dead, skipping");
        return;
    };

    std::stringstream ss;
    ss << "\tA fight! ";
    ss << left->name << " vs " << right->name;

    double& health = right->attrs->health->current_val;
    double dmg = left->attrs->damage->current_val;

    health -= dmg;

    ss << " " << right->name << " at " << right->attrs->health->current_val << " hp;" << std::endl;
    ss << " " << right->name << " dealing " << dmg << " dmg;";

    if (right->combat->is_dead())
    {
        ss << " and it died!";

        //only give exp to killer
        left->combat->give_exp(right->xp->value);
        // this->distribute_exp(right);
    }

    //printj(ss.str());
    // CCLOG("clog: %s", ss.str().c_str());
};

void Battle::do_battle()
{
    if (this->combatants.size() > 1)
    {
        vsFighter team_1 = this->combatants_by_team(Fighter::TeamOne);
        vsFighter team_2 = this->combatants_by_team(Fighter::TeamTwo);

        //team1 attacks team2
        for (auto fighter : team_1)
        {
            for (auto enemy : team_2) {
                this->fight(fighter, enemy);
            }
        }

        //team2 attacks team1 after
        for (auto fighter : team_2)
        {
            for (auto enemy : team_1) {
                this->fight(fighter, enemy);
            }
        }
    }
    else
    {
        printj1("no one to fight");
    }
    return;
}

///Give every other combatant exp
void Battle::distribute_exp(spFighter dead_fighter)
{
    float live_combatants = 0;
    for (auto fighter : this->combatants) {
        if (!fighter->combat->is_dead()) {
            live_combatants++;
        };
    };

    for (auto fighter : this->combatants) {
        if (!fighter->combat->is_dead()) {
            fighter->combat->give_exp(dead_fighter->xp->value / (int)live_combatants);

        };
    };
};
