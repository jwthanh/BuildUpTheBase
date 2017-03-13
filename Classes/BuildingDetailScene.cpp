#include "BuildingDetailScene.h"

#include "constants.h"

#include "HouseBuilding.h"
#include "NodeBuilder.h"
#include "MiscUI.h"
#include "Util.h"
#include "GameLogic.h"

#include "2d/CCScene.h"
#include "base/CCEventListenerKeyboard.h"
#include "base/CCEventDispatcher.h"
#include "base/CCEventListenerTouch.h"
#include "base/CCDirector.h"
#include "ui/UIButton.h"
#include "ui/UIText.h"
#include "ui/UILayout.h"

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
    this->panel = editor_scene->getChildByName("panel");
    this->panel->removeFromParent();
    this->addChild(panel);

    auto back_btn = dynamic_cast<cocos2d::ui::Button*>(this->panel->getChildByName("back_btn"));
    prep_back_button(back_btn);

#ifdef _WIN32
    FUNC_INIT_WIN32(BuildingDetailScene);
#else
    FUNC_INIT(BuildingDetailScene);
#endif

    this->init_children();
    this->init_callbacks();

    return true;
};

void BuildingDetailScene::init_children()
{
    this->level_panel = dynamic_cast<cocos2d::ui::Layout*>(this->panel->getChildByName("level_panel"));
    this->level_button = dynamic_cast<cocos2d::ui::Button*>(level_panel->getChildByName("level_button"));
    prep_button(level_button);
};

void BuildingDetailScene::init_callbacks()
{
    bind_touch_ended(this->level_button, [](){ GameDirector::switch_to_scrap_item_menu(); });

    auto leveled_lbl = dynamic_cast<cocos2d::ui::Text*>(level_panel->getChildByName("current_level_lbl"));
    auto update_leveled = [this, leveled_lbl](float dt) {
        res_count_t leveled = this->building->building_level;
        leveled_lbl->setString(beautify_double(leveled));
    };
    leveled_lbl->schedule(update_leveled, AVERAGE_DELAY, "update_leveled");
    update_leveled(0);
};
