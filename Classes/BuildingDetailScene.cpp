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
#include "2d/CCParticleSystemQuad.h"
#include "Beatup.h"
#include "utilities/vibration.h"
#include "FShake.h"

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
    auto touch_level_button = [this](){
        res_count_t cost = scale_number(10.0L, ((res_count_t)this->building->get_building_level())-1.0L, 10.5L);
        res_count_t total_coins = BEATUP->get_total_coins();

        if (cost <= total_coins)
        {
            BEATUP->add_total_coins(-cost);

            this->building->set_building_level(this->building->get_building_level() + 1);

            auto explosion_parts = cocos2d::ParticleSystemQuad::create("particles/upgrade.plist");
            explosion_parts->setPosition({570, 300});
            explosion_parts->setAutoRemoveOnFinish(true);

            this->addChild(explosion_parts);

            this->runAction(FShake::actionWithDuration(0.25f, 2.5f));

            do_vibrate(175);
        }
    };
    bind_touch_ended(this->level_button, touch_level_button);

    auto leveled_lbl = dynamic_cast<cocos2d::ui::Text*>(level_panel->getChildByName("current_level_lbl"));
    auto update_leveled = [this, leveled_lbl](float dt) {
        const int& leveled = this->building->get_building_level();
        leveled_lbl->setString(beautify_double(leveled));
    };
    leveled_lbl->schedule(update_leveled, FPS_10, "update_leveled");
    update_leveled(0);
};
