#include "Beatup.h"

#include "assert.h"
#include <iostream>


#include "Clock.h"
#include "Face.h"
#include "SoundEngine.h"
#include "ShopScene.h"
#include "constants.h"
#include "Weapons.h"
#include "Fist.h"
#include "DataManager.h"
#include "ProgressBar.h"
#include "Combo.h"
#include "ShatterNode.h" //is used on android
#include "Quest.h"
#include "Alert.h"
#include "Level.h"


#pragma warning(default:4267)
#pragma warning(default:4251)
#pragma warning(default:4244)
#include "Util.h"
#include "MainMenu.h"
#include <proj.win32/HouseBuilding/Recipe.h>

USING_NS_CC;

FUNC_CREATESCENE(Beatup);

std::string Beatup::tutorial_id_key = "tutorials_enabled";

Scene* Beatup::main_menu_scene = NULL;

Scene* Beatup::createScene(int level = 0)
{
    Scene* scene = Scene::create();
    Beatup* beatup = Beatup::create();
    beatup->setName("beatup");
    scene->addChild(beatup);

    Director::getInstance()->setProjection(Director::Projection::_2D); //needed for console cmd resizing

    if (level > 0)
    {
        beatup->prep_level(level);
    };

    beatup->create_quest_alert();

	return scene;
};

std::string Beatup::total_hit_key = "total_hit_key";
std::string Beatup::total_coin_key = "total_coin_key";

void Beatup::set_background(BackgroundTypes bg_type)
{
    std::string background_img_path;
    if (bg_type == Forest) background_img_path = "forest.png";
    else if (bg_type == Volcano) background_img_path = "volcano.png";
    else if (bg_type == Western) background_img_path = "western.png";

    this->background = Sprite::create(background_img_path);

    this->background->setAnchorPoint(Vec2(0.5, 0.5));
    this->background->setScale(sx(10.5f));
    this->background->setPosition(this->get_center_pos());

    this->background->setColor(Color3B(100, 100, 100));
    this->background->getTexture()->setAliasTexParameters();

    this->addChild(this->background, -1);
};

bool Beatup::init()
{

#ifdef _WIN32
    FUNC_INIT_WIN32(Beatup);
#else
    FUNC_INIT(Beatup);
#endif

    CCLOG("beatup init");

    this->level = NULL;
    this->quest = NULL;
    this->quest_completed = false;

    this->block_count = 0;
    this->temp_coins = 0;
    this->temp_charges = 0;

    this->level_been_won = false;
    this->level_been_lost = false;

    this->color_layer = LayerColor::create(Color4B(0, 0, 0, 25));
    this->addChild(this->color_layer);

    this->punch_log = new PunchLog();
    this->punches_to_coin = 10;
    this->punch_count = 0;

    this->shake_clock = new Clock(0.0f);

    this->spawn_coin_clock = new Clock(0.15f);

    this->is_comboing = false;
    this->flurry_switch_clock = new Clock(0.032f);
    this->flurry_with_left_hand = false;

    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    this->combos = new std::vector<Combo*>();

    int side_height = (int)sy(100);

    Vec2* new_pos = new Vec2(
        visibleSize.width / 2 + origin.x + sx(350),
        visibleSize.height / 2 + origin.y + sy(300)
        );
    new_pos->x = new_pos->x + sx(20);
    new_pos->y = new_pos->y - side_height + sy(30);



    auto create_combo = [this, new_pos, side_height](
        std::string id_key, std::string name, unsigned int cost,
        std::initializer_list<FistHands> order, float shake_time,
        float stamina_regen)
    {
        Combo* combo = new Combo(this, id_key, name);
        combo->shop_cost = cost;
        combo->hand_order = new std::vector<FistHands>(order);
        combo->shake_time = shake_time;
        combo->stamina_regen = stamina_regen;

        combo->defaults.cooldown = stamina_regen * 0.7f;
        combo->cooldown = combo->defaults.cooldown;
        combo->cooldown_clock->set_threshold(combo->cooldown);

        this->combos->push_back(combo);

        int combo_height = (int)combo->cd_bar->front_timer->getBoundingBox().size.height;

        new_pos->y = new_pos->y - combo_height - sy(5);
        combo->cd_bar->do_finish_bump = true;
        combo->cd_bar->setPosition(*new_pos);

    };

    // LRLLLR
    auto order = { FistHands::Left, FistHands::Right, FistHands::Left, FistHands::Left, FistHands::Left, FistHands::Right };
    create_combo("left_field", "Left Field", 25, order, 0.5f, 10.0f);

    // LRRRLL
    order = { FistHands::Left, FistHands::Right, FistHands::Right, FistHands::Right, FistHands::Left, FistHands::Left };
    create_combo("grand_tour", "Grand Tour", 35, order, 0.75f, 15.0f);

    // RRLLRL
    order = { FistHands::Right, FistHands::Right, FistHands::Left, FistHands::Left, FistHands::Right, FistHands::Left };
    create_combo("fist_salad", "Fist Salad", 65, order, 1.15f, 20.0f);

    // RRRLRLLR
    order = { FistHands::Right, FistHands::Right, FistHands::Right, FistHands::Left, FistHands::Right, FistHands::Left, FistHands::Left, FistHands::Right };
    create_combo("knuckle_avalanche", "Knuckle Avalanche", 155, order, 1.50f, 25.0f);

    // LRLLRLLLR
    order = { FistHands::Left, FistHands::Right, FistHands::Left, FistHands::Left, FistHands::Right, FistHands::Left, FistHands::Left, FistHands::Left, FistHands::Right };
    create_combo("love_tap", "Love Tap", 200, order, 1.75f, 30.0f);

    // LRLLRRLLLR
    order = { FistHands::Left, FistHands::Right, FistHands::Left, FistHands::Left, FistHands::Right, FistHands::Right, FistHands::Left, FistHands::Left, FistHands::Left, FistHands::Right };
    create_combo("impressive_hook", "Impressive Hook", 300, order, 2.25f, 35.0f);

    // RRLRRLLLLR
    order = { FistHands::Right, FistHands::Right, FistHands::Left, FistHands::Right, FistHands::Right, FistHands::Left, FistHands::Left, FistHands::Left, FistHands::Left, FistHands::Right };
    create_combo("brawlhalla", "Brawlhalla", 400, order, 2.75f, 55.0f);

    //shop_lbl
    this->shop_banner = Sprite::createWithSpriteFrameName("shop_banner.png");

    this->shop_banner->setAnchorPoint(Vec2(0.0f, 0.0f));
    Vec2 banner_pos = Vec2(
        0,
        visibleSize.height / 2 + origin.y + sy(100)
        );
    this->shop_banner->setScale(sx(4));
    this->shop_banner->setPosition(banner_pos);
    this->addChild(this->shop_banner);

    this->shop_lbl = Sprite::createWithSpriteFrameName("text_shop.png");
    this->shop_lbl->setPosition(Vec2(
        28,
        17
        ));

    this->shop_banner->addChild(this->shop_lbl);

    this->coins_lbl = Label::createWithTTF("0 coins", DEFAULT_FONT, 24);
    this->coins_lbl->setAnchorPoint(Vec2(0.5f, 0.5f));
    this->coins_lbl->getFontAtlas()->setAliasTexParameters();
    this->coins_lbl->setPosition(Vec2(
        28,
        6.5f
        ));
    this->coins_lbl->setScale(0.2f);

    this->shop_banner->addChild(this->coins_lbl);

    //rocket
    this->rocket = new RocketWeapon(this, "throwable_rocket", "Rocket");
    this->rocket->init("weapon_rocket.png");

    //grenade
    this->grenade = new GrenadeWeapon(this, "throwable_grenade", "Grenade");
    this->grenade->init("weapon_grenade.png");
    this->grenade->change_cost(15);

    auto quest_button = MenuItemSprite::create(
        Sprite::createWithSpriteFrameName("back_banner.png"),
        Sprite::createWithSpriteFrameName("back_banner.png"),
        std::bind(&Beatup::create_quest_alert, this));

    quest_button->setScale(sx(4.5f));

    quest_button->setPosition(Vec2(
        origin.x + visibleSize.width - quest_button->getContentSize().width*quest_button->getScaleX(),
        origin.y + quest_button->getContentSize().height*quest_button->getScaleY())
        );

    // create menu, it's an autorelease object
    auto menu = Menu::create(quest_button, NULL);
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu, 1);

    this->title_lbl = Label::createWithTTF("Beat up the face!", DEFAULT_FONT, 24);

    title_lbl->setScaleX(sx(1));
    title_lbl->setScaleY(sy(1));
    title_lbl->setPosition(this->get_center_pos(
        0,
        sy(265)
        ));

    // add the label as a child to this layer
    this->addChild(title_lbl, 1);

    this->coins = new std::vector<Coin*>();
    this->gores = new std::vector<Gore*>();

    this->faces = new std::vector<Face*>();

    this->target_face = NULL;

    this->block_clock = new Clock(0.0f);

    this->prep_fists();
    this->prep_other();

    this->update(0.001f);
    this->update_coins_lbl(false);

    this->setup_commands();

    this->has_been_initialized = true;

    auto gold__plistFile = FileUtils::getInstance()->fullPathForFilename("gold.plist");
    Coin::particle_map = FileUtils::getInstance()->getValueMapFromFile(gold__plistFile.c_str());

    auto _plistFile = FileUtils::getInstance()->fullPathForFilename("blood.plist");
    Gore::particle_map = FileUtils::getInstance()->getValueMapFromFile(_plistFile.c_str());

    this->buildup = new Buildup();
    auto player = new Player("Jimothy");
    player->coins = 100;
    this->buildup->player = player;

    auto animal = std::make_shared<Animal>("Tank");

    this->buildup->city = Buildup::init_city(this->buildup);
    this->buildup->city->update_buildings(0);

    //Director::getInstance()->getScheduler()->scheduleUpdate(schedule_selector(Beatup::update_buildup), 0, false);
    //Scheduler::scheduleUpdate(schedule_selector(Buildup::update), 0, false);
    this->schedule(schedule_selector(Beatup::update_buildup));

    return true;
}

void Beatup::update_buildup(float dt)
{
    this->buildup->update(dt);
}

void Beatup::setup_commands()
{
    Console::Command face_charge = {"fc", "makes the face charge up the attack",
        [this](int, std::string)
        {
            this->get_target_face()->start_charging_attack();
        } };
    auto console = Director::getInstance()->getConsole();
    console->addCommand(face_charge);

    Console::Command reset_combo = {"rc", "restores a combo's cooldown",
        [this](int, std::string args)
        {
            this->combos->at(stoi(args))->update(99999999.0f);
        } };
    console->addCommand(reset_combo);

    auto gl = Director::getInstance()->getOpenGLView();
    auto frame_size = gl->getFrameSize();
    auto design_size = gl->getDesignResolutionSize();
    CCLOG("frame %f %f", frame_size.width, frame_size.height);
    CCLOG("design %f %f", design_size.width, design_size.height);


    Console::Command hd = {"hd", "1080p",
        [this](int, std::string args)
        {
            auto glView = Director::getInstance()->getOpenGLView();
            glView->setFrameSize(1920, 1080);
            glView->setDesignResolutionSize(1920, 1080, ResolutionPolicy::SHOW_ALL);
            Director::getInstance()->popToRootScene();
        } };
    console->addCommand(hd);

    Console::Command sd = {"sd", "640p",
        [this](int, std::string args)
        {
            auto glView = Director::getInstance()->getOpenGLView();
            glView->setFrameSize(960, 640);
            glView->setDesignResolutionSize(960, 640, ResolutionPolicy::SHOW_ALL);
        } };
    console->addCommand(sd);

};

void Beatup::remove_all_faces()
{
    auto delete_all_faces = [this](Face* elem)
    {
        elem->remove();
        delete elem;
    };
    std::for_each( this->faces->begin(), this->faces->end(), delete_all_faces);
    this->faces->clear();
};

void Beatup::prep_level(int level)
{
    Level* level_obj = Level::create_level(level);
    if (level_obj == NULL)
    {
        return;
    }
    //assumes there's only one new face 
    this->remove_all_faces();

    Face* face = new Face();
    face->beatup = this;

    this->faces->push_back(face);
    this->target_face = face;

    this->prep_face(face);

    this->apply_flash_shader();

    level_obj->setup_face(*face);
    level_obj->setup_player(*this);
    level_obj->setup_quest(*this);
    level_obj->setup_scene(*this);


    this->apply_flash_shader();
};

void Beatup::add_total_hit(int x)
{
    DataManager::incr_key("total_hit_key", x);
};

int Beatup::get_total_hits()
{
    return DataManager::get_int_from_data(Beatup::total_hit_key);
};

unsigned Beatup::get_total_coins()
{
    return DataManager::get_int_from_data(Beatup::total_coin_key);
};

void Beatup::add_total_coin(int x)
{
    this->temp_coins += x;
    DataManager::incr_key("total_coin_key", x);

    auto farm = this->buildup->city->building_by_name("The Farm");

    farm->create_resources(Resource::Ingredient, x, "grain");
};

void Beatup::back_to_menu()
{
    SoundEngine::stop_all_sound_and_music();
    SoundEngine::play_music("music\\menu.mp3");

    if (this->get_level_over())
    {
        MainMenu::beatup_scene = NULL; //this is to disable the resume button
    };

    auto director = Director::getInstance();
    director->pushScene(Beatup::main_menu_scene);
}

void Beatup::onEnter()
{
    GameLayer::onEnter();
    //this is so it updates again because if you pop a scene or something, the
    //game stops the scheduler FIXME
    this->schedule(schedule_selector(Beatup::update));
};

void Beatup::switch_to_shop()
{
    auto director = Director::getInstance();

    if (this->shop_scene == NULL)
    {
        this->shop_scene = Scene::create();
    }
    if (this->shop == NULL)
    {
        this->shop = Shop::create();
        this->shop->beatup = this;
        this->shop->menu_init();
        this->shop->update(0);

        this->shop_scene->addChild(this->shop);
        this->shop->beatup_scene = director->getRunningScene();
    };

    this->shop->update_coins_lbl();

    director->pushScene(this->shop_scene);
};

bool Beatup::onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event)
{
    GameLayer::onTouchBegan(touch, event);

    if (!this->getChildByName("quest_alert"))
    {
        for (Fist* fist : { this->left_fist, this->right_fist })
        {
            if (touch_in_node(fist->sprite, touch) && !fist->is_punching)
            {
                if (this->is_blocking)
                {

                }
                else
                {
                    fist->punch();

                    if (DataManager::get_bool_from_data("charging_enabled", false))
                    {
                        fist->charging_clock->start();
                    };

                    return true;
                }
            }

        };
    };

    if ((touch_in_node(this->player_hp_bar->front_timer, touch, 1.5) || touch_in_node(this->player_hp_bar->back_timer, touch, 1.5) ||
                touch_in_node(this->stamina_prog->front_timer, touch, 1.5) || touch_in_node(this->stamina_prog->back_timer, touch, 1.5)) 
            && !this->right_fist->is_punching)
    {
        this->toggle_is_blocking();
        return false;
    }

    if (touch_in_node(this->shop_banner, touch))
    {
        this->switch_to_shop();
        return false;
    }

    if (touch_in_node(this->rocket->sprite, touch))
    {
        this->rocket->fire();
        return false;
    }

    if (touch_in_node(this->grenade->sprite, touch))
    {
        this->grenade->fire();
        return false;
    }

    else 
    {

        for (auto combo : *this->combos)
        {
            if (!combo->get_been_bought()) { continue; };

            if (touch_in_node(combo->cd_bar->front_timer, touch) ||
                touch_in_node(combo->cd_bar->back_timer, touch))
            {
                combo->cd_bar->bump(0.125f, 1.05f);

                combo->toggle_order();

                this->print_inventory();
                return false;
            };
        };
    }

    return true; //determines whether the touch keeps going or something
};

void Beatup::onTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event)
{
    GameLayer::onTouchEnded(touch, event);

    if (!this->getChildByName("quest_alert"))
    {
        //quest alert active, don't update
        for (Fist* fist : { this->left_fist, this->right_fist })
        {
            if (touch_in_node(fist->sprite, touch) && !fist->is_punching)
            {

                if (DataManager::get_bool_from_data("charging_enabled", false) && fist->charging_clock->is_started())
                {
                    if (fist->charging_clock->passed_threshold())
                    {
                        //charged punch is true
                        fist->punch(true);
                    };
                }
            }
            fist->charging_clock->reset();
            fist->sprite->setScale(sx(fist->defaults.scale));

            auto parent = fist->sprite->getParent();
            parent->stopAllActions();
            parent->runAction(StopGrid::create());

            fist->is_shaky_running = false;
            fist->has_played_charge_sound = false;
        };
    };
};

RepeatForever* animate_idle(std::string neutral_str, std::string left_str, std::string right_str)
{
	SpriteFrameCache* sfc = SpriteFrameCache::getInstance();
	auto neutral = sfc->getSpriteFrameByName(neutral_str);
	auto left = sfc->getSpriteFrameByName(left_str);
	auto right = sfc->getSpriteFrameByName(right_str);
	auto vec = Vector<SpriteFrame*>(100);

    for (int i = 0; i<= 25; i++)
    {
        vec.pushBack(neutral);
    };
    vec.pushBack(left);
    vec.pushBack(left);
    for (int i = 0; i<= 9; i++)
    {
        vec.pushBack(neutral);
    };
    vec.pushBack(right); 
    vec.pushBack(right); 
    vec.pushBack(right); 
    vec.pushBack(neutral);
    vec.pushBack(left);
    vec.pushBack(left);
    vec.pushBack(neutral);
    Animation* anim = Animation::createWithSpriteFrames(vec, 0.15f);
    return  RepeatForever::create(Animate::create(anim));

};

void Beatup::prep_face(Face* face)
{

    this->node_grid = NodeGrid::create();
    this->node_grid->setPosition(0,0 );

    this->addChild(node_grid);

    face->health_bar = new ProgressBar(this, "", "", "enemy_healthbar_bar.png", "enemy_healthbar.png");
    face->health_bar->back_timer->setSprite(Sprite::createWithSpriteFrameName("enemy_healthbar_bar_white.png"));
    //face->health_bar->front_timer->setBarChangeRate(Vec2(1.0f, 0.8f));
    //face->health_bar->back_timer->setBarChangeRate(Vec2(1.0f, 0.8f));
    face->health_bar->fit_back_to_front();
    face->health_bar->setAnchorPoint(Vec2(0.5f, 0.5f));

    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();
    float hby = face->health_bar->front_timer->getContentSize().height;
    Vec2 health_pos = Vec2(
        this->get_center_pos().x,
        visibleSize.height - sy(hby)*2
    );
    face->health_bar->setPosition(health_pos);

    auto create_animated_sprite = [face](std::string sprite_path, std::string neutral_path, std::string left_path, std::string right_path) {
        auto sprite = Sprite::createWithSpriteFrameName(sprite_path);
        sprite->getTexture()->setAliasTexParameters();
        if (neutral_path != "")
        {
            sprite->runAction(animate_idle(neutral_path, left_path, right_path));
        };
        face->addChild(sprite);
        sprite->setVisible(false);

        return sprite;
    };

    face->attacking_sprite = create_animated_sprite("f_eyes_open.png", "", "", "");
    face->recovering_sprite = create_animated_sprite("f_side_closed.png", "", "", "");


    Sprite* face_sprt = create_animated_sprite("f_face_neutral.png", "f_face_neutral.png", "f_look_left.png", "f_look_right.png");

    Sprite* face_sprt_hurt = create_animated_sprite("f_teeth.png", "", "", ""); //TODO missing anim

    Sprite* face_sprt_wounded = create_animated_sprite("f_teeth_one.png", "f_teeth_one.png", "f_look_left_teeth.png", "f_look_right_teeth.png");

    Sprite* face_sprt_dead = create_animated_sprite("f_face_neutral_eye.png", "f_face_neutral_eye.png", "f_look_left_eye.png", "f_look_right_eye.png");

    //unused?
    //Sprite* face_sprt_destroyed = create_animated_sprite("f_face_neutral_2eye.png", "f_face_neutral_eye.png", "f_look_left_2eye.png", "f_look_right_2eye.png");

    face->sprite_list = new std::vector < Sprite* > {
        face_sprt,
            face_sprt_hurt,
            face_sprt_wounded,
            face_sprt_dead,
            face_sprt_dead
    };


	Sprite* hit_face_sprt = create_animated_sprite("f_gritted_teeth.png", "", "", "");

	Sprite* hit_face_sprt_hurt = create_animated_sprite("f_teeth_side.png", "", "", "");

	Sprite* hit_face_sprt_wounded = create_animated_sprite("f_teeth_side_one.png", "", "", "");

	Sprite* hit_face_sprt_dead = create_animated_sprite("f_gritted_teeth_one.png", "", "", "");

    //unused?
    Sprite* hit_face_sprt_destroyed = Sprite::create("hit_doomguy.png", {0, 266/2, 52/2, 64/2});
    hit_face_sprt_destroyed->getTexture()->setAliasTexParameters();

    face->hurt_sprite_list = new std::vector < Sprite* > {
        hit_face_sprt,
            hit_face_sprt_hurt,
            hit_face_sprt_wounded,
            hit_face_sprt_dead,
            hit_face_sprt_dead
    };

    this->node_grid->addChild(face);

    face->beatup = this;

    Vec2 face_pos = Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y);
    face->set_center(face_pos);

    //hide all unused sprites
    face->update_sprite();
    face->width = (int)(face->get_sprite()->getTextureRect().size.width * face->getScaleX());

    face->setCascadeColorEnabled(true);
    face->reset_color();
};

void Beatup::prep_fists()
{
    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

	//auto sfc = SpriteFrameCache::getInstance();
    //init fists
    this->left_fist = new Fist(FistSprite::createWithSpriteFrameName("fist_neutral.png"), true);
    this->left_fist->moving_sprite = FistSprite::createWithSpriteFrameName("fist_moving.png");
    this->left_fist->hit_sprite = FistSprite::createWithSpriteFrameName("fist_hit.png");
    this->left_fist->hand = FistHands::Left;
    this->left_fist->beatup = this;
    this->left_fist->defaults.x = (int)(visibleSize.width / 2 + origin.x-sx(200));
    this->left_fist->defaults.y = (int)(visibleSize.height / 2 + origin.y-sx(250));
    this->left_fist->reset_pos();

    if (this->tutorials_enabled())
    {
        ParticleSystemQuad* left_touch = ParticleSystemQuad::create("touch_here.plist");
        this->addChild(left_touch, 20);
        left_touch->setPosition(this->left_fist->get_punch_pos());
    };

    Vec2 flames_pos(6, 8);
    this->left_fist->sprite->prep(this->left_fist, flames_pos);
    this->left_fist->sprite->setVisible(true);
    this->left_fist->moving_sprite->prep(this->left_fist, flames_pos);
    this->left_fist->hit_sprite->prep(this->left_fist, flames_pos);

    this->right_fist = new Fist(FistSprite::createWithSpriteFrameName("fist_neutral.png"), false);
    this->right_fist->moving_sprite = FistSprite::createWithSpriteFrameName("fist_moving.png");
    this->right_fist->hit_sprite = FistSprite::createWithSpriteFrameName("fist_hit.png");
    this->right_fist->beatup = this;
    this->right_fist->hand = FistHands::Right;
    this->right_fist->defaults.x = (int)(visibleSize.width / 2 + origin.x+sx(200));
    this->right_fist->defaults.y = (int)(visibleSize.height / 2 + origin.y-sx(250));
    this->right_fist->reset_pos();

    this->right_fist->sprite->prep(this->right_fist, flames_pos);
    this->right_fist->sprite->setVisible(true);
    this->right_fist->moving_sprite->prep(this->right_fist, flames_pos);
    this->right_fist->hit_sprite->prep(this->right_fist, flames_pos);

    this->left_fist_node_grid = NodeGrid::create();
    this->addChild(this->left_fist_node_grid, 10);
    this->right_fist_node_grid = NodeGrid::create();
    this->addChild(this->right_fist_node_grid, 10);

    this->left_fist->other = this->right_fist;
    this->left_fist_node_grid->addChild(this->left_fist->sprite);
    this->left_fist_node_grid->addChild(this->left_fist->moving_sprite);
    this->left_fist_node_grid->addChild(this->left_fist->hit_sprite);

    this->right_fist->other = this->right_fist;
    this->right_fist_node_grid->addChild(this->right_fist->sprite);
    this->right_fist_node_grid->addChild(this->right_fist->moving_sprite);
    this->right_fist_node_grid->addChild(this->right_fist->hit_sprite);

};

void Beatup::set_visible_weapon_button(FistWeaponTypes weapon_type, bool val)
{
    WeaponButton* button = NULL;
    if (weapon_type == FistWeaponTypes::Flames)
    {
        button = this->flames_button;
    }
    else if (weapon_type == FistWeaponTypes::Psionic)
    {
        button = this->psionic_button;
    }
    else if (weapon_type == FistWeaponTypes::Frost)
    {
        button = this->frost_button;
    }
    else
    {
        assert(false && "Can't make this unrecognized FistWeaponTypes visible");
    };

    button->setVisible(val);
};

void Beatup::update_player_hp_bar()
{
    float percentage = (float)this->player_hp/(float)this->player_total_hp;
    this->player_hp_bar->scroll_to_percentage(percentage);
};

void Beatup::prep_other()
{
    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    //player HP

    this->player_hp = 10; //these number gets replaced in setup_level
    this->player_total_hp = 10;

    this->player_hp_bar = new ProgressBar(this, "", "", "healthbar_bar.png", "healthbar.png");
    this->player_hp_bar->back_timer->setSprite(Sprite::createWithSpriteFrameName("healthbar_bar_white.png"));
    this->player_hp_bar->setAnchorPoint(Vec2(0.5f, 0.5f));
    this->player_hp_bar->setPosition(Vec2(
        this->get_center_pos().x,
        this->player_hp_bar->front_timer->getSprite()->getContentSize().height*2
    ));
    this->player_hp_bar->setScale(sx(5));

    this->update_player_hp_bar();

    this->stamina_clock = new Clock(0.25f);
    this->stamina_max = 80;
    this->stamina_count = this->stamina_max;

    this->stamina_prog = new ProgressBar(this, "", "", "combo_bar.png", "");
    this->stamina_prog->back_timer->setSprite(Sprite::createWithSpriteFrameName("combo_bar_white.png"));
    this->stamina_prog->fit_back_to_front();
    this->stamina_prog->setPosition( this->get_center_pos(0, -sy(230)));
    this->stamina_prog->setAnchorPoint(Vec2(0.5f, 0.5f));
    this->stamina_prog->set_percentage(100.0f);
     // this->stamina_prog->setScale(sx(4));
    this->stamina_prog->wait_to_clear = false;

    this->defaults.stamina_punch_cost = 2;
    this->stamina_punch_cost = this->defaults.stamina_punch_cost;

    this->defaults.stamina_regen_rate = 1;
    this->stamina_regen_rate = this->defaults.stamina_regen_rate;

    this->shop = NULL;
    this->shop_scene = NULL;


    float button_y = 200.0f;
    float button_margin = 50.0f;

    this->fist_flame = new FistWeapon(this, "flaming_fists", "Flaming fists", FistWeaponTypes::Flames);
    this->fist_flame->init({
        this->left_fist->sprite,
        this->left_fist->moving_sprite,
        this->left_fist->hit_sprite,

        this->right_fist->sprite,
        this->right_fist->moving_sprite,
        this->right_fist->hit_sprite,
    });
    this->flames_button = WeaponButton::create("weapon_select_button.png", "weapon_select_selected.png", "weapon_select_button.png");
    this->flames_button->prep(this, this->fist_flame, Vec2(0, sy(button_y)), "sounds\\new\\lighter\\C_lighter_1.mp3", "weapon_flame.png");
    

    this->fist_psionic = new FistWeapon(this, "psionic_fists", "Psionic fists", FistWeaponTypes::Psionic);
    this->fist_psionic->init({
        this->left_fist->sprite,
        this->left_fist->moving_sprite,
        this->left_fist->hit_sprite,

        this->right_fist->sprite,
        this->right_fist->moving_sprite,
        this->right_fist->hit_sprite,
    });
    this->psionic_button = WeaponButton::create("weapon_select_button.png", "weapon_select_selected.png", "weapon_select_button.png");
    this->psionic_button->prep(this, this->fist_psionic, Vec2(0, sy(button_y-=button_margin)), "sounds\\old\\radio_wave.mp3", "weapon_psionic.png");


    this->fist_frost = new FistWeapon(this, "frost_fists", "Frosty fists", FistWeaponTypes::Frost);
    this->fist_frost->init({
        this->left_fist->sprite,
        this->left_fist->moving_sprite,
        this->left_fist->hit_sprite,

        this->right_fist->sprite,
        this->right_fist->moving_sprite,
        this->right_fist->hit_sprite,
    });
    this->frost_button = WeaponButton::create("weapon_select_button.png", "weapon_select_selected.png", "weapon_select_button.png");
    this->frost_button->prep(this, this->fist_frost, Vec2(0, sy(button_y-=button_margin)), "sounds\\old\\ice_freeze.mp3", "weapon_ice.png");

    this->addChild(this->frost_button);
    this->addChild(this->psionic_button);
    this->addChild(this->flames_button);

};

void Beatup::deal_player_dmg(int dmg)
{
    this->player_hp-=dmg;
    if (this->player_hp <= 0)
    {
        this->player_hp = 0;
        this->level_been_lost = true;
    };
};

void Beatup::lose_level()
{
    log("GAME OVER");
    this->level_been_lost = false;


    this->remove_all_faces();

    this->hide_ui();

    auto resize_btn = [](ui::Button* button) {
        auto lbl_size = button->getTitleRenderer()->getContentSize();
        button->setContentSize(Size(lbl_size.width * 1.5f, lbl_size.height * 2.5f));
    };

    Sprite* lose_sprite = Sprite::createWithSpriteFrameName("lose.png");
    lose_sprite->setPosition(this->get_center_pos());
    lose_sprite->setScale(sx(4));
    lose_sprite->getTexture()->setAliasTexParameters();
    this->addChild(lose_sprite);
    Sprite* lose_face = Sprite::createWithSpriteFrameName("lose_face.png");
    lose_face->setAnchorPoint(Vec2(0.5f, 0.5f));
    lose_face->setPosition(Vec2(
        lose_sprite->getContentSize().width/2,
        lose_sprite->getContentSize().height/2+05
        ));
    lose_sprite->addChild(lose_face);

    lose_face->runAction(RepeatForever::create(Sequence::createWithTwoActions(
        JumpBy::create(2.5f, Vec2(0, 0), 5.0f, 5),
        DelayTime::create(1.0f))
        ));

    ui::Button* main_button = ui::Button::create();
    main_button->loadTextureNormal("main_UI_export_10_scale9_x4.png", ui::TextureResType::PLIST);

    main_button->setTitleText("Continue");
    main_button->setScale9Enabled(true);
    main_button->setScale(sx(1)); //scale 1 but its on a x4 sprite
    main_button->setTitleFontSize(30);
    main_button->setTitleFontName(DEFAULT_FONT);
    main_button->setTitleColor(Color3B::BLACK);
    main_button->addTouchEventListener([this](Ref*, cocos2d::ui::Widget::TouchEventType){this->back_to_menu(); });
    resize_btn(main_button);

    this->addChild(main_button);
    Size lose_size = lose_sprite->getContentSize();
    main_button->setPosition(Vec2(
                this->get_center_pos().x,
                this->get_center_pos().y-sy(275)
    ));
    main_button->setAnchorPoint(Vec2(0.5f, 0.5f));

    log("You've lost the level!");
};

void Beatup::detect_combo(FistHands hand)
{
    this->punch_log->add_punch(hand);

    for (auto combo : *this->combos)
    {
        if (!combo->get_been_bought() || !combo->can_punch()) 
        {
            continue;
        }

        else if (combo->handle_hand(hand))
        {
            //This is where combo action happens
            Face* face = this->get_target_face(); 
            if (!face) { return; }

            face->is_crit_hit = true;
            float shake_time = combo->shake_time;
            this->shake_clock->set_threshold(shake_time); //TODO should this start the clock?
            face->shake_clock->set_threshold(shake_time);

            this->left_fist->punch_clock->start_for_thres(shake_time);
            this->left_fist->hit_sprite->setPosition(this->left_fist->sprite->getPosition());
            this->left_fist->hit_sprite->setVisible(true);
            this->left_fist->moving_sprite->setVisible(false);

            this->right_fist->punch_clock->start_for_thres(shake_time);
            this->right_fist->hit_sprite->setPosition(this->right_fist->sprite->getPosition());
            this->right_fist->hit_sprite->setVisible(true);
            this->right_fist->moving_sprite->setVisible(false);

            combo->cooldown_clock->start_for_thres(combo->cooldown);
            combo->can_activate = false;

            this->is_comboing = true;

            face->end_charge_attack();

            // Coin::spawn(face, this);
        };
    };
};

bool Beatup::should_shake()
{
    bool result =  this->shake_clock->is_active();
    //CCLOG("should shake? %i", result);
    return result;
};

void Beatup::set_target_face(Face* target)
{
    Face* old_target = this->get_target_face();
    if (old_target != NULL)
    {
        old_target->swap_center(target);
        // old_target->get_sprite()->setOpacity(155);
        //old_target->setCascadeColorEnabled(true);
        //old_target->setOpacityModifyRGB(true);
        //old_target->setOpacity(200);
    }
    this->target_face = target;
    SoundEngine::play_sound("sounds\\old\\Swoosh.mp3");
    // target->get_sprite()->setOpacity(255);
};

Face* Beatup::get_target_face()
{
    if (this->target_face == NULL || this->target_face->removed) { return NULL; };
    return this->target_face;
};

void Beatup::flash_color_on_face(float total_duration, Color3B flash_color, Color3B reset_color)
{
    Face* face = this->get_target_face();
    if (face != NULL)
    {
        if (flash_color == Color3B::WHITE)
        {

            auto flash = [this](){
                this->get_target_face()->recovering_sprite->getGLProgramState()->setUniformFloat("u_amount", 1.75f);
                return;
            };
            auto delay = DelayTime::create(total_duration);
            auto reset_flash = [this](){
                this->get_target_face()->recovering_sprite->getGLProgramState()->setUniformFloat("u_amount", 1.0f);
                return;
            };
            face->runAction(Sequence::create(
                CallFunc::create(flash),
                delay,
                CallFunc::create(reset_flash),
                NULL
            ));
        }
        else
        {
            face->runAction(Sequence::create(
                TintTo::create(total_duration/2, flash_color),
                TintTo::create(total_duration/2, reset_color),
                NULL
            ));
        };
    };
};

void Beatup::print_inventory()
{
    this->hide_ui();

    Size visibleSize = Director::getInstance()->getVisibleSize();
    float y = visibleSize.height - sy(50);

    auto make_label = [&](std::string name)
    {
        auto farm = this->buildup->city->building_by_name("The " + name);
        std::string lbl_string = name + " " + farm->get_inventory();
        std::string spc_string = " " + farm->get_specifics();
        if (this->getChildByName(name))
        {
            ((Label*)this->getChildByName(name))->setString(lbl_string);
            ((Label*)this->getChildByName(name + "_specific"))->setString(spc_string);
        }
        else
        {
            auto farm_inv_lbl = Label::createWithTTF(lbl_string, DEFAULT_FONT, 24);
            farm_inv_lbl->setName(name);
            farm_inv_lbl->setAnchorPoint(Vec2(0, 0));
            farm_inv_lbl->setPosition(0, y);
            this->addChild(farm_inv_lbl);

            y -= sy(25);
            auto farm_inv_spc_lbl = Label::createWithTTF(spc_string, DEFAULT_FONT, 24);
            farm_inv_spc_lbl->setName(name + "_specific");
            farm_inv_spc_lbl->setAnchorPoint(Vec2(0, 0));
            farm_inv_spc_lbl->setPosition(0, y);
            this->addChild(farm_inv_spc_lbl);
        };

        y -= sy(25);
    };

    make_label("Farm");
    make_label("Dump");
    make_label("Workshop");
    make_label("Marketplace");
    make_label("Arena");
    make_label("Mine");
    make_label("Graveyard");
    make_label("Underscape");
    make_label("Forest");


    // farm_inv_lbl->setPosition(100, 100);


}

void Beatup::onKeyReleased(EventKeyboard::KeyCode keyCode, Event*)
{
    if(keyCode == EventKeyboard::KeyCode::KEY_BACK || keyCode == EventKeyboard::KeyCode::KEY_ESCAPE) 
    {
        Beatup::back_to_menu();
    }
    else if(keyCode == EventKeyboard::KeyCode::KEY_Q || keyCode == EventKeyboard::KeyCode::KEY_1) 
    {
        this->left_fist->punch();
    }
    else if(keyCode == EventKeyboard::KeyCode::KEY_E || keyCode == EventKeyboard::KeyCode::KEY_3) 
    {
        this->right_fist->punch();
    }
    else if(keyCode == EventKeyboard::KeyCode::KEY_R || keyCode == EventKeyboard::KeyCode::KEY_4) 
    {
        if (this->get_target_face())
            this->get_target_face()->disable_shield();

        auto farm = this->buildup->city->building_by_name("The Farm");
        Recipe recipe = Recipe("pileofgrain");
        recipe.components[Ingredient::IngredientType::Grain] = 10;
        if (recipe.is_satisfied(farm->ingredients))
        {
            farm->create_resources(Resource::Ingredient, 1, "PileOfGrain");
            print1("Created a pile of grain)");
            recipe.consume(farm->ingredients);
        };
    }
    else if(keyCode == EventKeyboard::KeyCode::KEY_F) 
    {
        if (this->get_target_face())
            this->get_target_face()->remove_all_features();
    }
    else if(keyCode == EventKeyboard::KeyCode::KEY_W) 
    {
        Alert* q_alert = dynamic_cast<Alert*>(this->getChildByName("quest_alert"));
        if (q_alert)
        {
            q_alert->shrink_close(Vec2(0, 0));
        }
    }
    else if(keyCode == EventKeyboard::KeyCode::KEY_C) 
    {
        int new_val = this->get_total_coins()*10;
        DataManager::set_int_from_data(Beatup::total_coin_key, new_val);
    }
    else if(keyCode == EventKeyboard::KeyCode::KEY_TAB) 
    {
        // this->get_target_face()->recovering_sprite->getGLProgramState()->setUniformFloat("u_amount", 1.0f);
        this->cycle_next_face();
    }
    else if(keyCode == EventKeyboard::KeyCode::KEY_F1) 
    {
            auto glView = Director::getInstance()->getOpenGLView();
            glView->setFrameSize(1920, 1080);
            glView->setDesignResolutionSize(1920, 1080, ResolutionPolicy::SHOW_ALL);
            // Director::getInstance()->popToRootScene();
    }
    else if(keyCode == EventKeyboard::KeyCode::KEY_F2) 
    {
            auto glView = Director::getInstance()->getOpenGLView();
            glView->setFrameSize(960, 640);
            glView->setDesignResolutionSize(960, 640, ResolutionPolicy::SHOW_ALL);
            // Director::getInstance()->popToRootScene();
    }
    else if(keyCode == EventKeyboard::KeyCode::KEY_SPACE) 
    {
        // this->get_target_face()->recovering_sprite->getGLProgramState()->setUniformFloat("u_amount", 4.0f);
        this->toggle_is_blocking();
    }
    else if (keyCode == EventKeyboard::KeyCode::KEY_Z)
    {

        // Face* face = this->get_target_face();
        // face->break_all_features();

        // this->create_quest_alert();

        //this->quest->do_unlock();
        // this->fist_flame->set_been_bought(true);
        // this->fist_frost->set_been_bought(true);
        // this->fist_psionic->set_been_bought(true);
        //this->check_quest();

        this->print_inventory();
    }
    else if(keyCode == EventKeyboard::KeyCode::KEY_X) 
    {
        Face* face = this->get_target_face(); 
        if (face != NULL)
        {
            face->kill();
        };
    }
    else if(keyCode == EventKeyboard::KeyCode::KEY_V) 
    {
        Face* face = this->get_target_face(); 
        if (face != NULL)
        {
            face->charge_clock->start_time = 999;
            face->last_attack_clock->start_time = 999;
        };
        this->player_hp = -1;
    }
    else if(keyCode == EventKeyboard::KeyCode::KEY_S) 
    {
        this->switch_to_shop();
    }
    else if(keyCode == EventKeyboard::KeyCode::KEY_B) 
    {
        float scale = 1.0;
        this->left_fist->speed_flames(scale);
        this->right_fist->speed_flames(scale);
    }
    else if(keyCode == EventKeyboard::KeyCode::KEY_N) 
    {
        float scale = 2.0;
        this->left_fist->speed_flames(scale);
        this->right_fist->speed_flames(scale);
    }
    else if(keyCode == EventKeyboard::KeyCode::KEY_M) 
    {
        float scale = 4.0;
        this->left_fist->speed_flames(scale);
        this->right_fist->speed_flames(scale);
    }
}

void Beatup::update(float dt)
{

    GameLayer::update(dt);

    if (!SoundEngine::get_playing_music())
    {
        SoundEngine::play_music("music\\in_fight.mp3");
    }

    if (this->level_been_lost)
    {
        this->lose_level();
        return;
    };

    if (this->get_level_over())
    {
        this->setPosition(0, 0);
    };

    this->spawn_coin_clock->update(dt);

    for (auto combo : *this->combos)
    {
        combo->update(dt);
    };

    if (this->block_clock->is_active())
    {
        this->block_clock->update(dt);
    }
    else
    {
        this->set_is_blocking(false);
    };

    Face* targetted_face = this->get_target_face();

    if (targetted_face != NULL && this->should_shake())
    {
        int shake_intensity = 20;
        GLubyte opacity = 75;
        if (targetted_face->is_crit_hit)
        {
            shake_intensity *= 4;
            opacity *= 4;

            if (this->is_comboing && this->flurry_switch_clock->passed_threshold())
            {
                //every time the fists move, deal some damage
                if (!targetted_face->has_shield_active())
                {
                    int dmg = (int)this->left_fist->get_punch_dmg(); //TODO use the proper fist
                    targetted_face->take_damage(dmg);
                    targetted_face->spawn_dmg_lbl(dmg);
                };

                auto move_to_crit_punch = [](Fist* fist) {
                    fist->hit_sprite->setPosition(fist->get_punch_pos());
                    fist->sprite->setVisible(false);
                    fist->hit_sprite->setVisible(true);
                    fist->moving_sprite->setVisible(false);

                    fist->other->reset_pos();
                };

                if (this->flurry_with_left_hand)
                {
                    move_to_crit_punch(this->left_fist);
                    this->flurry_with_left_hand = false;
                }
                else
                {
                    move_to_crit_punch(this->right_fist);
                    this->flurry_with_left_hand = true;
                }
                this->flurry_switch_clock->reset();
            }
            else
            {
                this->flurry_switch_clock->update(dt);

                auto hide_and_move_hit_show_moving = [](Fist* fist){
                    fist->hit_sprite->setPosition(fist->sprite->getPosition());
                    fist->hit_sprite->setVisible(false);
                    fist->moving_sprite->setVisible(true);
                };

                hide_and_move_hit_show_moving(this->left_fist);
                hide_and_move_hit_show_moving(this->right_fist);

            };

            //during a combo, only spawn a coin so often
            if (this->spawn_coin_clock->passed_threshold())
            {
                if (!targetted_face->has_shield_active())
                {
                    //punch for no damage, for shake and sounds etc
                    Coin::spawn(targetted_face, targetted_face->attacker, false);
                    int punch_dmg = 0;
                    targetted_face->hit(punch_dmg);
                    Coin::spawn(targetted_face, targetted_face->attacker, false);
                    targetted_face->hit(punch_dmg);
                }
                else
                {
                    targetted_face->shield->play_hit();
                };

                this->spawn_coin_clock->reset();

            };
        };

        this->shake_screen(shake_intensity);

        this->shake_clock->update(dt);

        this->shake_color(opacity);

    }
    else
    {
        if (targetted_face != NULL)
        {
            this->reset();
        }
        else if (targetted_face == NULL && this->has_been_initialized && this->level_been_won == false)
        {
            this->level_been_won = true;
        };
    };

    for (Face* face : *this->faces)
    {
        face->update(dt);
    }

    this->stamina_clock->update(dt);
    if (this->stamina_clock->passed_threshold())
    {
        this->add_to_stamina(this->stamina_regen_rate);
        this->stamina_clock->reset();
    };
    float percentage = this->stamina_count/this->stamina_max;
    this->stamina_prog->scroll_to_percentage(percentage);

    this->left_fist->update(dt);
    this->right_fist->update(dt);

    this->update_coins(dt);

    // this->update_coins_lbl(); //cant run this because we have the fancy coin label thing
    this->update_player_hp_bar();

    this->rocket->update(dt);
    this->grenade->update(dt);

    this->fist_flame->update(dt);
    this->fist_frost->update(dt);
    this->fist_psionic->update(dt);

    if (this->quest != NULL)
    {
        if (!this->quest_completed && this->quest->get_is_satisfied())
        {
            this->quest_completed = true;
        };
    };

};

void Beatup::update_coins_lbl(bool do_bump, unsigned int total_coins)
{
    std::stringstream ss;
    if (total_coins == -321)
    {
        total_coins = this->get_total_coins();
    };
    ss << "$ " << total_coins;

    std::string old_string = this->coins_lbl->getString();
    if (old_string != ss.str())
    {
        this->coins_lbl->setString(ss.str());

        if (do_bump)
        {
            float duration = 0.125f;
            TintTo* tint_to = TintTo::create(duration, 254, 233, 34);
            TintTo* tint_back = TintTo::create(duration, Color3B::WHITE);
            ScaleTo* sb = ScaleTo::create(duration, 0.6f);
            ScaleTo* bs = ScaleTo::create(duration, 0.2f);
            Sequence* seq = Sequence::create(tint_to, sb, tint_back, bs, NULL);

            this->coins_lbl->runAction(seq);
        };
    };
};


void Beatup::update_coins(float dt)
{
    auto coins_to_remove = std::vector<Coin*>();

    for (auto i: *this->coins)
    {
        Coin* coin = i;
        bool should_remove = coin->update(dt);
        if (should_remove)
        {
            coins_to_remove.push_back(i);
        };
    };

    for (auto _: coins_to_remove)
    {
        this->coins->erase(
                std::remove_if(
                    this->coins->begin(),
                    this->coins->end(),
                    [&](Coin* coin)-> bool {
                    bool should = coin->should_remove;
                    return should;
                    }),
                this->coins->end());
    };
    for (auto i: coins_to_remove)
    {
        delete i;
    };

};

void Beatup::hide_ui()
{
    this->player_hp_bar->setVisible(false);
    this->stamina_prog->setVisible(false);

    this->shop_lbl->setVisible(false);
    this->coins_lbl->setVisible(false);

    this->title_lbl->setVisible(false);

    this->shop_banner->setVisible(false);

    this->flames_button->setVisible(false);
    this->frost_button->setVisible(false);
    this->psionic_button->setVisible(false);
};

void Beatup::win_level()
{
    this->level_been_won = true;

    auto resize_btn = [](ui::Button* button) {
        auto lbl_size = button->getTitleRenderer()->getContentSize();
        button->setContentSize(Size(lbl_size.width * 1.5f, lbl_size.height * 2.5f));
    };

    Sprite* win_sprite = Sprite::createWithSpriteFrameName("win.png");
    win_sprite->setPosition(this->get_center_pos());
    win_sprite->setScale(sx(4));
    win_sprite->getTexture()->setAliasTexParameters();
    Sprite* win_face = Sprite::createWithSpriteFrameName("win_face.png");
    win_face->setAnchorPoint(Vec2(0.5f, 0.5f));
    win_face->setPosition(Vec2(
        win_sprite->getContentSize().width/2,
        win_sprite->getContentSize().height/2+5
        ));
    win_sprite->addChild(win_face);
    this->addChild(win_sprite);

    float rotation_angle = 2.5f;
    float rotate_time = 0.125f;
    win_face->runAction(RepeatForever::create(Sequence::create(
        RotateTo::create(rotate_time, rotation_angle),
        RotateTo::create(rotate_time, -2*rotation_angle),
        DelayTime::create(0.1f),
        RotateTo::create(rotate_time, rotation_angle),
        RotateTo::create(rotate_time, 2*rotation_angle),
        RotateTo::create(rotate_time, 0.0f),
        RotateTo::create(rotate_time, rotation_angle),
        RotateTo::create(rotate_time, -2*rotation_angle),
        DelayTime::create(0.1f),
        RotateTo::create(rotate_time, rotation_angle),
        RotateTo::create(rotate_time, 2*rotation_angle),
        RotateTo::create(rotate_time, 0.0f),
        DelayTime::create(1.5f),
        NULL)
        ));


    ui::Button* main_button = ui::Button::create();
    main_button->loadTextureNormal("main_UI_export_10_scale9_x4.png", ui::TextureResType::PLIST);

    main_button->setTitleText("Continue");
    main_button->setScale9Enabled(true);
    main_button->setScale(sx(1)); //scale 1 but its on a x4 sprite
    main_button->setTitleFontSize(30);
    main_button->setTitleFontName(DEFAULT_FONT);
    main_button->setTitleColor(Color3B::BLACK);
    main_button->addTouchEventListener([this](Ref*, cocos2d::ui::Widget::TouchEventType){this->back_to_menu(); });
    resize_btn(main_button);

    this->addChild(main_button);
    Size win_size = win_sprite->getContentSize();
    main_button->setPosition(Vec2(
                this->get_center_pos().x,
                this->get_center_pos().y-sy(275)
    ));
    main_button->setAnchorPoint(Vec2(0.5f, 0.5f));

    this->check_quest();

    log("You've beaten the level!");

    this->hide_ui();

    // SoundEngine::play_sound("sounds\\old\\cheering.mp3");
    SoundEngine::play_sound("sounds\\new\\cheering\\C_cheering_13.mp3");

    if (this->quest->get_is_satisfied())
    {
        this->level->mark_complete();

        this->quest->do_unlock();
    };
};

void Beatup::check_quest()
{
    if (this->quest != NULL)
    {
        bool satisfied = this->quest->get_is_satisfied();
        std::stringstream ss;
        ss << std::boolalpha << satisfied;
        CCLOG("Quest: \"%s\" is satisfied: %s", this->quest->name.c_str(), ss.str().c_str());

        for (Objective* obj : *this->quest->objectives)
        {
            ss.str("");
            ss << std::boolalpha << obj->get_is_satisfied();
            CCLOG("  - \"%s\" is satisfied: %s", obj->name.c_str(), ss.str().c_str());
        }
    };
};

void Beatup::swipe_face_horizontal(float dt)
{
    Face* face = this->get_target_face();
    if (face == NULL || face->removed) { return; };

    Rect rect = face->get_swipable_rect();

    if (this->vec2_in_rect(&rect, Vec2(initialTouchPos[0], initialTouchPos[1])))
    {
        if (face->shield != NULL && face->shield->type == ShieldTypes::Human)
        {
            face->disable_shield();
        }
        else 
        {
            this->cycle_next_face();
        };
    };

};

void Beatup::swipe_face_vertical(float dt)
{
    Face* face = this->get_target_face();
    if (face == NULL || face->removed) { return; };

    Rect rect = face->get_swipable_rect();

    if (this->vec2_in_rect(&rect, Vec2(initialTouchPos[0], initialTouchPos[1])))
    {
        if (face->shield != NULL && face->shield->type == ShieldTypes::Brick)
        {
            face->disable_shield();
        }
    };
};

void Beatup::onSwipeLeft(float dt)
{
    this->swipe_face_horizontal(dt);
};

void Beatup::onSwipeRight(float dt)
{
    this->swipe_face_horizontal(dt);
};

void Beatup::onSwipeUp(float dt)
{
    this->swipe_face_vertical(dt);
};

void Beatup::onSwipeDown(float dt)
{
    this->swipe_face_vertical(dt);
};

void Beatup::reset_hits()
{
    this->get_target_face()->reset_hits();
};

void Beatup::cycle_next_face()
{
    if (this->faces->size() > 1)
    {
        std::vector<Face*>::iterator face_iter = std::find(this->faces->begin(), this->faces->end(), this->get_target_face());

        if (face_iter+1 != this->faces->end())
        {
            this->set_target_face(*(face_iter + 1));
        }
        else
        {
            this->set_target_face(*this->faces->begin());
        }
    }
    else
    {
        log("0 or 1 faces to swap between, need two or more.");
    };
};

void Beatup::toggle_is_blocking()
{
    this->set_is_blocking(!this->is_blocking);
};

void Beatup::set_is_blocking(bool val)
{
    if (val)
    {
        this->block_clock->start_for_thres(1);
    }
    else
    {
        this->block_clock->reset();
    }
    this->is_blocking = val;
};

void Beatup::shake_screen(int intensity)
{
    float x = (rand() % intensity) - 0.5f; //Generate a random x coordinate
    float y = (rand() % intensity) - 0.5f; //Do the same for the y coordinate
    this->setPosition(x, y);
};

void Beatup::shake_color(GLuint opacity)
{
    int r = (int)(200*(CCRANDOM_0_1()));
    int g = 73;
    int b = 52;
    this->color_layer->setColor(Color3B(r, g, b));
    this->color_layer->setOpacity(opacity);
};

void Beatup::reset()
{
    Face* targetted_face = this->get_target_face();
    this->setPosition(0, 0);
    if (targetted_face != NULL)
    {
        targetted_face->setRotation(0.0);
        if (targetted_face->shield != NULL)
        {
            targetted_face->shield->get_sprite()->setRotation(0.0);
        }
        targetted_face->is_crit_hit = false;
    }

    this->color_layer->setColor(Color3B(0, 0, 0));
};

void Beatup::apply_flash_shader()
{

    Texture2D::TexParams p;
    p.minFilter = GL_NEAREST;
    p.magFilter = GL_NEAREST;
    p.wrapS = GL_CLAMP_TO_EDGE;
    p.wrapT = GL_CLAMP_TO_EDGE;

    Size visibleSize = Director::getInstance()->getVisibleSize();
    Point origin = Director::getInstance()->getVisibleOrigin();
    auto center = Point(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y);

    GLProgram* prog = new GLProgram();
    prog->initWithFilenames("shaders/clouds.vs", "shaders/clouds.fs");

    prog->link();
    prog->use();

    auto amountLoc = prog->getUniformLocationForName("u_amount");
    prog->setUniformLocationWith1f(amountLoc, 4.0f);

    auto resolutionLoc = prog->getUniformLocationForName("u_resolution");
    Vec2 resolution(1.0f, 1.0f);
    float res[2] = { 1.0f, 1.0f };
    prog->setUniformLocationWith2f(resolutionLoc, res[0], res[1]);

    prog->updateUniforms();
    CHECK_GL_ERROR_DEBUG();


    ShaderCache::getInstance()->addGLProgram(prog, "clouds");

    Face* face = this->get_target_face();
    auto sprite_lists = std::vector<Sprite*>();
    sprite_lists.insert(sprite_lists.end(), face->sprite_list->begin(), face->sprite_list->end());
    sprite_lists.insert(sprite_lists.end(), face->hurt_sprite_list->begin(), face->hurt_sprite_list->end());
    sprite_lists.push_back(face->attacking_sprite);
    sprite_lists.push_back(face->recovering_sprite);

    for (Sprite* sprite : sprite_lists)
    {

        sprite->setBlendFunc(BlendFunc::ALPHA_NON_PREMULTIPLIED);
        sprite->setGLProgram(prog);

    };

    for (FacialFeature* facial : *face->features)
    {
        facial->sprite->setBlendFunc(BlendFunc::ALPHA_NON_PREMULTIPLIED);
        //facial->sprite->setGLProgram(prog);
    };

    CHECK_GL_ERROR_DEBUG();


    GLProgramState* state = GLProgramState::getOrCreateWithGLProgram(prog);
    state->setUniformFloat("u_amount", 1.0f);
    state->setUniformVec2("u_resolution", Director::getInstance()->getOpenGLView()->getDesignResolutionSize());
    prog->release();


}

bool Beatup::tutorials_enabled()
{
    return DataManager::get_bool_from_data(Beatup::tutorial_id_key, true);
};

bool Beatup::get_level_over()
{
    return this->level_been_won || this->level_been_lost;
};

void Beatup::add_to_stamina(float val)
{
    this->stamina_count += val;
    if (this->stamina_count > this->stamina_max)
    {
        this->stamina_count = this->stamina_max;
        if (val > 5) //idk, dont do this for regular regen TODO make this less common
        {
            ParticleSystemQuad* stamina_regen_part = ParticleSystemQuad::create("stamina_aquired.plist");
            this->addChild(stamina_regen_part);
        };
    };
};

void Beatup::reload_resources()
{
#ifdef __ANDROID__
#else
    system("xcopy \"C:\\Users\\Primary\\workspace\\beatupisis\\proj.win32\\..\\Resources\" \"C:\\Users\\Primary\\workspace\\beatupisis\\proj.win32\\Debug.win32\\\" /E /I /F /Y  /D /K ");
#endif
};

unsigned int Beatup::get_activation_count()
{
    unsigned int activates = 0;
    for (Combo* combo : *this->combos)
    {
        activates += combo->activate_count;
    };

    return activates;
};

void Beatup::create_quest_alert()
{
    if (this->quest == NULL)
    {
        CCLOG("no quest");
        return;
    };
    
    Vec2 origin = Director::getInstance()->getVisibleOrigin();
    Size visibleSize = Director::getInstance()->getVisibleSize();

    Vec2 close_button_pos = Vec2( //FIXME use close_button's pos
        origin.x + visibleSize.width - 20,
        origin.y + 20
    );

    if (this->getChildByName("quest_alert") != NULL)
    {
        // CCLOG("quest alert already exists");
        Alert* alert = dynamic_cast<Alert*>(this->getChildByName("quest_alert"));
        alert->shrink_close(close_button_pos);
        return;
    }
    CCLOG("Creating quest alert");

    Alert* alert = Alert::create();
    alert->setName("quest_alert");

    alert->set_header_str(this->quest->name);
    alert->set_sub_header_str(this->quest->unlock.description);

    std::stringstream ss;

    for (Objective* obj : *this->quest->objectives)
    {
        ss << "- " << obj->name << std::endl;
        if (obj->get_is_satisfied())
            ss << "  (satisfied)";
        else
            ss << "  (incomplete)";
        ss << std::endl;
    }
    alert->set_body_str(ss.str());

    alert->setPosition(close_button_pos);

    alert->setScale(0.0f);
    alert->grow_open(this->get_center_pos());
    alert->setAnchorPoint(Vec2(0.5f, 0.5f));

    // alert->setScaleX(sx(1));
    // alert->setScaleY(sy(1));

    auto resize_btn = [](ui::Button* button) {
        auto lbl_size = button->getTitleRenderer()->getContentSize();

        button->ignoreContentAdaptWithSize(true);
        auto lbl = button->getTitleRenderer();
        button->setContentSize(
            Size(
                lbl_size.width * 1.5f,
                lbl_size.height * 1.5f
            )
        );
        lbl->setDimensions(lbl_size.width*1.5f, lbl_size.height*1.5f);

    };

    auto create_btn = [&](std::string msg, ui::RelativeLayoutParameter* param) {
        auto button = ui::Button::create();
        button->setTitleText(msg);
        button->setTitleFontName(DEFAULT_FONT);
        button->setTitleFontSize(40.0f);
        button->setTitleColor(Color3B::BLACK);

        resize_btn(button);

        button->setScale9Enabled(true);
        button->loadTextureNormal("main_UI_export_10_scale9_x4.png", ui::TextureResType::PLIST);

        button->ignoreContentAdaptWithSize(false); //word wrap or something

        button->setLayoutParameter(param);

        alert->addChild(button);

        return button;
    };

    std::string unlock_str = "Unlock Reward!";
    if (this->quest->unlock.description == "No unlocks QUEST DESC")
    {
        unlock_str = "Sucess!";
    };

    if (this->quest->get_is_completed())
    {
        unlock_str = "Already unlocked!";
    };


    ui::RelativeLayoutParameter* center_param = ui::RelativeLayoutParameter::create();
    center_param->setAlign(ui::RelativeAlign::LOCATION_ABOVE_CENTER);
    center_param->setMargin(ui::Margin(sx(10), sy(10), sx(10), sy(30)));
    center_param->setRelativeToWidgetName("done_btn");
    ui::Button* unlock_btn = create_btn(unlock_str, center_param);
    unlock_btn->setScale(sx(1.0f));


    unlock_btn->setEnabled(this->quest->get_is_satisfied());
    if (this->quest->get_is_completed())
    {
        unlock_btn->setEnabled(false);
    }
    //unlock_btn->setTitleColor(Color3B::GREEN);


    if (!this->quest->get_is_satisfied())
    {
        unlock_btn->setVisible(false);

        if (!unlock_btn->isEnabled())
        {
            unlock_btn->setTitleText("Not yet completed");
            unlock_btn->setTitleColor(Color3B::RED);
            unlock_btn->loadTextureNormal("");
            resize_btn(unlock_btn);
        };
    };

    unlock_btn->addTouchEventListener([this, unlock_btn, alert](Ref* sender, ui::Widget::TouchEventType type)
    {
        if (type == ui::Widget::TouchEventType::ENDED)
        {
            for (Objective* obj : *this->quest->objectives)
            {
                //obj->win_func(this);
            }

            SoundEngine::play_sound("sounds\\new\\coin\\C_coin_1.mp3");
            this->quest->do_unlock();

            unlock_btn->setVisible(false);

            Vec2 origin = Director::getInstance()->getVisibleOrigin();
            Size visibleSize = Director::getInstance()->getVisibleSize();
            Vec2 pos = Vec2(
                origin.x + visibleSize.width - 20,
                origin.y + 20
                );
            alert->shrink_close(pos);
        };
    });

    this->addChild(alert, 900); //on top of everything
};
