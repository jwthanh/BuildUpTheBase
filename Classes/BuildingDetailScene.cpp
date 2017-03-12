#include "BuildingDetailScene.h"

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

BuildingDetailScene* BuildingDetailScene::create(spBuilding building)
{
    BuildingDetailScene* pRet = new(std::nothrow) BuildingDetailScene();
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

cocos2d::Scene* BuildingDetailScene::createScene(spBuilding building)
{
    auto scene = cocos2d::Scene::create();
    auto* layer = BuildingDetailScene::create(building);
    scene->addChild(layer);
    return scene;
};

bool BuildingDetailScene::init(spBuilding building)
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
    FUNC_INIT_WIN32(BuildingDetailScene);
#else
    FUNC_INIT(BuildingDetailScene);
#endif

    // this->init_children();
    // this->init_callbacks();

    return true;
};
