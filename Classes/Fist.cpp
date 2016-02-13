#include "Fist.h"

#include "Clock.h"
#include "Beatup.h"
#include "Face.h"

#include "constants.h"
#include "SoundEngine.h"
#include "Combo.h"

#include "Weapons.h"
#include "ProgressBar.h"
#include "RandomWeightMap.h"
#include "Util.h"

USING_NS_CC;


Fist::Fist(FistSprite* sprite, bool is_left)
{
    this->x = 0;
    this->y = 0;

    this->beatup = NULL;

    this->punch_clock = new Clock(0.0f);
    this->block_shake_clock = new Clock(0.0f);

    this->defaults.charge_threshold = 1.0f;
    this->charging_clock = new Clock(
        this->defaults.charge_threshold
    );

    this->is_left = is_left;

    sprite->flames_part = NULL;
    sprite->psionic_part = NULL;
    this->sprite = sprite;

    this->is_shaky_running = false;
    this->has_played_charge_sound = false;
    this->is_punching = false;

    this->punch_dmg = 3;
    this->defaults.punch_dmg = 3;
    this->defaults.scale = 4.5;

    this->spark_part = ParticleSystemQuad::create("spark.plist");
    this->spark_part->stopSystem();
    this->spark_part->setPosition(
        this->is_left ? this->sprite->getContentSize().width  : 0,
        this->sprite->getContentSize().height
    );
    this->spark_part->setScale(0.65f);
    this->sprite->addChild(spark_part);
};

void Fist::update(float dt)
{
    if (this->charging_clock->is_active())
    {
        this->charging_clock->update(dt);

        //scale the fist up to 4x based on the ratio
        auto percent = this->charging_clock->get_percentage();
        this->sprite->setScale(
            this->defaults.scale + sx(percent*4.0f)
        );

        if (is_shaky_running == false && this->charging_clock->start_time > 0.25f)
        {
            //SoundEngine::play_sound("sounds\\old\\shotgun_reload.mp3");
            SoundEngine::play_sound("sounds\\new\\shotgun_reload\\C_shotgun_reload_1.mp3");
            this->is_shaky_running = true;
            // /*  this whole bit fucks with the timers or something, punching fucks up on the phone, seems like maybe if performance gets bad this might not work as well or something
            auto shaky_small = Shaky3D::create(0.5f, Size(5, 5), 10, true);
            auto shaky_med = Shaky3D::create(0.5f, Size(15, 15), 10, true);
            auto shaky_large = Shaky3D::create(2.5f, Size(25, 25), 10, true);
            auto stop = StopGrid::create();

            auto seq = Sequence::create(shaky_small, shaky_med, shaky_large, stop, NULL);

            this->sprite->getParent()->runAction(seq); //FIXME if fist sprite isnt ever on a node grid this'll break
            // */

            //CCLOG("running action");
        }
        else if (!this->has_played_charge_sound && this->charging_clock->passed_threshold())
        {
            this->has_played_charge_sound = true;
            // SoundEngine::play_sound("sounds\\old\\ding.mp3");
            SoundEngine::play_sound("sounds\\new\\charged\\C_charged_1.mp3");
            this->beatup->shake_clock->start_for_thres(0.1f);
            do_vibrate(100);
        };
    };

    if (this->punch_clock->is_active())
    {
        this->punch_clock->update(dt);
    }
    else
    {
        this->is_punching = false;
        if (this->beatup->is_blocking || this->block_shake_clock->is_active())
        {
            this->block();
        }
        else
        {
            this->reset_pos();
        };
        this->sprite->setVisible(true);
        this->moving_sprite->setVisible(false);
        this->hit_sprite->setVisible(false);
    };

    if (this->block_shake_clock->is_active())
    {
        this->block_shake_clock->update(dt);
    };

    if (this->beatup->get_level_over()) //override all the above stuff
    {
        this->sprite->setVisible(false);
        this->moving_sprite->setVisible(false);
        this->hit_sprite->setVisible(false);
    }

};

int Fist::adj(int val)
{
    if (!this->is_left)
        return val*-1;
    else
        return val;
};

float Fist::adj(float val)
{
    if (!this->is_left)
        return val*-1.0f;
    else
        return val;
};

double Fist::adj(double val)
{
    if (!this->is_left)
        return val*-1.0;
    else
        return val;
};

int Fist::neg(int val)
{
    int result = val;
    if (!this->is_left)
    {
        if (val < 0)
        {
            result = result *-1;
        }
    }
    else {
        if (val > 0)
            result = result*-1;
    }

    return result;
};

float Fist::neg(float val)
{
    float result = val;
    if (!this->is_left)
    {
        if (val < 0)
        {
            result = result *-1.0f;
        }
    }
    else {
        if (val > 0)
            result = result*-1.0f;
    }

    return result;
};

double Fist::neg(double val)
{
    double result = val;
    if (!this->is_left)
    {
        if (val < 0)
        {
            result = result *-1.0f;
        }
    }
    else {
        if (val > 0)
            result = result*-1.0f;
    }

    return result;
};

void Fist::block()
{
   if (this->beatup->is_blocking)
   {
       float distance = sx(150);
       this->sprite->setRotation(adj(30.0));
       Vec2 new_pos = Vec2(this->defaults.x + adj(distance), this->defaults.y);
       this->sprite->setPosition(new_pos);
   }
};

void Fist::punch(bool is_charged_punch)
{

    this->beatup->buildup->target_building = this->beatup->get_target_building();

    Face* face = this->beatup->get_target_face();
	if (face == NULL)
	{
        SoundEngine::play_sound("sounds\\old\\Swoosh.mp3");
	}
    else if (face->is_crit_hit)
    {
        return;
    }

    if (this->beatup->stamina_count <= 0)
    {
        ProgressBar* stamina_prog = this->beatup->stamina_prog;
        stamina_prog->color_layer->setColor(Color3B::RED);
        stamina_prog->color_layer->setOpacity(255);

        auto fade_in = FadeIn::create(0.1f);
        auto fade_out = FadeOut::create(1.0f);
        stamina_prog->bump(0.1f, 1.15f);
        auto seq = Sequence::create(fade_in, fade_out, NULL);
        stamina_prog->color_layer->runAction(seq);

        return;
    }
    else
    {
        this->beatup->stamina_count -= this->beatup->stamina_punch_cost;

        if (is_charged_punch)
        {
            this->beatup->temp_charges += 1;
        };

    };

    this->is_punching = !this->is_punching;

    if (this->is_punching)
    {
        bool critical_hit = false;
        float punch_time = 0.20f;

        if (face != NULL && face->shield == NULL)
        {
            //scale the color of the face based on the current weapon
            //this may change the face's sprite.

            float total_punch_dmg = this->get_punch_dmg();
            //CCLOG("total %f", total_punch_dmg);

            face->spawn_dmg_lbl(total_punch_dmg);

            face->hit(total_punch_dmg);

            this->beatup->punch_count += rand() % int(total_punch_dmg);
            if (this->beatup->punch_count >= this->beatup->punches_to_coin)
            {
                Coin::spawn(face, this, true && !is_charged_punch);
                this->beatup->punch_count = 0;
            }
            else
            {
                Gore::spawn(face, this, true && !is_charged_punch);
            };

            if (is_charged_punch)
            {
                critical_hit = true; 
            }
            if (critical_hit)
            {
                ActionInterval* shaky = Shaky3D::create( punch_time, Size(25, 25), 10, true);
                auto stop_grid = StopGrid::create();
                this->beatup->node_grid->runAction(Sequence::create(shaky, stop_grid, NULL));

                punch_time *= 3; 
            };

        }
        else if (face != NULL && face->shield != NULL)
        {
            face->shield->hit(1);
        };

        this->beatup->shake_clock->start_for_thres(punch_time);
        this->punch_clock->start_for_thres(punch_time);

        float rotation = adj(15.0f);

        if (face != NULL)
        {
            face->shake_clock->start_for_thres(punch_time);
            face->get_punched(this, critical_hit, rotation);
        }

        Vec2 punch_pos = this->get_punch_pos();
        this->sprite->setVisible(false);

        this->hit_sprite->setPosition(punch_pos);
        this->hit_sprite->setVisible(false);
        this->moving_sprite->setPosition(punch_pos);
        this->moving_sprite->setVisible(false);

        if (critical_hit)
        {
            this->hit_sprite->setVisible(true);
        }
        else
        {
            this->moving_sprite->setVisible(true);
        };
    }
    else
    {
        this->reset_pos();
        // this->sprite->setVisible(true);
        // this->moving_sprite->setVisible(false);
    };

	if (face != NULL)
	{
		face->update_sprite();

		if (face->shield == NULL)
        {
            FistHands hand = this->hand;
            if (is_charged_punch)
            {
                if (this->hand == FistHands::Left)
                {
                    hand = FistHands::LCharge;
                }
                else if (this->hand == FistHands::Right)
                {
                    hand = FistHands::RCharge;
                }
                else
                {
                    assert(false && "weird hand");
                };
            };
        }
	}
};

void Fist::reset_pos()
{
    Vec2 new_pos = Vec2(this->defaults.x, this->defaults.y);

    this->sprite->setPosition(new_pos);
    this->sprite->setRotation(0);
    //TODO find better spots for the hit/moving sprites
    this->hit_sprite->setPosition(new_pos);
    this->moving_sprite->setPosition(new_pos);
};

void Fist::one_weapon_active(FistWeaponTypes weapon_type)
{

    this->set_weapon_active(weapon_type, true);

    auto deactivate_if_not = [this, weapon_type](FistWeaponTypes wt)
    {
        if (wt != weapon_type)
        {
            this->set_weapon_active(wt, false);
        };
    };

    deactivate_if_not(FistWeaponTypes::Flames);
    deactivate_if_not(FistWeaponTypes::Psionic);
    deactivate_if_not(FistWeaponTypes::Frost);
};

void Fist::toggle_weapon_active(FistWeaponTypes weapon_type)
{
    bool weapon_active = this->sprite->get_weapon_particlesys(weapon_type)->isActive();
    this->set_weapon_active(weapon_type, !weapon_active);
};

void Fist::set_weapon_active(FistWeaponTypes weapon_type, bool is_active)
{
    if (is_active)
    {
        this->sprite->get_weapon_particlesys(weapon_type)->resetSystem();
        this->moving_sprite->get_weapon_particlesys(weapon_type)->resetSystem();
        this->hit_sprite->get_weapon_particlesys(weapon_type)->resetSystem();
    }
    else
    {
        this->sprite->get_weapon_particlesys(weapon_type)->stopSystem();
        this->moving_sprite->get_weapon_particlesys(weapon_type)->stopSystem();
        this->hit_sprite->get_weapon_particlesys(weapon_type)->stopSystem();
    };
};

Vec2 Fist::get_punch_pos()
{
    int x_diff = sx(adj(50));
    Vec2 punch_pos = Vec2(
            this->sprite->getPosition().x + x_diff,
            this->sprite->getPosition().y + sy(100)
            );

    return punch_pos;
};

cocos2d::ParticleSystemQuad* FistSprite::get_weapon_particlesys(FistWeaponTypes weapon_type)

{
    if (weapon_type == FistWeaponTypes::Flames)
    {
        return this->flames_part;
    }
    else if (weapon_type == FistWeaponTypes::Psionic)
    {
        return this->psionic_part;
    }
    else if (weapon_type == FistWeaponTypes::Frost)
    {
        return this->frost_part;
    }
    else
    {
        CCLOG("Unknown weapon type");
        return NULL;
    };
};

FistSprite* FistSprite::createWithSpriteFrame(cocos2d::SpriteFrame *spriteFrame)
{
    FistSprite *sprite = new (std::nothrow) FistSprite();
    if (sprite && spriteFrame && sprite->initWithSpriteFrame(spriteFrame))
    {
        sprite->autorelease();
        return sprite;
    }
    CC_SAFE_DELETE(sprite);
    return nullptr;
};

FistSprite* FistSprite::createWithSpriteFrameName(const std::string& spriteFrameName)
{
    SpriteFrame *frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(spriteFrameName);
    
#if COCOS2D_DEBUG > 0
    char msg[256] = {0};
    sprintf(msg, "Invalid spriteFrameName: %s", spriteFrameName.c_str());
    CCASSERT(frame != nullptr, msg);
#endif
    
    return createWithSpriteFrame(frame);
}

void FistSprite::prep(Fist* fist, Vec2 flames_pos)
{
    this->fist = fist;

    this->setScale(sx(fist->defaults.scale));
    this->setVisible(false);
    
    if (this->fist->hand == FistHands::Left)
    {
        this->setFlippedX(true);
    };

    auto size = this->getContentSize();
    if (this->fist->is_left)
    {
        flames_pos.x = size.width - flames_pos.x;
    }
    flames_pos.y = size.height - flames_pos.y;
    this->flames_pos = flames_pos;

};

void Fist::speed_flames(float scale)
{
    //FIXME the defaults actually 3 times this, need to change this too
    float default_speed = 30.0;
    float default_lifespan = 3.0;
    float default_max = 150.0;

    for (FistSprite* fs : { this->sprite, this->moving_sprite, this->hit_sprite })
    {
        ParticleSystemQuad* flames = fs->flames_part;
        if (!flames)
        {
            return;
        };

        flames->setSpeed(default_speed*scale);
        flames->setLife(default_lifespan/scale);
        //fist_flame->setTotalParticles(default_max*scale);
        flames->setTotalParticles(default_max); //need to do this or else there doens't seem to be enough parts AND this resets particles
        //fist_flame->setBlendFunc(BlendFunc::ADDITIVE);

    }
};

float Fist::get_punch_dmg()
{
    enum DamagePercent  {
        Full,
        TwoThirds,
        OneThird
    };

    RandomWeightMap<DamagePercent> dmg_rwp = RandomWeightMap<DamagePercent>();
    dmg_rwp.add_item(Full, 50);
    dmg_rwp.add_item(TwoThirds, 25);
    dmg_rwp.add_item(OneThird, 25);

    float total_punch_dmg = 0;
    DamagePercent dmg_per = dmg_rwp.get_item();
    if (dmg_per == Full)
    {
        total_punch_dmg = this->punch_dmg;
    }
    else if (dmg_per == TwoThirds)
    {
        total_punch_dmg = this->punch_dmg*0.66f;
    }
    else 
    {
        total_punch_dmg = this->punch_dmg*0.33;
    };
    total_punch_dmg = round(total_punch_dmg);

    return total_punch_dmg;
};

PunchLog::PunchLog()
{
    this->log_items = new std::deque<PunchLogItem*>();
    this->punch_count = 0;
};

void PunchLog::add_punch(FistHands hand)
{
    PunchLogItem* item = new PunchLogItem(hand);
    this->log_items->push_front(item);

    this->punch_count++;

    if (this->log_items->size() > 20)
    {
        item = this->log_items->back();
        this->log_items->pop_back();
        delete item;
    };
};

PunchLogItem::PunchLogItem(FistHands hand)
{
    this->hand = hand;
};

bool PunchLog::check_one(FistHands hand, int index, std::deque<PunchLogItem*> log)
{
    return log.at(index)->hand == hand;
};

bool PunchLog::check_combo(Combo* combo)
{
    auto hand_order = combo->hand_order;
    int hand_order_size = hand_order->size();
    if (hand_order->empty() || this->log_items->size() <= (unsigned int)hand_order_size)
    {
        return false;
    };

    std::map<int, FistHands> hand_map = std::map<int, FistHands>();

    int index = 0;
    for (auto h : *hand_order)
    {
        hand_map[index++] = h;
    };

    //reverse the punch logs and take only the last few relevant ones
    std::deque<PunchLogItem*> sliced_deque = std::deque<PunchLogItem*>(this->log_items->begin(), this->log_items->begin() + hand_order_size);
    std::reverse(sliced_deque.begin(), sliced_deque.end());

    for (int idx=0; idx < hand_order_size; idx++)
    {
        bool valid_hand = this->check_one(hand_map[idx], idx, sliced_deque);
        if (!valid_hand)
        {
            return false;
        }
    };

    return true;

};

