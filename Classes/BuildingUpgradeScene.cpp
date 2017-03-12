#include "BuildingUpgradeScene.h"

#include "constants.h"

#include "HouseBuilding.h"
#include "NodeBuilder.h"
#include "MiscUI.h"

#include "2d/CCScene.h"
#include "base/CCEventListenerKeyboard.h"
#include "base/CCEventDispatcher.h"
#include "base/CCEventListenerTouch.h"
#include "base/CCDirector.h"
#include "ui/UIButton.h"

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

    auto editor_scene = get_prebuilt_node_from_csb("editor/scenes/building_detail_scene.csb");
    editor_scene->removeFromParent();
    auto panel = editor_scene->getChildByName("panel");
    panel->removeFromParent();
    this->addChild(panel);

    auto back_btn = dynamic_cast<cocos2d::ui::Button*>(panel->getChildByName("back_btn"));
    prep_back_button(back_btn);

#ifdef _WIN32
    FUNC_INIT_WIN32(BuildingUpgradeScene);
#else
    FUNC_INIT(BuildingUpgradeScene);
#endif

    // this->init_children();
    // this->init_callbacks();

    return true;
};
