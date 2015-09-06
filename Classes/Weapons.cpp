#include "Weapons.h"
#include "ShopScene.h"
#include "constants.h"
#include "Beatup.h"
#include "SoundEngine.h"
#include "Face.h"
#include "Clock.h"
#include "DataManager.h"
#include "Combo.h"
#include "Util.h"
#include "FShake.h"
#include "ProgressBar.h"

USING_NS_CC;
 
Weapon::Weapon(Beatup* beatup, std::string id_key, std::string name) : Buyable(beatup, id_key), name(name)
{
};


ThrowableWeapon::ThrowableWeapon(Beatup* beatup, std::string id_key, std::string name) : Weapon(beatup, id_key, name)
{
    this->sprite = NULL;
    this->label = Label::createWithTTF("lbl", DEFAULT_FONT, 12);
    this->label->setVisible(false);

    this->cooldown = new Clock(2.5f);

    this->change_cost(999);
};

void ThrowableWeapon::init(std::string sprite_path) 
{
    //sprite setup
    this->sprite_path = sprite_path;
    this->sprite = Sprite::createWithSpriteFrameName(sprite_path);
    this->background = Sprite::createWithSpriteFrameName("weapon_select_button.png");

    this->sprite->setAnchorPoint(Vec2(0.5f, 0.5f));
    this->background->setAnchorPoint(Vec2(0.5f, 0.5f));

    float scale = sx(4);
    this->sprite->setScale(scale);
    this->background->setScale(scale);
    this->sprite->setCascadeColorEnabled(true);

    this->beatup->addChild(this->sprite, 100);
    this->beatup->addChild(this->background, 99);

    //label setup
    auto sprite_pos = this->sprite->getBoundingBox();
    this->label->setPosition(
        sprite_pos.size.width/2,
        sprite_pos.size.height/2
    );
    this->background->addChild(this->label);

    this->init_position();
    this->change_cost(50);

};


void ThrowableWeapon::do_fire()
{
    CCLOG("inner");
    Face* target_face = this->beatup->get_target_face();
    target_face->is_crit_hit = false;

    target_face->shake_clock->start_for_thres(1.0f);
    this->beatup->shake_clock->start_for_thres(1.0f);

    if (target_face->shield == NULL)
    {
        this->cooldown->start();

        //spawn gore for each point of damage
        int damage = this->get_damage();
        for (int i = 0; i < damage; i++)
        {
            Gore::spawn(target_face, this->beatup->left_fist, false);
        };

        //ripple face //FIXME too slow on phone
        // auto stop = StopGrid::create();
        // auto waves = Ripple3D::create(1.0f, Size(2.5f, 2.5f), this->beatup->get_center_pos(), 500, 5, 10);
        // this->beatup->node_grid->runAction(Sequence::create(waves, stop, NULL));

        //check if face is dead
        if (target_face->get_hits_percent() >= 1)
        {
            target_face->kill();
        };

        Vec2 origin = Director::getInstance()->getVisibleOrigin();
        Vec2 new_pos = this->beatup->get_center_pos(
                0,
                origin.y + sy(80)
                );


        //create smoke
        auto smoke_part = ParticleSmoke::create();
        smoke_part->setTotalParticles(100);
        smoke_part->setDuration(1.75);
        smoke_part->setLife(2);
        smoke_part->setPosition(new_pos);
        smoke_part->setEmissionRate(9);
        this->beatup->addChild(smoke_part);

        //create fire
        new_pos.y -= sy(80);
        auto part = ParticleFire::create();
        part->setStartSize(part->getStartSize() * sx(2));
        part->setStartSizeVar(part->getStartSizeVar() * sx(2));
        part->setScale(sx(3));
        part->setTotalParticles(50);
        part->setDuration(1.75);
        part->setLife(.75);
        part->setPosition(new_pos);
        part->setEmissionRate(5);
        this->beatup->addChild(part);

        target_face->hits_taken += damage;
    };

    target_face->destroy_shield();

    SoundEngine::play_sound(this->get_fire_mp3_path());
};

void ThrowableWeapon::change_label(std::string msg)
{
    if (this->label != NULL)
    {
        this->label->setString(msg);
    };
};

void ThrowableWeapon::update(float dt)
{
    if (this->beatup->get_level_over())
    {
        this->sprite->setVisible(false);
        this->background->setVisible(false);
    }
    else 
    {
        if (this->get_been_bought())
        {
            this->sprite->setVisible(true);
            this->background->setVisible(true);
        }
        else
        {
            this->sprite->setVisible(false);
            this->background->setVisible(false);
        };
    };

    if (this->beatup->get_total_coins() >= this->coin_cost)
    {
        if (!this->cooldown->is_active())
        {
            this->sprite->setColor(Color3B(255, 255, 255));
            this->sprite->setOpacity(255);
        }
        else
        {
            this->cooldown->update(dt);
            if (this->cooldown->passed_threshold())
            {
                this->cooldown->reset();
            };
            this->sprite->setColor(Color3B(255, 255, 255));
            this->sprite->setOpacity(255/4);
        };
    }
    else
    {
        this->sprite->setColor(Color3B(255, 0, 0));
        this->sprite->setOpacity(255/2);

    };

    // if (this->get_been_bought())
    // {
    //     this->sprite->setColor(Color3B(0, 255, 0));
    // };
};


void ThrowableWeapon::fire()
{
	Face* target_face = this->beatup->get_target_face();
	if (target_face == NULL) { return; };

    if (this->beatup->get_total_coins() >= this->coin_cost
            && !this->cooldown->is_active())
    {
        this->do_animate();

        DataManager::decr_key(Beatup::total_coin_key, this->coin_cost);
        this->beatup->update_coins_lbl();


    };
};

void ThrowableWeapon::change_cost(unsigned int new_cost)
{
    this->coin_cost = new_cost;
    std::stringstream ss;
    ss << this->coin_cost << "$";
    this->change_label(ss.str());
};

void RocketWeapon::init_position()
{
    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    Vec2 weapon_pos = Vec2(
        this->background->getContentSize().width/2 * this->background->getScaleX(),
        visibleSize.height / 2 + origin.y
    );
    this->sprite->setPosition(weapon_pos);
    this->background->setPosition(weapon_pos);
};

void RocketWeapon::do_animate()
{
    float shake_duration = 0.55f;

    FShake* fshake = FShake::actionWithDuration(shake_duration, 10.0f);
    MoveTo* move_to = MoveTo::create(0.25f, this->beatup->get_center_pos());
    CallFunc* call_func = CallFunc::create(CC_CALLBACK_0(ThrowableWeapon::do_fire, this));

    Sequence* seq = Sequence::create(
            fshake,
            move_to,
            call_func, MoveTo::create(0.1f, this->sprite->getPosition()),
            NULL
            );
    this->sprite->runAction(seq);

};

void GrenadeWeapon::do_animate()
{
    float duration = 0.30f;

    RotateBy* rotate_by = RotateBy::create(duration/4, -45.0f);
    JumpTo* jump_to = JumpTo::create(duration, this->beatup->get_center_pos(), sy(200), 1);
    CallFunc* call_func = CallFunc::create(CC_CALLBACK_0(ThrowableWeapon::do_fire, this));

    Sequence* seq = Sequence::create(
        rotate_by,
        rotate_by->reverse(),
        jump_to,
        call_func, MoveTo::create(0.1f, this->sprite->getPosition()),
        NULL
        );

    this->sprite->runAction(seq);

};

void GrenadeWeapon::init_position()
{
    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    Vec2 weapon_pos = Vec2(
        this->background->getContentSize().width/2 * this->background->getScaleX(),
        visibleSize.height / 2 + origin.y + sy(50)
    );
    this->sprite->setPosition(weapon_pos);
    this->background->setPosition(weapon_pos);
};

std::string RocketWeapon::get_fire_mp3_path()
{
    // return "sounds\\old\\doom_rocket.mp3";
    return "sounds\\new\\extra\\C_explosion_1.mp3";
};

std::string GrenadeWeapon::get_fire_mp3_path()
{
    // return "sounds\\old\\doom_rocket.mp3";
    return "sounds\\new\\extra\\C_explosion_1.mp3";
};

unsigned int RocketWeapon::get_damage()
{
    return 50;
};

unsigned int GrenadeWeapon::get_damage()
{
    return 10;
};

void FistWeapon::init_psionic(FistSprite* fs)
{
    this->shop_cost = 25;
    fs->psionic_part = ParticleSystemQuad::create("psionic.plist");
    fs->psionic_part->resetSystem();
    fs->psionic_part->setScale(0.25);

    fs->psionic_part->setPosition(fs->flames_pos);

    fs->addChild(fs->psionic_part);

    fs->psionic_part->stopSystem();

    this->r_scale = 0;
    this->g_scale = 0.5;
    this->b_scale = 1;

    this->on_activated = [this]()
    {
        Face* face = this->beatup->get_target_face();
        if (face)
        {
            //face takes longer to charge
            face->last_attack_clock->set_threshold(face->defaults.last_attack_threshold * 2);
        };
    };

    this->on_deactivated = [this]()
    {
        Face* face = this->beatup->get_target_face();
        if (face)
        {
            face->last_attack_clock->set_threshold(face->defaults.last_attack_threshold);
        }
    };
};

void FistWeapon::init_frost(FistSprite* fs)
{
    this->shop_cost = 35;

    fs->frost_part = ParticleSystemQuad::create("fist_frost.plist");
    fs->frost_part->resetSystem();
    fs->frost_part->setScale(0.125);

    fs->frost_part->setPosition(fs->flames_pos);

    fs->addChild(fs->frost_part);
    fs->frost_part->stopSystem();

    this->r_scale = 1;
    this->g_scale = 1;
    this->b_scale = 0;

    this->on_activated = [this]()
    {
        //face does less damage
        Face* face = this->beatup->get_target_face();
        if (face != NULL)
        {
            face->charge_dmg = face->defaults.charge_dmg / 2;

            //combos take 1.5x longer to regen
            for (Combo* combo : *this->beatup->combos)
            {
                float cooldown = combo->defaults.cooldown*1.5;
                combo->cooldown = cooldown;
                combo->cooldown_clock->set_threshold(combo->cooldown);
            };
        }

        for (Combo* combo : *this->beatup->combos)
        {
            auto bar = combo->cd_bar->front_timer;
            auto frost_part = ParticleSystemQuad::create("fist_frost.plist");
            frost_part->setName("frost_part");
            frost_part->resetSystem();

            frost_part->setEmissionRate(5);
            auto tint = TintTo::create(0.125f, Color3B(48, 222, 244));
            bar->runAction(tint);

            if (!combo->get_been_bought())
            {
                continue;
            }

            auto pos2 = Vec2(
                bar->getContentSize().width / 2,
                bar->getContentSize().height / 2
                );
            frost_part->setSourcePosition(Vec2(0, 0));
            frost_part->setPosition(Vec2(pos2));
            frost_part->setPosVar(Vec2(pos2 * 4 * 4)); //counter the 0.075f scale
            frost_part->setScale(0.075f);

            frost_part->setEmitterMode(ParticleSystemQuad::Mode::GRAVITY);

            combo->cd_bar->front_timer->addChild(frost_part);
        };
    };

    this->on_deactivated = [this]()
    {
        Face* face = this->beatup->get_target_face();
        if (face != NULL)
        {
            face->charge_dmg = face->defaults.charge_dmg;

            for (Combo* combo : *this->beatup->combos)
            {
                float cooldown = combo->defaults.cooldown;
                combo->cooldown = cooldown;
                combo->cooldown_clock->set_threshold(combo->cooldown);

                if (!combo->get_been_bought())
                {
                    continue;
                }

                auto bar = combo->cd_bar->front_timer;
                auto tint = TintTo::create(0.125f, Color3B::WHITE);
                bar->runAction(tint);
                auto frost_part = bar->getChildByName("frost_part");
                if (frost_part != NULL)
                {
                    frost_part->removeFromParentAndCleanup(true);
                };
            };
        }
    };
};

void FistWeapon::init_flames(FistSprite* fs)
{
    this->shop_cost = 45;
    fs->flames_part = ParticleSystemQuad::create("fist_flame.plist");
    fs->flames_part->resetSystem();
    fs->flames_part->setScale(0.20f);

    fs->flames_part->setPosition(fs->flames_pos);

    fs->addChild(fs->flames_part);

    fs->flames_part->stopSystem();

    this->r_scale = 0;
    this->g_scale = 1;
    this->b_scale = 1;

    this->on_activated = [this]()
    {
        //double dmg
        this->beatup->left_fist->punch_dmg = this->beatup->left_fist->defaults.punch_dmg*2;
        this->beatup->right_fist->punch_dmg = this->beatup->right_fist->defaults.punch_dmg*2;

        //1.5 stamina cost
        this->beatup->stamina_punch_cost = this->beatup->defaults.stamina_punch_cost * 1.5;

            auto bar = beatup->stamina_prog->front_timer;
            auto flame_part = ParticleSystemQuad::create("fist_flame.plist");
            flame_part->setName("flame_part");
            flame_part->resetSystem();

            flame_part->setEmissionRate(10);
            // bar->setColor(Color3B(48, 222, 244));


            auto pos2 = Vec2(
                bar->getContentSize().width / 2,
                bar->getContentSize().height / 2
                );
            flame_part->setSourcePosition(Vec2(0, 0));
            flame_part->setPosition(Vec2(pos2));
            flame_part->setPosVar(Vec2(pos2 * 4 * 4)); //counter the 0.075f scale
            flame_part->setScale(0.075f);

            flame_part->setEmitterMode(ParticleSystemQuad::Mode::GRAVITY);

            bar->addChild(flame_part);
    };

    this->on_deactivated = [this]()
    {
        this->beatup->left_fist->punch_dmg = this->beatup->left_fist->defaults.punch_dmg;
        this->beatup->right_fist->punch_dmg = this->beatup->right_fist->defaults.punch_dmg;

        this->beatup->stamina_punch_cost = this->beatup->defaults.stamina_punch_cost;

        auto bar = beatup->stamina_prog->front_timer;;
        auto frost_part = bar->getChildByName("flame_part");
        if (frost_part != NULL)
        {
            frost_part->removeFromParentAndCleanup(true);
        };
    };
};


void FistWeapon::init(std::initializer_list<FistSprite*> fist_sprites)
{
    for (FistSprite* fs : fist_sprites)
    {
        if (this->type == FistWeaponTypes::Flames)
        {
            this->init_flames(fs);
        }
        else if (this->type == FistWeaponTypes::Psionic)
        {
            this->init_psionic(fs);
        }
        else if (this->type == FistWeaponTypes::Frost)
        {
            this->init_frost(fs);
        }
        else
        {
            cocos2d::log("Unknown FistWeaponTypes %d", this->type);
            assert(false && "Unhandled FistWeaponType");
        };
    };
};

void FistWeapon::scale_face_color(Face* face)
{
    Color3B new_color = face->getColor();

    new_color.r -= this->r_scale;
    new_color.b -= this->g_scale;
    new_color.g -= this->b_scale;

    face->setColor(new_color);

};

void FistWeapon::update(float dt)
{
    if (this->beatup->get_level_over())
    {
        this->beatup->set_visible_weapon_button(this->type, false);
    }

};

void FistWeapon::call_on_activated()
{
    this->is_active = true;
    this->on_activated();
};

void FistWeapon::call_on_deactivated()
{
    this->is_active = false;
    this->on_deactivated();

};

WeaponButton* WeaponButton::create(std::string normal, std::string selected, std::string disabled)
{
    WeaponButton *btn = new (std::nothrow) WeaponButton;
    if (btn && btn->init(normal,selected,disabled, TextureResType::PLIST))
    {
        btn->autorelease();
        return btn;
    }
    CC_SAFE_DELETE(btn);
    return nullptr;

};

void WeaponButton::prep(Beatup* beatup, FistWeapon* fist_weapon, Vec2 pos, std::string on_activate_sound_path, std::string icon_path)
{ 
    this->beatup = beatup;
    this->fist_weapon = fist_weapon;

    this->on_activate_sound_path = on_activate_sound_path;

    this->setScale(sx(4.0));

    this->setAnchorPoint(Vec2(0.0f, 0.0f));
    this->setPosition(pos);

    this->setTitleText(fist_weapon->name);
    this->getTitleRenderer()->setScale(0.25f);
    this->setTitleColor(Color3B::BLACK);

    this->icon = Sprite::createWithSpriteFrameName(icon_path);
    // this->icon->setScale(sx(0.5f));
    this->icon->setAnchorPoint(Vec2(0.5f, 0.5f));
    this->icon->setPosition(Vec2(
        17,
        10
    ));
    this->addChild(this->icon);

    std::function<void(Ref*, ui::Widget::TouchEventType)> flames_cb = [this](Ref* ref, ui::Widget::TouchEventType event)
    {
        if (event == ui::Widget::TouchEventType::ENDED)
        {
            if (this->on_activate_sound_path != "")
            {
                SoundEngine::play_sound(this->on_activate_sound_path);
            };

            //if its inactive, activate this one, deactivate all others
            if (!this->fist_weapon->is_active)
            {
                this->fist_weapon->call_on_activated();
                this->beatup->left_fist->one_weapon_active(this->fist_weapon->type);
                this->beatup->right_fist->one_weapon_active(this->fist_weapon->type);

                auto deactivate_if_not = [this](FistWeapon* fw)
                {
                    if (fw->type != this->fist_weapon->type)
                    {
                        if (this->beatup->get_target_face())
                        {
                            this->beatup->get_target_face()->reset_color();
                        };
                        fw->call_on_deactivated();
                    };
                };

                deactivate_if_not(this->beatup->fist_flame);
                deactivate_if_not(this->beatup->fist_psionic);
                deactivate_if_not(this->beatup->fist_frost);
            }
            //if its active already, we want to disable it
            else
            {
                this->fist_weapon->call_on_deactivated();

                this->beatup->left_fist->set_weapon_active(this->fist_weapon->type, false);
                this->beatup->right_fist->set_weapon_active(this->fist_weapon->type, false);
            };

        };

    };
    this->addTouchEventListener(flames_cb);

    if (!this->fist_weapon->get_been_bought())
    {
        this->setVisible(false);
    };
};
