#include "Fist.h"

#include "Face.h"
#include "Beatup.h"
#include "SoundEngine.h"
#include "Clock.h"

#include "ProgressBar.h"
#include "RandomWeightMap.h"

#include "ShatterNode.h"

#include "Quest.h"
#include "Util.h"
#include "Level.h"

USING_NS_CC;

bool Coin::_has_spin_anim = false;
RepeatForever* Coin::_spin_animation = NULL;

ValueMap Coin::particle_map = ValueMap();
ValueMap Gore::particle_map = ValueMap();

//bool Gore::_has_blood_parts = false;
//ParticleSystemQuad* Gore::__blood_parts = NULL;

bool Shield::first_time_human = true;
bool Shield::first_time_brick = true;

float Hittable::get_hits_percent()
{
    return float(this->hits_taken) / float(this->hits_to_kill);

};

cocos2d::Rect Face::get_swipable_rect()
{

    Sprite* target_sprite;
    if (this->has_shield_active())
    {
        target_sprite = this->shield->get_sprite();
    }
    else
    {
        target_sprite = this->get_sprite();
    };

    Rect rect = target_sprite->getBoundingBox();
    rect.origin = this->getPosition();
    rect.origin.x -= rect.size.width / 2 * this->getScaleX();
    rect.origin.y -= rect.size.height / 2 * this->getScaleY();
    rect.size.width *= this->getScaleX();
    rect.size.height *= this->getScaleY();

    // DEBUG
    // LayerColor* lc = LayerColor::create(Color4B::RED);
    // lc->setContentSize(rect.size);
    // lc->setPosition(rect.origin);
    // this->beatup->addChild(lc);

    return rect;
};

bool Face::take_damage(int damage)
{
    // this->beatup->flash_color_on_face(0.05f, Color3B(255, 119, 119)); //off-red
    //CCLOG("%d take damage", damage);
    if (damage == 3 || damage == 6) //FIXME hardcoded for 3 max base damage, and 6 after flames
    {
        //CCLOG("flash");
        this->beatup->flash_color_on_face(0.05f, Color3B::WHITE);
    };

    this->hits_taken += damage;
    this->beatup->add_total_hit(1);

    // if (this->hits_taken % 2 == 0)
    if (this->hits_taken % 75 == 0)
    {
        this->add_shield();
    };

    if (this->get_hits_percent() >= 1)
    {
        this->kill();
        return false; //cant hit anymore its dead
    };

    return true;

};

bool Face::hit(int punch_dmg)
{
    do_vibrate(200);

    auto new_path = [](std::string new_path) {
        return "sounds\\new\\"+new_path;
    };
    auto old_path = [](std::string old_path) {
        return "sounds\\old\\"+old_path;
    };
    std::vector<std::string> fist_path = {
        new_path("real_punch\\C_punch+1.mp3"),
        new_path("real_punch\\C_punch+2.mp3"),
        new_path("real_punch\\C_punch+3.mp3"),
        // new_path("real_punch\\C_combo_1.mp3"), //bell ringing?
        new_path("real_punch\\C_punch_11.mp3"),
        new_path("real_punch\\C_punch_21.mp3"),
        new_path("real_punch\\C_punch_22.mp3"),
        new_path("real_punch\\C_punch_23.mp3"),
        new_path("real_punch\\C_punch_24.mp3"),
        new_path("real_punch\\C_punch_25.mp3"),
        //new_path("real_punch\\C_punch_26.mp3"), //weird
        new_path("real_punch\\C_punch_27.mp3"),
        new_path("real_punch\\C_punch_28.mp3"),
        new_path("real_punch\\C_punch_31.mp3")
    };

    if (is_crit_hit)
    {
        this->hit_sound_path = "sounds\\old\\real_punch.mp3";
    }
    else
    {
        std::vector<std::string> hit_paths = {
            new_path("dsnoway\\C_Hit_18.mp3"),
            new_path("dsnoway\\C_Hit_11.mp3"),
            new_path("dsnoway\\C_Hit_12.mp3"),
            new_path("dsnoway\\C_Hit_13.mp3"),
            new_path("dsnoway\\C_Hit_14.mp3"),
            new_path("dsnoway\\C_Hit_15.mp3"),
            new_path("dsnoway\\C_Hit_16.mp3"),
            new_path("dsnoway\\C_Hit_17.mp3")
        };
        this->hit_sound_path = pick_one(hit_paths);

    };

    SoundEngine::play_sound(this->hit_sound_path); //face
    SoundEngine::play_sound(pick_one(fist_path)); //fist 

    int total_damage = punch_dmg;
    this->take_damage(total_damage);

    return true; //can still hit
};

void Face::start_charging_attack()
{
    // log("Starting to charge up an attack");
    this->charge_clock->reset();

    if (!this->charging_part->isActive())
    {
        // cocos2d::log("started charging");
        this->charging_part->resetSystem();
        SoundEngine::play_sound("sounds\\old\\woop.mp3");
        // SoundEngine::play_sound("sounds\\new\\woop\\C_woop_1.mp3");
    }
    else
    {
        // cocos2d::log("cant start charhing");
    };

};

void Face::end_charge_attack()
{
    this->charging_part->stopSystem();
    // log("...I am doing the actual attack");
    this->charge_clock->reset();
    this->last_attack_clock->reset();

};

void Face::do_charge_attack()
{
    // log("...I am done attacking");
    int vibrate_ms;
    if (!this->beatup->is_blocking)
    {
        SoundEngine::play_sound("sounds\\old\\barrel.mp3");
        this->beatup->shake_clock->set_threshold(1.0f);
        this->beatup->shake_clock->start();
        this->beatup->deal_player_dmg(this->charge_dmg);

        vibrate_ms = 400;
    }
    else
    {
        this->beatup->deal_player_dmg(this->charge_dmg/2.0);
        this->beatup->left_fist->spark_part->resetSystem();
        this->beatup->right_fist->spark_part->resetSystem();
        vibrate_ms = 100;
        this->beatup->shake_clock->set_threshold(0.1f);
        this->beatup->shake_clock->start();
        SoundEngine::play_sound("sounds\\new\\block\\C_block+1.mp3");

        this->beatup->block_count += 1;


        //FIXME this causes lag. changing the Size to (5, 5) made no difference
        // auto stop1 = StopGrid::create();
        // auto liquid1 = Liquid::create(0.25f, Size(100, 100), 1, 5);
        // this->beatup->left_fist_node_grid->runAction(Sequence::create(liquid1, stop1, NULL));

        // auto stop2 = StopGrid::create();
        // auto liquid2 = Liquid::create(0.25f, Size(100, 100), 1, 5);
        // this->beatup->right_fist_node_grid->runAction(Sequence::create(liquid2, stop2, NULL));

        this->beatup->left_fist->block_shake_clock->start_for_thres(0.5f);
        this->beatup->right_fist->block_shake_clock->start_for_thres(0.5f);
        this->beatup->set_is_blocking(false);
    };

    do_vibrate(vibrate_ms);
    this->end_charge_attack();
};

bool Face::long_enough_since_last_attack()
{
    return this->last_attack_clock->passed_threshold();
};

bool Face::get_percent_charged(float percent)
{
    auto result = this->charge_clock->get_percentage();
    return result > percent;
};

bool Face::is_fully_charged()
{
    return this->charge_clock->passed_threshold();
};

bool Face::has_shield_active()
{
    return this->shield != NULL;
};

void Face::add_shield()
{
    if (this->shield == NULL)
    {
        this->shield = new Shield();
        this->shield->face = this;
        this->shield->hits_to_kill = 30;

        float type_chance = CCRANDOM_0_1();
        if (type_chance < 0.75f)
        {
            shield->prep_human();
        }
        else
        {
            shield->prep_brickwall();
        };
    };
};

void Face::destroy_shield()
{
    if (this->has_shield_active())
    {
        Node* raw_s = this->getChildByName("brick_shield_sprite");
        //brick shield
        if (raw_s != NULL)
        {
            ShatterSprite* s = dynamic_cast<ShatterSprite*>(raw_s);
            s->runAction(ShatterAction::create(4));
        }
        //human shield
        else
        {
            for (int i = 0; i < 50; i++) { Gore::spawn(this, this->beatup->left_fist, false); }
            CCLOG("shatter sprite is null");
        };
        this->shield->play_destroy();
        this->remove_shield();
    };
};

void Face::disable_shield()
{
    if (this->has_shield_active())
    {
        this->shield->play_disable();
        for (int i = 0; i < 5; i++) { Coin::spawn(this, this->beatup->left_fist, false); }
        this->remove_shield();
    };
};

void Face::remove_shield()
{
    if (this->has_shield_active())
    {
        this->shield->get_sprite()->setOpacity(0); //TODO figure out how to remove it from play
        delete this->shield;
    };
};

int Face::get_sprite_index()
{
    int result;
    float percent = this->get_hits_percent();

    if (percent < 0.10)
    {
        result = 0;
    }
    else if (percent < 0.45)
    {
        result = 1;
    }
    else if (percent < 0.75)
    {
        result = 2;
    }
    else
    {
        result = 3;
    }

    return result;
};

void Face::update_sprite()
{
    if (this->removed){ return; };
    //TODO make this not waste all the time on setting opacity

    auto if_targetted = [this](Sprite* sprt){
        sprt->setOpacity(255);
        this->setScale(sx(8));
        this->setLocalZOrder(2);
    };
    auto if_inactive = [this](Sprite* sprt){
        sprt->setOpacity(200);
        this->setScale(sx(6));
        this->setLocalZOrder(1);
    };

    Face* targetted_face = this->beatup->get_target_face();

    auto set_attrs = [this, targetted_face, if_targetted, if_inactive](Sprite* sprt){
        if (targetted_face == this)
        {
            if_targetted(sprt);
        }
        else
        {
            if_inactive(sprt);
        };
    };

    Sprite* current_sprite = this->get_sprite();
    set_attrs(current_sprite);
    this->width = current_sprite->getContentSize().width;

    //make non current sprites in visible
    current_sprite->setVisible(true);
    for (auto sprt : *this->sprite_list)
    {
        if (sprt != current_sprite)
        {
            (*sprt).setVisible(false);
            set_attrs(sprt);
        }
    }
    for (auto sprt : *this->hurt_sprite_list)
    {
        if (sprt != current_sprite)
        {
            (*sprt).setVisible(false);
            set_attrs(sprt);
        }
    }
    if (current_sprite != this->attacking_sprite)
    {
        this->attacking_sprite->setVisible(false);
        set_attrs(this->attacking_sprite);
    };
    if (current_sprite != this->recovering_sprite)
    {
        this->recovering_sprite->setVisible(false);
        set_attrs(this->recovering_sprite);
    };

};


Face::Face()
{

    this->setAnchorPoint(Vec2(0.5, 0.5));
    this->beatup = NULL;
    this->attacker = NULL;
    this->shield = NULL;

    this->is_crit_hit = false;

    //this->is_first_time = true;

    this->center_x = 0;
    this->center_y = 0;

    this->hit_sound_path = this->default_sound_path;
    this->hits_taken = 0;
    this->hits_to_kill = 500;

    this->shake_clock = new Clock(0.0f);
    this->last_attack_clock = new Clock(5.0f);
    this->defaults.last_attack_threshold = 5.0f;
    this->charge_clock = new Clock(2.75f);

    this->sprite_list = new std::vector<cocos2d::Sprite*>();
    this->hurt_sprite_list = new std::vector<cocos2d::Sprite*>();

    this->features = new std::vector<FacialFeature*>();

    this->health_bar = NULL;
    this->removed = false;

    charging_part = ParticleSystemQuad::create("charging.plist");
    charging_part->setPosition(Vec2(0, 0));
    charging_part->setScale(0.15f);
    charging_part->stopSystem();

    this->charge_dmg = 2.0f;
    this->defaults.charge_dmg = 2.0f;

    this->addChild(charging_part);

};

void Face::kill()
{
    ParticleSystemQuad* fireworks_parts = ParticleSystemQuad::create("firework.plist");
    this->beatup->addChild(fireworks_parts);
    fireworks_parts->setPosition(this->getPosition());
    SoundEngine::play_sound("sounds\\old\\doom_rocket.mp3");

    this->remove();

    //try to win level if there's no other face
    if (this->beatup->get_target_face() == NULL)
    {
        this->beatup->win_level();
    };
};

void Face::remove()
{
    log("removing a face");
    if (this->removed) { return; };

    if (this->beatup->get_target_face() == this)
    {
        this->beatup->cycle_next_face();
    };
    // if the target face is still this that means there's only one target, so
    // deselect all, TODO make this less redundant
    if (this->beatup->get_target_face() == this)
    {
        this->beatup->set_target_face(NULL);
    };

    auto remove_from_faces = [this](Face* face)
    {
        bool res = this == face;
        // log("%d", res);
        return res;
    };

    auto faces = this->beatup->faces;
    faces->erase(std::remove_if(faces->begin(), faces->end() , remove_from_faces), faces->end());

    // this->beatup = NULL; // dont do this yet, needed later on punch
    // this->attacker = NULL;
    if (this->has_shield_active())
    {
        this->remove_shield();
    };

    this->sprite_list->clear();
    this->hurt_sprite_list->clear();

    delete this->sprite_list;
    delete this->hurt_sprite_list;

    this->remove_all_features();
    delete this->features;

    delete this->health_bar;
    this->removeFromParentAndCleanup(true);
    this->removed = true;
};

void Face::add_feature(Face::FeatureType ftype)
{
    if (ftype == FeatureType::Glasses)
    {
        FacialFeature* glasses = new FacialFeature(this, "ff_glasses.png", 20);
        glasses->sprite->setScale(1);
    }
    else if (ftype == FeatureType::Hat)
    {
        FacialFeature* hat = new FacialFeature(this, "ff_hat.png", 90);
        hat->sprite->setScale(1);
        Vec2 gpos = Vec2(0, this->get_sprite()->getBoundingBox().size.height/2  );
        hat->sprite->setPosition(gpos);
    }
    else if (ftype == FeatureType::Moustache)
    {
        FacialFeature* hat = new FacialFeature(this, "ff_moustache.png", 80);
        hat->sprite->setScale(1);
        Vec2 gpos = Vec2(0, -7);
        hat->sprite->setPosition(gpos);
    }
    else if (ftype == FeatureType::Beard)
    {
        FacialFeature* hat = new FacialFeature(this, "ff_beard.png", 25);
        hat->sprite->setScale(1);
        Vec2 gpos = Vec2(0, (this->get_sprite()->getBoundingBox().size.height/2)-33  );
        hat->sprite->setPosition(gpos);
    }
    else if (ftype == FeatureType::Monocle)
    {
        FacialFeature* hat = new FacialFeature(this, "ff_monocle.png", 125);
        hat->sprite->setScale(1);
        Vec2 gpos = Vec2(7, 0);
        hat->sprite->setPosition(gpos);
    };

};

void Face::reset_hits()
{
    this->hits_taken = 0;
    this->update_sprite();
};

void Face::break_all_features()
{
    auto do_brk = [](FacialFeature* feature){
        feature->do_break(); 
    };

    std::for_each(this->features->begin(), this->features->end(), do_brk);
};

void Face::remove_all_features()
{
    auto destroy_all = [](FacialFeature* elem){
        delete elem;
    };

    std::for_each(this->features->begin(), this->features->end(), destroy_all);
    this->features->clear();
};


void Face::swap_center(Face* other)
{
	if (other == NULL) { return; }
    int old_x = this->center_x;
    int old_y = this->center_y;

    this->center_x = other->center_x;
    this->center_y = other->center_y;

    other->center_x = old_x;
    other->center_y = old_y;
};

bool Face::should_shake()
{
    return this->shake_clock->is_started() && this->shield == NULL;
};

void Face::get_punched(Fist* attacker, bool is_crit_hit, float rotation)
{
    if (this->removed) { return; };

    if (attacker != NULL)
    {
        this->attacker = attacker;
    };

    this->is_crit_hit = is_crit_hit;
    if (is_crit_hit)
    {
        rotation *= 1.5f;
    };

    if (this->has_shield_active())
    {
        this->shield->get_sprite()->setRotation(rotation);
    }
    else
    {
        this->setRotation(rotation);
    };

    for (auto feat : *this->features)
    {
        if (feat->should_break(this->hits_taken))
        {
            feat->do_break();
        }
    };
};

void Face::set_center(Vec2 new_pos)
{
    this->set_center(new_pos.x, new_pos.y);
};

void Face::set_center(int x, int y)
{
    this->center_x = x;
    this->center_y = y;
};

void Face::update_health_bar(float dt)
{
    if (this->beatup->get_target_face() == this)
    {
        float percentage = 1.0f-this->get_hits_percent();

        this->health_bar->scroll_to_percentage(percentage);
        this->health_bar->setVisible(true);
    }
    else
    {
        this->health_bar->setVisible(false);
    };
};

void Face::update(float dt)
{
    if (this->removed) { return; };


    this->setPosition(this->center_x, this->center_y);

    if (this->beatup->getChildByName("quest_alert"))
    {
        return; //quest alert active, don't update
    }

    this->update_sprite();

    this->update_health_bar(dt);

    if (this->has_shield_active())
    {
        this->shield->update_sprite();
    };

    if (this->shake_clock->is_active())
    {
        this->shake_clock->update(dt);

        if (this->shake_clock->passed_threshold())
        {
            this->shake_clock->reset();
        }
    };

    this->last_attack_clock->update(dt);
    //if you haven't attacked in a while
    if (this->long_enough_since_last_attack())
    {
        //if you havent started charging
        if (this->charge_clock->start_time == 0)
        {
            this->start_charging_attack(); 
        }
        else
        {
            // cocos2d::log("%f time spent charging", this->time_spent_charging);
        }

        float percent_charged = this->charge_clock->get_percentage();
        this->charging_part->setEndColor(Color4F(percent_charged, 0, 0, 1.0));

        this->charge_clock->update(dt); //TODO fixthis adding dt too early after attacking

        //if you've been attacking for a while
        if (this->is_fully_charged())
        {
            this->do_charge_attack();
        };
    };
};

Sprite* Face::get_sprite()
{
    int index = this->get_sprite_index();
    if (this->should_shake())
    {
        return this->hurt_sprite_list->at(index);
    }
    else if (this->long_enough_since_last_attack())
    {
        if (this->charge_clock->start_time > this->charge_clock->threshold-0.15)
        {
            return this->recovering_sprite;
        }
        else if (!this->is_fully_charged())
        {
            return this->attacking_sprite;
        }

        return NULL;
    }
    else
    {
        return this->sprite_list->at(index);
    };
};


float Face::get_spawnable_width()
{
    return (this->width-8)*this->getScaleX();
};

void Face::reset_color()
{
    this->setColor(Color3B::WHITE);
};

void Face::spawn_dmg_lbl(int damage)
{
    float duration = 2.0f;

    float x_scale = sx(100*CCRANDOM_0_1());
    float y_scale = sy(100 + (10 * CCRANDOM_0_1()));

    ccBezierConfig config = {
        Vec2(x_scale, y_scale),
        Vec2(x_scale, 1),
        Vec2(1, y_scale)
    };

    auto hit_dmg = Label::createWithTTF(std::to_string(damage), DEFAULT_FONT, 48);
    hit_dmg->setScale(sx(1));
    hit_dmg->setPosition( this->beatup->get_center_pos(
        sx((75*CCRANDOM_MINUS1_1())),
        sy((150 * CCRANDOM_0_1()))
    ));
    hit_dmg->runAction(TintTo::create(duration, Color3B::RED));
    hit_dmg->runAction(ScaleBy::create(duration, 0.25f));
    hit_dmg->runAction(FadeOut::create(duration));
    hit_dmg->runAction(
        RepeatForever::create(
            Sequence::create(
                BezierBy::create(duration, config),
                BezierBy::create(duration, config),
                NULL
            )
        )
    );

    this->beatup->addChild(hit_dmg);
};

FacialFeature::FacialFeature(Face* face, std::string sprite_name, int hit_limit)
{
    this->face = face;

    this->hit_limit = hit_limit;

    this->sprite = ShatterSprite::createWithSpriteFrameName(sprite_name);
    this->sprite->getTexture()->setAliasTexParameters();
    this->face->addChild(this->sprite, 1);

    this->face->features->push_back(this);

};

FacialFeature::~FacialFeature()
{

    this->face->removeChild(this->sprite);

    //FIXME this messes with the features list since its being iterated over
    //std::vector<FacialFeature*>* vec = this->face->features;
    //vec->erase(std::remove(vec->begin(), vec->end(), this), vec->end());

    this->face = NULL;

};

bool FacialFeature::should_break(int hits_taken)
{
    bool hits_passed = hits_taken > static_cast<int>(this->hit_limit);
    return hits_passed && this->sprite->getOpacity() != 0;
};

void FacialFeature::do_break()
{
    ShatterSprite* s = this->sprite;
    s->setOpacity(0); //FIXME should this be set automatically by the action?
    this->sprite->runAction(ShatterAction::create(4));

    SoundEngine::play_sound("sounds\\old\\glassbreak.mp3");
};

Shield::Shield()
{
    this->sprite_list = new std::vector<cocos2d::Sprite*>();
    this->face = NULL;

    this->center_x = 0;
    this->center_y = 0;
    this->width = 0;

    this->hits_taken = 0;
    this->hits_to_kill = 1234;

    this->type = ShieldTypes::Human;

    this->tutorial_swipe = MotionStreak::create(1.0f, 2, 20, Color3B::WHITE, "motion_arrow.png");
};

Shield::~Shield()
{
    this->sprite_list->clear();
    delete this->sprite_list;

    ShatterSprite* s = dynamic_cast<ShatterSprite*>(this->face->getChildByName("shield_sprite"));
    if (s)
    {
        s->removeFromParentAndCleanup(true);
    }


    if (this->face != NULL)
    {
        this->face->shield = NULL; //remove ref to itself on delete, idk if this is a bad idea or not
    }
    this->face = NULL;

    // cocos2d::log("shield gone");

};

bool Shield::hit(int punch_dmg)
{
    this->play_hit();

    int base_damage = 0;
    int total_damage = base_damage + punch_dmg;

    this->hits_taken += total_damage;

    if (this->get_hits_percent() >= 1)
    {
        //this->play_disable();
        //this->face->disable_shield();
        this->face->destroy_shield();
        return false; //cant hit anymore its dead
    };

    return true;
};

void Shield::play_disable()
{
    if (this->type == ShieldTypes::Brick)
    {
        SoundEngine::play_sound("sounds\\old\\brick_scrape.mp3");
    }
    else
    {
        SoundEngine::play_sound("sounds\\old\\shield_disable.mp3");
    };
};

void Shield::play_hit()
{
    if (this->type == ShieldTypes::Brick)
    {
        SoundEngine::play_sound("sounds\\old\\brick_hit.mp3");
    }
    else
    {
        SoundEngine::play_sound("sounds\\old\\shield_hit.mp3");
    };
};

void Shield::play_destroy()
{
    if (this->type == ShieldTypes::Brick)
    {
        SoundEngine::play_sound("sounds\\old\\brick_destroy.mp3");
    }
    else
    {
        SoundEngine::play_sound("sounds\\old\\suicide_bomber.mp3");
    };
};


int Shield::get_sprite_index()
{
    int result = 0;
    return result;
};

Sprite* Shield::get_sprite()
{
    int index = this->get_sprite_index();
    return this->sprite_list->at(index);
};

void Shield::update_sprite()
{
    Sprite* current_sprite = this->get_sprite();
    current_sprite->setPosition(this->center_x, this->center_y);
    this->width = current_sprite->getContentSize().width;

    //make non current sprites in visible
    current_sprite->setVisible(true);
    for (auto it : *this->sprite_list)
    {
        if (it != current_sprite)
        {
            (*it).setVisible(false);
        }
    }
};

void Shield::set_center(Vec2 new_pos)
{
    this->set_center(new_pos.x, new_pos.y);
};

void Shield::set_center(int x, int y)
{
    this->center_x = x;
    this->center_y = y;
};

void Shield::prep_human()
{
    this->type = ShieldTypes::Human;

    Face* face = this->face;
    if (face == NULL)
    {
        return;
    };

    Sprite* shield_sprite = Sprite::createWithSpriteFrameName("s_baby.png");
    shield_sprite->setName("human_shield_sprite");
    shield_sprite->setScale(0.75);

    face->addChild(shield_sprite, 10);
    this->sprite_list->push_back(shield_sprite);
    this->set_center(0, 0);

    if (Shield::first_time_human && face->beatup->tutorials_enabled())
    {
        Shield::first_time_human = false;
        face->addChild(this->tutorial_swipe, 20);
        this->tutorial_swipe->setPosition(Vec2(
            -shield_sprite->getContentSize().width/2,
            0
        ));
        auto move_by = MoveBy::create(1.0f, Vec2(35, 0));
        this->tutorial_swipe->runAction(move_by);
    };

};

void Shield::prep_brickwall()
{
    this->type = ShieldTypes::Brick;

    Face* face = this->face;
    if (face == NULL)
    {
        return;
    };

    ShatterSprite* shield_sprite = ShatterSprite::createWithSpriteFrameName("brickwall.png");
    shield_sprite->setName("brick_shield_sprite");
    // shield_sprite->setScale(this->face->sx(1.0f));

    face->addChild(shield_sprite, 10);
    this->sprite_list->push_back(shield_sprite);
    this->set_center(0, 0);

    if (Shield::first_time_brick && face->beatup->tutorials_enabled())
    {
        Shield::first_time_brick = false;
        face->addChild(this->tutorial_swipe, 50);
        this->tutorial_swipe->setPosition(
            Vec2(0, 40)
        );
        auto move_by = MoveBy::create(1.0f, Vec2(0, -85));
        this->tutorial_swipe->runAction(move_by);

    };
};


Coin::Coin(int full_width, Beatup* beatup) : beatup(beatup)
{
	//AnimationCache* ac = AnimationCache::getInstance();
    auto anim = Coin::get_spin_animation();

    this->sprite = Sprite::createWithSpriteFrameName("coin_flat.png");
    this->sprite->runAction(anim);
    this->scale = 3;
    this->sprite->setScale(this->scale); //TODO get resolution scaling done here instead of in spawn

    this->move_to_coins_lbl = true;


    this->expiry_time = 1.2f;
    this->time_alive_clock = new Clock(this->expiry_time*2.0f);

    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();
    Vec2 pos = Vec2(
        origin.x + visibleSize.width / 2 + (cocos2d::rand_minus1_1()*(full_width / 2.0f)),
        origin.y + visibleSize.height - 300 + (cocos2d::rand_minus1_1() * 150)
    );
    this->sprite->setPosition(pos);

    this->angle = cocos2d::rand_minus1_1()*1.5;

};

Coin* Coin::spawn(Face* face, Fist* attacker, bool use_particles)
{
    Coin* coin = new Coin(face->get_spawnable_width(), face->beatup);
    coin->height_extra = rand() % 150 - 25;

    coin->init(face, attacker, use_particles);

    face->beatup->add_total_coin(1);

    SoundEngine::play_sound("sounds\\old\\coin.mp3");
    // SoundEngine::play_sound("sounds\\new\\coin\\C_coin_2.mp3");

    return coin;
};

void Coin::init(Face* face, Fist* attacker, bool use_particles)
{
    if (face->removed) { return ; };


    this->scale = sx(this->scale);
    if (face->is_crit_hit)
    {
        this->scale = 1.5f* this->scale;
    };
    this->sprite->setScale(this->scale);

    this->angle = -1 * attacker->neg(this->angle);

    face->beatup->addChild(this->sprite, 10);
    face->beatup->coins->push_back(this);

    this->should_remove = false;

    if (use_particles)
    {
        auto particle_effect = this->get_particles();

        this->sprite->addChild(particle_effect);
    };

    this->sprite->runAction(this->get_movement_action());

    float fade_duration = this->expiry_time;
    if (this->move_to_coins_lbl)
    {
        fade_duration *= 1.5f;
    };
    this->sprite->runAction(FadeOut::create(fade_duration));

};

bool Coin::update(float dt)
{
    this->time_alive_clock->update(dt);

    if (this->time_alive_clock->passed_threshold())
    {
        if (this->should_remove)
        {
            //its already been removed
            return false;
        }
        else
        {
            this->sprite->setOpacity(10);
            this->sprite->removeFromParentAndCleanup(true);
            this->sprite = NULL;
            this->should_remove = true;
            // CCLOG("remove coin or gore");
            return true;
            //delete this; //TODO actually remove
        }
    }
    else
    {
        // float percent = 1 - this->time_alive_clock->get_percentage();
        // this->sprite->setOpacity(percent * 255);

        // float x_scale = 100 * angle*dt;
        // this->sprite->setPositionX(this->sprite->getPositionX() + x_scale);

        // float y_scale = (percent*expiry_time*(500 * percent)*dt) + (this->height_extra*percent*dt);
        // this->sprite->setPositionY(this->sprite->getPositionY() + y_scale);

        return false;
    };

};

RepeatForever* Coin::get_spin_animation()
{
    //if (!Coin::_has_spin_anim)
    //{
        Coin::_has_spin_anim = true;
        SpriteFrameCache* sfc = SpriteFrameCache::getInstance();
        auto sprite_frame_flat = sfc->getSpriteFrameByName("coin_flat.png");
        auto sprite_frame_left = sfc->getSpriteFrameByName("coin_left.png");
        auto sprite_frame_side = sfc->getSpriteFrameByName("coin_side.png");
        auto sprite_frame_right = sfc->getSpriteFrameByName("coin_right.png");

        auto vec = Vector<SpriteFrame*>(4);
        vec.pushBack(sprite_frame_flat);
        vec.pushBack(sprite_frame_left); 
        vec.pushBack(sprite_frame_side);
        vec.pushBack(sprite_frame_right);
        Animation* anim = Animation::createWithSpriteFrames(vec, 0.05f);
        return Coin::_spin_animation = RepeatForever::create(Animate::create(anim));
    //};

    //return  Coin::_spin_animation->clone();
};

Gore::Gore(int full_width, Beatup* beatup) : Coin(full_width, beatup)
{
    this->move_to_coins_lbl = false;

    RandomWeightMap<int> rwm;
    rwm.add_item(1, 90);
    rwm.add_item(2, 20);
    rwm.add_item(3, 10);
    rwm.add_item(4, 90);
    rwm.add_item(5, 20);
    rwm.add_item(6, 10);
    rwm.add_item(7, 90);
    rwm.add_item(8, 20);
    rwm.add_item(9, 10);

    int idx = rwm.get_item();
    std::stringstream ss;
    ss << "gore" << idx << ".png";
    this->sprite = Sprite::createWithSpriteFrameName(ss.str());
    this->scale = 3;

    this->sprite->setScale(this->scale); //TODO get resolution scaling done here instead of in spawn

    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();
    Vec2 pos = Vec2(
        origin.x + visibleSize.width / 2 + (cocos2d::rand_minus1_1()*(full_width / 2.0f)),
        origin.y + visibleSize.height - 300 + (cocos2d::rand_minus1_1() * 150)
    );
    this->sprite->setPosition(pos);

    this->angle = cocos2d::rand_minus1_1()*1.5;
};
//
//ParticleSystemQuad* Gore::get_blood_parts()
//{
//    if (!Gore::_has_blood_parts)
//    {
//        Gore::__blood_parts = ParticleSystemQuad::create("blood.plist");
//        Gore::__blood_parts->setTotalParticles(5);
//        Size sprite_size = Size(35, 28);
//        Gore::__blood_parts->setPosition(sprite_size.width/2, sprite_size.height/2);
//        Gore::__blood_parts->setAutoRemoveOnFinish(true);
//    };
//
//    //return Gore::__blood_parts->du
//};

ParticleSystemQuad* Coin::get_particles()
{
    ParticleSystemQuad* gold_parts = ParticleSystemQuad::create(Coin::particle_map);
    gold_parts->setTotalParticles(5);
    gold_parts->setDuration(.5f);
    gold_parts->setScale(.10f);
    // gold_parts->setAutoRemoveOnFinish(true);

    Color4F end_col = gold_parts->getStartColor();
    end_col.a = 0;
    gold_parts->setEndColor(end_col);

    auto sprite_pos = this->sprite->getContentSize();
    float x = sprite_pos.width / sx(2);
    float y = sprite_pos.height / sy(2);
    gold_parts->setPosition(x, y);
    return gold_parts;
};

Sequence* Coin::get_movement_action()
{
    float x_scale = 100*this->angle;
    float y_scale = 100+this->height_extra;

    Vec2 jump_vec = Vec2(
            sx(x_scale),
            sy(y_scale)
    );
    float height = 100;
    float jumps = 1;

    JumpBy* jump_by = JumpBy::create(this->expiry_time, jump_vec, height, jumps);

    Sequence* seq;

    if (this->move_to_coins_lbl)
    {
        
        std::function<void()> func = std::bind(&Beatup::update_coins_lbl, this->beatup, true, this->beatup->get_total_coins()+1);
        Sequence* targeted_seq = Sequence::create(
            CallFunc::create(func),
            NULL
        );

        MoveTo* move_to = MoveTo::create(
            0.1f,
            this->beatup->coins_lbl->convertToWorldSpace(this->beatup->coins_lbl->getPosition())
        );

        seq = Sequence::create(
            jump_by, 
            move_to,
            TargetedAction::create(this->beatup->coins_lbl, targeted_seq),
            NULL
        );
    }
    else
    {
        seq = Sequence::create(jump_by, NULL);
    };
    return seq;
};

Coin* Gore::spawn(Face* face, Fist* attacker, bool use_particles)
{
    //this whole thing is a rough copy of Coin::spawn because I don't want to
    //use templates or whatever yet
    Gore* gore = new Gore(face->get_spawnable_width(), face->beatup);


    gore->sprite->setRotation(rand_0_1() * 100);
    auto rotate = RotateBy::create(1, attacker->adj(80.0f));
    gore->sprite->runAction(rotate);

    gore->height_extra = rand() % 220 - 105;

    gore->init(face, attacker, use_particles);

    return gore;
};

ParticleSystemQuad* Gore::get_particles()
{
    auto blood_parts = ParticleSystemQuad::create(Gore::particle_map);
    blood_parts->setTotalParticles(5);
    Size sprite_size = Size(35, 28);
    blood_parts->setPosition(sprite_size.width/2, sprite_size.height/2);
    blood_parts->setAutoRemoveOnFinish(true);

    return blood_parts;
};
