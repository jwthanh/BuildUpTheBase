#include "Beatup.h"

#include "assert.h"
#include <iostream>


#include "Clock.h"
#include "Face.h"
#include "SoundEngine.h"
#include "BaseMenu.h"
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
#include "Recipe.h"
#include "attribute.h"
#include "FShake.h"

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

    // beatup->create_quest_alert();

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

    this->buildup = new Buildup();
    this->buildup->beatup = this;

    this->face_fight_node = Node::create();
    this->face_fight_node->setPositionY(this->get_center_pos().y/2);
    this->addChild(face_fight_node);

    auto player = new Player("Jimothy");
    player->coins = 100;
    this->buildup->player = player;

    auto animal = std::make_shared<Animal>("Tank");

    this->buildup->city = Buildup::init_city(this->buildup);
    this->buildup->city->update_buildings(0);

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
    this->punches_to_coin = 1;
    this->punch_count = 0;

    this->shake_clock = new Clock(0.0f);

    this->spawn_coin_clock = new Clock(0.15f);


    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();


    int side_height = (int)sy(100);

    auto quest_button = MenuItemSprite::create(
        Sprite::createWithSpriteFrameName("back_banner.png"),
        Sprite::createWithSpriteFrameName("back_banner.png"),
        std::bind(&Beatup::cycle_next_building, this, false)
    );

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

    //this->update(0.001f); //dont remember what this was for

    this->setup_commands();

    this->has_been_initialized = true;

    auto gold__plistFile = FileUtils::getInstance()->fullPathForFilename("gold.plist");
    Coin::particle_map = FileUtils::getInstance()->getValueMapFromFile(gold__plistFile.c_str());

    auto _plistFile = FileUtils::getInstance()->fullPathForFilename("blood.plist");
    Gore::particle_map = FileUtils::getInstance()->getValueMapFromFile(_plistFile.c_str());

    this->hide_ui();

    this->schedule(schedule_selector(Beatup::update_buildup));

    return true;
}

void Beatup::update_buildup(float dt)
{
    this->buildup->update(dt);

    //for fighter and not my dudes, update the fighter nodes
    auto arena = this->buildup->city->building_by_name("The Arena");
    for (auto fighter : arena->fighters) {
        if (fighter != this->buildup->fighter) {

        };
    };
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

int Beatup::get_total_coins()
{
    return DataManager::get_int_from_data(Beatup::total_coin_key);
};

void Beatup::add_total_coin(int x)
{
    this->temp_coins += x;
    DataManager::incr_key("total_coin_key", x);

    spBuilding target_building = this->buildup->target_building;
    target_building->create_resources(Resource::Ingredient, x, target_building->punched_ingredient_type);
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

void Beatup::switch_to_building_menu()
{
    auto scene = Scene::create();
    BuildingMenu* building_menu = BuildingMenu::create(this->buildup->target_building);
    scene->addChild(building_menu);

    auto director = Director::getInstance();
    director->pushScene(scene);
};

void Beatup::switch_to_city_menu()
{
    auto scene = Scene::create();
    CityMenu* building_menu = CityMenu::create(this->buildup->target_building);
    scene->addChild(building_menu);

    auto director = Director::getInstance();
    director->pushScene(scene);
};

void Beatup::switch_to_inventory_menu()
{
    auto scene = Scene::create();
    InventoryMenu* building_menu = InventoryMenu::create(this->buildup->target_building);
    scene->addChild(building_menu);

    auto director = Director::getInstance();
    director->pushScene(scene);
};

void Beatup::switch_to_character_menu(spFighter fighter)
{
    if (fighter == NULL)
    {
        printj("fighter is null, no character menu");
        return;
    }

    auto scene = Scene::create();
    CharacterMenu* building_menu = CharacterMenu::create(fighter);
    scene->addChild(building_menu);

    auto director = Director::getInstance();
    director->pushScene(scene);
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
        this->shop = BaseMenu::create();
        this->shop->beatup = this;
        this->shop->menu_init();
        this->shop->update(0);

        this->shop_scene->addChild(this->shop);
        this->shop->beatup_scene = director->getRunningScene();
    };

    this->shop->set_main_lbl();

    director->pushScene(this->shop_scene);
};

bool Beatup::onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event)
{
    GameLayer::onTouchBegan(touch, event);
    auto touch_loc = touch->getLocation();

    auto get_sprite_rect = [&](Node* parent_node, Sprite* sprite)
    {
        auto pos = parent_node->convertToWorldSpace(sprite->getPosition());
        auto size = sprite->getContentSize();

        //NOTE: scale by parent as well
        auto width = size.width * sprite->getScaleX()* parent_node->getScaleX();
        auto height = size.height * sprite->getScaleY() * parent_node->getScaleY();

        auto rect = Rect(pos.x-width/2, pos.y-height/2, width, height);

        return rect;
    };

    if (!this->getChildByName("quest_alert"))
    {
        for (Fist* fist : { this->left_fist, this->right_fist })
        {
            auto fist_rect = get_sprite_rect(this->face_fight_node, fist->sprite);
            if (vec2_in_rect(&fist_rect, touch_loc) && !fist->is_punching)
            {
                if (!this->is_blocking)
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

    if (this->get_target_face())
    {
        Rect face_rect = get_sprite_rect(this->get_target_face(), this->get_target_face()->get_sprite());

        if (vec2_in_rect(&face_rect, touch_loc)) {
            this->switch_to_city_menu();
            return false;
        };
    }

    if ((touch_in_node(this->player_hp_bar->front_timer, touch, 1.5) || touch_in_node(this->player_hp_bar->back_timer, touch, 1.5) ||
                touch_in_node(this->stamina_prog->front_timer, touch, 1.5) || touch_in_node(this->stamina_prog->back_timer, touch, 1.5)) 
            && !this->right_fist->is_punching)
    {
        this->toggle_is_blocking();
        return false;
    }

    for (auto node : {this->fighter_node, this->brawler_node, this->enemy_node}){
        Rect fighter_rect = get_sprite_rect(node, node->sprite);
        if (vec2_in_rect(&fighter_rect, touch->getLocation()))
        {
            this->switch_to_character_menu(node->fighter);
        }

    }

    //debug
    // auto layer_color = LayerColor::create(Color4B::RED);
    // layer_color->setContentSize(fighter_rect.size);
    // layer_color->setPosition(fighter_rect.origin);
    // this->addChild(layer_color);


    return true; //determines whether the touch keeps going or something
};

void Beatup::onTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event)
{
    GameLayer::onTouchEnded(touch, event);

    // log_vector(touch->getLocation(), "touch");
    // this->face_fight_node->setPosition(touch->getLocation());

    if (!this->getChildByName("quest_alert"))
    {
        //quest alert active, don't update
        for (Fist* fist : { this->left_fist, this->right_fist })
        {
            auto get_sprite_rect = [&](Node* parent_node, Sprite* sprite)
            {
                auto pos = parent_node->convertToWorldSpace(sprite->getPosition());
                auto size = sprite->getContentSize();

                //NOTE: scale by parent as well
                auto width = size.width * sprite->getScaleX()* parent_node->getScaleX();
                auto height = size.height * sprite->getScaleY() * parent_node->getScaleY();

                auto rect = Rect(pos.x-width/2, pos.y-height/2, width, height);

                return rect;
            };
            if (vec2_in_rect(&get_sprite_rect(this, fist->sprite), touch->getLocation()) && !fist->is_punching)
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
    face->beatup = this; //unnecessary?

    this->node_grid = NodeGrid::create();
    this->node_grid->setPosition(0, 0);
    this->node_grid->addChild(face);


    this->face_fight_node->addChild(node_grid);
    this->face_fight_node->setScale(0.5f);

    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

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
    this->face_fight_node->addChild(this->left_fist_node_grid, 10);

    this->right_fist_node_grid = NodeGrid::create();
    this->face_fight_node->addChild(this->right_fist_node_grid, 10);

    this->left_fist->other = this->right_fist;
    this->left_fist_node_grid->addChild(this->left_fist->sprite);
    this->left_fist_node_grid->addChild(this->left_fist->moving_sprite);
    this->left_fist_node_grid->addChild(this->left_fist->hit_sprite);

    this->right_fist->other = this->right_fist;
    this->right_fist_node_grid->addChild(this->right_fist->sprite);
    this->right_fist_node_grid->addChild(this->right_fist->moving_sprite);
    this->right_fist_node_grid->addChild(this->right_fist->hit_sprite);

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

    this->player_hp_bar = new ProgressBar(this, "healthbar_bar.png", "healthbar.png");
    this->player_hp_bar->back_timer->setSprite(Sprite::createWithSpriteFrameName("healthbar_bar_white.png"));
    this->player_hp_bar->setAnchorPoint(Vec2(0.5f, 0.5f));
    this->player_hp_bar->setPosition(Vec2(
        this->get_center_pos().x,
        this->player_hp_bar->front_timer->getSprite()->getContentSize().height*2
    ));
    this->player_hp_bar->setScale(sx(5));

    this->update_player_hp_bar();

    this->stamina_clock = new Clock(0.25f);
    this->stamina_max = 80000;
    this->stamina_count = this->stamina_max;

    this->stamina_prog = new ProgressBar(this, "combo_bar.png", "");
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


    this->enemy_node = FighterNode::create(this, NULL);
    this->enemy_node->setPosition(this->get_center_pos(sx(350), sy(0)));

    this->fighter_node = FighterNode::create(this, this->buildup->fighter);
    this->fighter_node->setPosition(this->get_center_pos(sx(200), sy(50)));

    this->brawler_node = FighterNode::create(this, this->buildup->brawler);
    this->brawler_node->setPosition(this->get_center_pos(sx(200), sy(-150)));

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

bool Beatup::should_shake()
{
    bool result =  this->shake_clock->is_active();
    //CCLOG("should shake? %i", result);
    return result;
};

void Beatup::set_target_building(spBuilding target)
{
    spBuilding old_target = this->get_target_building();
    if (old_target != NULL)
    {
        // old_target->swap_center(target);
    }
    this->buildup->target_building = target;
    SoundEngine::play_sound("sounds\\old\\Swoosh.mp3");
};

spBuilding Beatup::get_target_building()
{
    if (this->buildup->target_building == NULL ) { return NULL; };
    return this->buildup->target_building;
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
        auto building = this->buildup->city->building_by_name("The " + name);
        std::string lbl_string = (this->get_target_building() == building ? ">>" : "") + name + " " + building->get_inventory();
        std::string spc_string = " " + building->get_specifics();

        auto lbl = this->getChildByName(name);
        auto specific = this->getChildByName(name + "_specific");
        if (lbl)
        {
            ((Label*)lbl)->setString(lbl_string);
            ((Label*)specific)->setString(spc_string);
            lbl->setVisible(true);
            specific->setVisible(true);
        }
        else
        {
            auto building_inv_lbl = Label::createWithTTF(lbl_string, DEFAULT_FONT, 24);
            building_inv_lbl->setName(name);
            building_inv_lbl->setAnchorPoint(Vec2(0, 0));
            building_inv_lbl->setPosition(0, y);
            this->addChild(building_inv_lbl);

            y -= sy(25);
            auto building_inv_spc_lbl = Label::createWithTTF(spc_string, DEFAULT_FONT, 24);
            building_inv_spc_lbl->setName(name + "_specific");
            building_inv_spc_lbl->setAnchorPoint(Vec2(0, 0));
            building_inv_spc_lbl->setPosition(0, y);
            this->addChild(building_inv_spc_lbl);
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

}

void Beatup::hide_inventory() 
{
    std::vector<std::string> building_names = {
        "Farm",
        "Dump",
        "Workshop",
        "Marketplace",
        "Arena",
        "Mine",
        "Graveyard",
        "Underscape",
        "Forest"
    };

    for (auto name : building_names) {
        auto lbl = this->getChildByName(name);
        if (lbl) {
            lbl->setVisible(false);
        };

        auto specific_lbl = this->getChildByName(name+"_specific");
        if (specific_lbl) {
            specific_lbl->setVisible(false);
        };
    };
};

void Beatup::onKeyReleased(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* pEvent)
{
    this->key_state[keyCode] = false;

    if(keyCode == EventKeyboard::KeyCode::KEY_BACK ) 
    {
        Beatup::back_to_menu();
        printj("esc/back key released");
    }
};

void Beatup::onKeyPressed(EventKeyboard::KeyCode keyCode, Event* evt)
{
    this->key_state[keyCode] = true;

    if(keyCode == EventKeyboard::KeyCode::KEY_BACK || keyCode == EventKeyboard::KeyCode::KEY_ESCAPE) 
    {
        Beatup::back_to_menu();
        printj("esc/back key pressed");
    }
    else if(keyCode == EventKeyboard::KeyCode::KEY_Q)
    {
        this->left_fist->punch();
    }
    else if(keyCode == EventKeyboard::KeyCode::KEY_E)
    {
        this->right_fist->punch();
    }
    else if(keyCode == EventKeyboard::KeyCode::KEY_R)
    {
        if (this->get_target_face())
            this->get_target_face()->disable_shield();

        auto farm = this->buildup->city->building_by_name("The Farm");
        Recipe recipe = Recipe("pileofgrain");
        recipe.components[Ingredient::IngredientType::Grain] = 10;
        if (recipe.is_satisfied(farm->ingredients))
        {
            farm->create_resources(Resource::Ingredient, 1, "PileOfGrain");
            printj1("Created a pile of grain)");
            recipe.consume(farm->ingredients);
        };
    }
    else if(keyCode == EventKeyboard::KeyCode::KEY_F) 
    {
        this->switch_to_city_menu();
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
        bool shift_pressed;
        try {
            shift_pressed = this->key_state.at(cocos2d::EventKeyboard::KeyCode::KEY_SHIFT);
        } catch (std::out_of_range)
        {
            shift_pressed = false;
        }
        this->cycle_next_building(shift_pressed);
    }
    else if(keyCode == EventKeyboard::KeyCode::KEY_F1) 
    {
            auto glView = Director::getInstance()->getOpenGLView();
            glView->setFrameSize(1920, 1080);
            glView->setDesignResolutionSize(1920, 1080, ResolutionPolicy::SHOW_ALL);
    }
    else if(keyCode == EventKeyboard::KeyCode::KEY_F2) 
    {
            auto glView = Director::getInstance()->getOpenGLView();
            glView->setFrameSize(960, 640);
            glView->setDesignResolutionSize(960, 640, ResolutionPolicy::SHOW_ALL);
    }
    else if(keyCode == EventKeyboard::KeyCode::KEY_F3) 
    {
    }
    else if(keyCode == EventKeyboard::KeyCode::KEY_F4) 
    {
    }
    else if(keyCode == EventKeyboard::KeyCode::KEY_F5) 
    {
        this->_visible_inventory = !this->_visible_inventory;
    }
    else if(keyCode == EventKeyboard::KeyCode::KEY_SPACE) 
    {
        this->toggle_is_blocking();
    }
    else if (keyCode == EventKeyboard::KeyCode::KEY_H)
    {
        auto farm = this->buildup->city->building_by_name("The Farm");
        Recipe heal_recipe = Recipe("HealingGrain");
        heal_recipe.components[Ingredient::IngredientType::Grain] = 5;
        if (heal_recipe.is_satisfied(farm->ingredients))
        {
            auto arena = this->buildup->city->building_by_name("The Arena");
            auto fighter = arena->fighters.front();
            fighter->attrs->health->current_val += 50;
            heal_recipe.consume(farm->ingredients);
            printj("healed first fighter");
        }
    }
    else if (keyCode == EventKeyboard::KeyCode::KEY_Z)
    {
        auto mine = this->buildup->city->building_by_name("The Mine");

        Recipe* recipe = new Recipe("sword");
        recipe->components[Ingredient::IngredientType::Copper] = 2;
        auto callback = [mine](Beatup* beatup) {
            mine->create_resources(Resource::Product, 1, "Sword");
            mine->create_resources(Resource::Waste, 1, "wasted_iron");
            mine->city->buildup->beatup->fighter_node->setColor(Color3B::RED);
            mine->city->buildup->fighter->has_sword = true;
        };
        recipe->_callback = callback;

        mine->consume_recipe(recipe);
    }
    else if(keyCode == EventKeyboard::KeyCode::KEY_X) 
    {
        Face* face = this->get_target_face(); 
        if (face != NULL)
        {
            face->kill();
        };
    }
    else if(keyCode == EventKeyboard::KeyCode::KEY_GRAVE) 
    {
        this->reload_resources();
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
    }
    else if(keyCode == EventKeyboard::KeyCode::KEY_N) 
    {
    }
    else if(keyCode == EventKeyboard::KeyCode::KEY_M) 
    {
    }

    else if(keyCode == EventKeyboard::KeyCode::KEY_1) 
    {
        this->switch_to_city_menu();
    }

    else if(keyCode == EventKeyboard::KeyCode::KEY_2) 
    {
        this->switch_to_building_menu();
    }

    else if(keyCode == EventKeyboard::KeyCode::KEY_3) 
    {
        this->switch_to_inventory_menu();
    }

    else if(keyCode == EventKeyboard::KeyCode::KEY_4) 
    {
    }

    else if(keyCode == EventKeyboard::KeyCode::KEY_5) 
    {
    }

    else if(keyCode == EventKeyboard::KeyCode::KEY_6) 
    {
    }

    else if(keyCode == EventKeyboard::KeyCode::KEY_7) 
    {
    }

    else if(keyCode == EventKeyboard::KeyCode::KEY_8) 
    {
    }

    else if(keyCode == EventKeyboard::KeyCode::KEY_9) 
    {
    }

    else if(keyCode == EventKeyboard::KeyCode::KEY_0) 
    {
    }
}

void Beatup::update(float dt)
{

    GameLayer::update(dt);

    if (!SoundEngine::get_playing_music())
    {
        SoundEngine::play_music("music\\medieval.mp3");
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

            if (false)
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

                if (false)
                {
                    move_to_crit_punch(this->left_fist);
                }
                else
                {
                    move_to_crit_punch(this->right_fist);
                }
            }
            else
            {

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

        float face_rotation = this->get_target_face()->getRotation();
        this->shake_screen(shake_intensity, face_rotation <= 0);

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

    this->update_player_hp_bar();

    if (this->quest != NULL)
    {
        if (!this->quest_completed && this->quest->get_is_satisfied())
        {
            this->quest_completed = true;
        };
    };

    if (_visible_inventory) {
        this->print_inventory();
    } else {
        this->hide_inventory();
    };

    this->fighter_node->update(dt);
    this->brawler_node->update(dt);
    this->enemy_node->update(dt);
    // for (auto node : this->enemy_nodes) {
    //     node->update(dt);
    // };

    if (this->buildup->fighter->xp->lvl == 2) {
        this->fighter_node->load_new_sprite("thief8x8.png");
    }
    else if (this->buildup->fighter->xp->lvl == 3) {
        this->fighter_node->load_new_sprite("hero8x8.png");
    }
    else if (this->buildup->fighter->xp->lvl == 4) {
        this->fighter_node->load_new_sprite("badmother20x20.png");
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

void Beatup::cycle_next_building(bool reverse)
{
    vsBuilding buildings = this->buildup->city->buildings;
    if (buildings.size() > 1)
    {
        if (reverse == false)
        {
            vsBuilding::iterator face_iter = std::find(buildings.begin(), buildings.end(), this->get_target_building());

            if (face_iter + 1 != buildings.end())
            {
                this->set_target_building(*(face_iter + 1));
            }
            else
            {
                this->set_target_building(*buildings.begin());
            }

        }
        else if (reverse == true)
        {
            vsBuilding::reverse_iterator face_iter = std::find(buildings.rbegin(), buildings.rend(), this->get_target_building());

            if (face_iter + 1 != buildings.rend())
            {
                this->set_target_building(*(face_iter + 1));
            }
            else
            {
                this->set_target_building(*buildings.rbegin());
            }

        }
    }
    else
    {
        log("0 or 1 buildings to swap between, need two or more.");
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

void Beatup::shake_screen(int intensity, bool left_angle)
{
    float x = (rand() % intensity) - 0.5f; //Generate a random x coordinate
    if (!left_angle)
    {
        x += 10;
    }
    else {
        x -= 10;
    }
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
};

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

void Beatup::view_army()
{
    Scene* army_scene = Scene::create();
    GameLayer* layer = GameLayer::create();

    auto pKeybackListener = EventListenerKeyboard::create();
    pKeybackListener->onKeyPressed = CC_CALLBACK_2(GameLayer::onKeyReleased, layer);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(pKeybackListener, layer);

    for (unsigned int i = 0; i < this->buildup->city->building_by_name("The Farm")->ingredients.size(); i++) {
        Sprite* bad_mother = Sprite::createWithSpriteFrameName("townsmen8x8.png");
        bad_mother->setScale(8);
        bad_mother->setPosition(300 + CCRANDOM_0_1() * 400, 100 + CCRANDOM_0_1() * 400);
        JumpBy* jump_by = JumpBy::create(CCRANDOM_0_1()* 1.5f + 1.0f, Vec2(0, 0), CCRANDOM_0_1() * 10, 3);
        bad_mother->runAction(RepeatForever::create(Sequence::createWithTwoActions(DelayTime::create(CCRANDOM_0_1()*1.0f), jump_by)));
        layer->addChild(bad_mother);
    };
    army_scene->addChild(layer);

    Director::getInstance()->pushScene(army_scene);
};
