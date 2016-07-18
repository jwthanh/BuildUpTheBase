#include "Beatup.h"

#include "assert.h"
#include <iostream>
#include <chrono>
#include <ctime>


#include "Clock.h"
#include "SoundEngine.h"
#include "constants.h"
#include "DataManager.h"
#include "ProgressBar.h"
#include "ShatterNode.h" //is used on android

#include "HouseBuilding.h"
#include "Fighter.h"


#pragma warning(default:4267)
#pragma warning(default:4251)
#pragma warning(default:4244)
#include "Util.h"
#include "Recipe.h"
#include "attribute.h"
#include "GameLogic.h"
#include "Animal.h"
#include "attribute_container.h"
#include "combat.h"

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

	return scene;
};

std::string Beatup::total_hit_key = "total_hit_key";
std::string Beatup::total_coin_key = "total_coin_key";
std::string Beatup::last_login_key = "last_login_key";

bool Beatup::init()
{

#ifdef _WIN32
    FUNC_INIT_WIN32(Beatup);
#else
    FUNC_INIT(Beatup);
#endif

    CCLOG("beatup init");

    this->buildup = new Buildup();

    this->face_fight_node = Node::create();
    this->face_fight_node->setPositionY(this->get_center_pos().y/2);
    this->addChild(face_fight_node);

    auto player = new Player("Jimothy");
    player->coins = 100;
    this->buildup->player = player; //this shouldn't set the other class right?

    auto animal = std::make_shared<Animal>("Tank");

    this->buildup->city = Buildup::init_city(this->buildup);
    this->buildup->city->update_buildings(0);

    this->level_been_won = false;
    this->level_been_lost = false;

    this->color_layer = LayerColor::create(Color4B(0, 0, 0, 25));
    this->addChild(this->color_layer);

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

    this->setup_commands();

    this->has_been_initialized = true;

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
    auto console = Director::getInstance()->getConsole();

    auto gl = Director::getInstance()->getOpenGLView();
    auto frame_size = gl->getFrameSize();
    auto design_size = gl->getDesignResolutionSize();

    //CCLOG("frame %f %f", frame_size.width, frame_size.height);
    //CCLOG("design %f %f", design_size.width, design_size.height);


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


void Beatup::add_total_hit(int x)
{
    DataManager::incr_key("total_hit_key", x);
};

int Beatup::get_total_hits()
{
    return DataManager::get_int_from_data(Beatup::total_hit_key);
};

double Beatup::get_total_coins()
{
    return DataManager::get_double_from_data(Beatup::total_coin_key);
}

void Beatup::set_last_login()
{
    //get current time
    std::chrono::time_point<std::chrono::system_clock> time_is_now = std::chrono::system_clock::now();
    time_t rendered = std::chrono::system_clock::to_time_t(time_is_now);

    //i assume this locallizes it
    struct tm* a_given_time = std::localtime(&rendered);
    time_t made_time = std::mktime(a_given_time);

    DataManager::set_int_from_data(Beatup::last_login_key, (int)made_time);
}

std::chrono::duration<double, std::ratio<3600>> Beatup::hours_since_last_login()
{
    time_t from_file = (time_t)DataManager::get_int_from_data(Beatup::last_login_key);

    std::stringstream ss;
    ss << "The last login time was " << std::ctime(&from_file);
    std::chrono::system_clock::time_point now_time_point = std::chrono::system_clock::now();

    if (from_file == 0)
    {
        CCLOG("last login not found, defaulting to now");
        std::chrono::time_point<std::chrono::system_clock> time_is_now = std::chrono::system_clock::now();
        from_file = std::chrono::system_clock::to_time_t(time_is_now);
        auto local_tm = std::localtime(&from_file);
        from_file = std::mktime(local_tm);
    }

    std::chrono::system_clock::time_point last_login_time_point = std::chrono::system_clock::from_time_t(from_file);
    std::chrono::system_clock::duration diff = now_time_point - last_login_time_point;

    typedef std::chrono::duration<double, std::ratio<3600>> hours_fp;
    auto hours = std::chrono::duration_cast<hours_fp>(diff);
    //ss << "this is hours since last login " << hours.count() << std::endl;

    //auto minutes = std::chrono::duration_cast<std::chrono::minutes>(diff);
    //ss << "this is minutes since last login " << minutes.count() << std::endl;

    //auto seconds = std::chrono::duration_cast<std::chrono::seconds>(diff);
    //ss << "this is seconds since last login " << seconds.count();

    return hours;
};

void Beatup::add_total_coin(double x)
{
    DataManager::incr_key("total_coin_key", x);
};

void Beatup::back_to_menu()
{
    SoundEngine::stop_all_sound_and_music();
    SoundEngine::play_music("music\\menu.mp3");

    auto director = Director::getInstance();
    director->pushScene(Beatup::main_menu_scene);
}

void Beatup::onEnter()
{
    GameLayer::onEnter();
    //this is so it updates again because if you pop a scene or something, the
    //game stops the scheduler FIXME
    auto update_selector = schedule_selector(Beatup::update);
    if (!this->isScheduled(update_selector))
    {
        this->schedule(update_selector);
        //assumes Buildup stuff isnt ready either
        this->schedule(schedule_selector(Beatup::update_buildup));
    }
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
    this->buildup->set_target_building(target);
};

spBuilding Beatup::get_target_building()
{
    if (this->buildup->get_target_building() == NULL ) { return NULL; };
    return this->buildup->get_target_building();
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
        pEvent->stopPropagation();
    }
};

void Beatup::onKeyPressed(EventKeyboard::KeyCode keyCode, Event* evt)
{
    this->key_state[keyCode] = true;

    if(keyCode == EventKeyboard::KeyCode::KEY_BACK || keyCode == EventKeyboard::KeyCode::KEY_ESCAPE) 
    {
        //dont want this because it'll give a keyReleased event to whatever it would popScene to
        //Beatup::back_to_menu();
        //printj("esc/back key pressed");
        //evt->stopPropagation();
    }
    else if(keyCode == EventKeyboard::KeyCode::KEY_F) 
    {
    }
    else if(keyCode == EventKeyboard::KeyCode::KEY_W) 
    {
    }
    else if(keyCode == EventKeyboard::KeyCode::KEY_C) 
    {
        double new_val = this->get_total_coins()*10;
        DataManager::set_double_from_data(Beatup::total_coin_key, new_val);
    }
    else if(keyCode == EventKeyboard::KeyCode::KEY_TAB) 
    {
        // this->get_target_face()->recovering_sprite->getGLProgramState()->setUniformFloat("u_amount", 1.0f);
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
        this->_visible_inventory = !this->_visible_inventory;
    }
    else if(keyCode == EventKeyboard::KeyCode::KEY_F5) 
    {
        this->reload_resources();
    }
    else if(keyCode == EventKeyboard::KeyCode::KEY_SPACE) 
    {
    }
    else if (keyCode == EventKeyboard::KeyCode::KEY_H)
    {
        auto farm = this->buildup->city->building_by_name("The Farm");
        Recipe heal_recipe = Recipe("HealingGrain");
        heal_recipe.components[Ingredient::SubType::Grain] = 5;
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

    }
    else if(keyCode == EventKeyboard::KeyCode::KEY_X) 
    {
    }
    else if(keyCode == EventKeyboard::KeyCode::KEY_GRAVE) 
    {
        this->reload_resources();
    }
    else if(keyCode == EventKeyboard::KeyCode::KEY_V) 
    {
    }
    else if(keyCode == EventKeyboard::KeyCode::KEY_S) 
    {
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
    }

    else if(keyCode == EventKeyboard::KeyCode::KEY_2) 
    {
        GameDirector::switch_to_building_menu();
    }

    else if(keyCode == EventKeyboard::KeyCode::KEY_3) 
    {
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

    if (this->get_level_over())
    {
        this->setPosition(0, 0);
    };

    this->spawn_coin_clock->update(dt);

    this->stamina_clock->update(dt);
    if (this->stamina_clock->passed_threshold())
    {
        this->add_to_stamina(this->stamina_regen_rate);
        this->stamina_clock->reset();
    };
    float percentage = this->stamina_count/this->stamina_max;
    this->stamina_prog->scroll_to_percentage(percentage);

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


void Beatup::hide_ui()
{
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

    log("You've beaten the level!");

    this->hide_ui();

    // SoundEngine::play_sound("sounds\\old\\cheering.mp3");
    SoundEngine::play_sound("sounds\\new\\cheering\\C_cheering_13.mp3");
};

void Beatup::cycle_next_building(bool reverse)
{
    vsBuilding buildings = this->buildup->city->buildings;
    if (buildings.size() > 1)
    {
        if (reverse == false)
        {
            vsBuilding::iterator next_building_iter = std::find(buildings.begin(), buildings.end(), this->get_target_building());

            if (next_building_iter + 1 != buildings.end())
            {
                this->set_target_building(*(next_building_iter + 1));
            }
            else
            {
                this->set_target_building(*buildings.begin());
            }

        }
        else if (reverse == true)
        {
            vsBuilding::reverse_iterator next_building_riter = std::find(buildings.rbegin(), buildings.rend(), this->get_target_building());

            if (next_building_riter + 1 != buildings.rend())
            {
                this->set_target_building(*(next_building_riter + 1));
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
    this->setPosition(0, 0);
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

    prog->updateUniforms();
    CHECK_GL_ERROR_DEBUG();


    ShaderCache::getInstance()->addGLProgram(prog, "clouds");

    CHECK_GL_ERROR_DEBUG();

    GLProgramState* state = GLProgramState::getOrCreateWithGLProgram(prog);
    state->setUniformFloat("u_amount", 1.0f);
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
    system("xcopy \"C:\\Users\\Primary\\workspace\\basebuilder\\proj.win32\\..\\Resources\" \"C:\\Users\\Primary\\workspace\\basebuilder\\proj.win32\\Debug.win32\\\" /E /I /F /Y  /D /K ");
#endif
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