#include "HarvestScene.h"
#include "Harvestable.h"

#include "GameLogic.h"

USING_NS_CC;

bool HarvestScene::init()
{
    FUNC_INIT(HarvestScene);

    this->add_harvestable();

    this->scheduleUpdate();

    return true;
};

void HarvestScene::add_harvestable()
{
    auto harvestable = Harvestable::create(GameLogic::getInstance()->beatup);
    harvestable->setPosition(this->get_center_pos());
    harvestable->setName("harvestable");
    this->addChild(harvestable);
};

void HarvestScene::update(float dt)
{
    GameLogic::getInstance()->update(dt);

    if (!this->getChildByName("harvestable")) {
        this->add_harvestable();
    };
};

void HarvestScene::onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event)
{
    if(keyCode == EventKeyboard::KeyCode::KEY_BACK || keyCode == EventKeyboard::KeyCode::KEY_ESCAPE) 
    {
        this->quit(NULL);
        event->stopPropagation();
    }
};
