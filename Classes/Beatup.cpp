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
#include "Buildup.h"

//#pragma warning(default:4267)
//#pragma warning(default:4251)
//#pragma warning(default:4244)

#include "Util.h"
#include "Recipe.h"
#include "GameLogic.h"

#include "2d/CCScene.h"
#include "base/CCDirector.h"
#include "base/CCEventListenerTouch.h"
#include "base/CCEventListenerKeyboard.h"
#include "base/CCEventDispatcher.h"
#include "2d/CCMenu.h"
#include "cocostudio/ActionTimeline/CCActionTimelineCache.h"
#include "ui/UIText.h"
#include "ui/UIButton.h"
#include "goals/Achievement.h"
#include "Technology.h"

USING_NS_CC;


std::string Beatup::tutorial_id_key = "tutorials_enabled";

std::string Beatup::total_hit_key = "total_hit_key";
std::string Beatup::total_coin_key = "total_coin_key";
std::string Beatup::last_login_key = "last_login_key";

bool Beatup::init()
{

    this->_total_coins = DataManager::get_double_from_data(Beatup::total_coin_key);
    this->_last_total_coins = this->_total_coins;

    this->buildup = new Buildup();


    this->buildup->city = Buildup::init_city(this->buildup);
    this->buildup->city->update_buildings(0);

    this->setup_commands();

    this->has_been_initialized = true;

    return true;
}

void Beatup::update_buildup(float dt)
{
    this->buildup->update(dt);
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

void Beatup::add_total_coins(res_count_t x)
{
    if (x > 0.1) {
        auto achievement_manager = AchievementManager::getInstance();
        std::shared_ptr<BaseAchievement> achievement = achievement_manager->getAchievement(AchievementType::TotalCoins);
        achievement->increment_by_n(x);
    };


    this->_total_coins += x;

    auto max_storage = this->get_max_coin_storage();

    this->_total_coins = std::min(max_storage, this->_total_coins);
};


res_count_t Beatup::get_total_coins()
{
    return this->_total_coins;
}

res_count_t Beatup::get_max_coin_storage()
{
    Technology technology = Technology(TechSubType::RaiseWalletCap);
    res_count_t def = 0.0;
    auto marketplace = BUILDUP->city->building_by_name("The Marketplace");
    res_count_t num_researched = map_get(marketplace->techtree->tech_map, technology.sub_type, def);

    return scale_number_flat_pow(100000.0L, num_researched, 11.3L);

};

res_count_t Beatup::get_coin_storage_left()
{
    return this->get_max_coin_storage() - this->get_total_coins();
};

void Beatup::set_last_login()
{
    //get current time
    std::chrono::time_point<std::chrono::system_clock> time_is_now = std::chrono::system_clock::now();
    time_t rendered = std::chrono::system_clock::to_time_t(time_is_now);

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

    return hours;
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

void Beatup::reload_resources()
{
#ifdef __ANDROID__
#else
    system("xcopy \"C:\\Users\\Primary\\workspace\\basebuilder\\proj.win32\\..\\Resources\" \"C:\\Users\\Primary\\workspace\\basebuilder\\proj.win32\\Debug.win32\\\" /E /I /F /Y  /D /K ");
#endif
};