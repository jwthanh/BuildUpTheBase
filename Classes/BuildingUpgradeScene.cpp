#include "BuildingUpgradeScene.h"

#include "constants.h"

#include "HouseBuilding.h"

#include "2d/CCScene.h"
#include "base/CCEventListenerKeyboard.h"
#include "base/CCEventDispatcher.h"
#include "base/CCEventListenerTouch.h"
#include "base/CCDirector.h"

BuildingUpgradeScene* BuildingUpgradeScene::create(spBuilding building)
{
    BuildingUpgradeScene* pRet = new(std::nothrow) BuildingUpgradeScene();
    if (pRet && pRet->init(building))
    {
        pRet->autorelease();
        return pRet;
    }
    else
    {
        delete pRet;
        pRet = nullptr;
        return nullptr;
    }
};

cocos2d::Scene* BuildingUpgradeScene::createScene(spBuilding building)
{
    auto scene = cocos2d::Scene::create();
    auto* layer = BuildingUpgradeScene::create(building);
    scene->addChild(layer);
    return scene;
};

bool BuildingUpgradeScene::init(spBuilding building)
{
    this->building = building;

#ifdef _WIN32
    FUNC_INIT_WIN32(BuildingUpgradeScene);
#else
    FUNC_INIT(BuildingUpgradeScene);
#endif

    // this->init_children();
    // this->init_callbacks();

    return true;
};
