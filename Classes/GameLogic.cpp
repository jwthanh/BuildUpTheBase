#include "GameLogic.h"

#include "Beatup.h"
#include "HouseBuilding.h"
#include "DataManager.h"
#include "NuMenu.h"
#include "Serializer.h"

GameLogic* GameLogic::_instance = NULL;

GameLogic::GameLogic()
{

};

bool GameLogic::init()
{
    GameLogic* instance = GameLogic::getInstance();
    instance->beatup = Beatup::create();
    instance->beatup->setName("beatup");
    instance->beatup->retain();
    instance->buildup = instance->beatup->buildup;

    CCLOG("loading game on startup, this should only happen once");
    for (spBuilding building : BUILDUP->city->buildings)
    {
        auto bldg_serializer = BuildingSerializer("test_building.json", building);
        bldg_serializer.load();
    };

    std::stringstream ss;
    auto hours_since_last_login = BEATUP->hours_since_last_login();

    ss << "It's been " << hours_since_last_login.count() << " hours since last login";
    CCLOG(ss.str().c_str());

    //set the last login time
    BEATUP->set_last_login();

    return true;
};

void GameLogic::update(float dt)
{
    this->beatup->update_buildup(dt);
};

GameLogic* GameLogic::getInstance()
{
    if (!GameLogic::_instance)
    {
        GameLogic::_instance = new GameLogic();
        GameLogic::_instance->init();
    };

    return GameLogic::_instance;
};

void GameLogic::add_total_harvests(int value)
{
    DataManager::incr_key("total_harvests", value);
};

int GameLogic::get_total_harvests()
{
    return DataManager::get_int_from_data("total_harvests");
};


void GameDirector::switch_to_building_menu()
{
    auto scene = cocos2d::Scene::create();
    BuildingNuMenu* building_menu = BuildingNuMenu::create(
        BUILDUP->get_target_building()
        );
    scene->addChild(building_menu);

    auto director = cocos2d::Director::getInstance();
    director->pushScene(scene);
};
