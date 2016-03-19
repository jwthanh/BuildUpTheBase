#include "HarvestScene.h"
#include "Harvestable.h"
#include "Beatup.h"

USING_NS_CC;

bool HarvestScene::init()
{
    FUNC_INIT(HarvestScene);

    this->beatup = Beatup::create();
    beatup->setName("beatup");
    beatup->retain(); //TODO figure out when beatup gets cleaned up aka why this is needed

    auto harvestable = Harvestable::create(this->beatup);
    harvestable->setPosition(this->get_center_pos());
    this->addChild(harvestable);

    this->scheduleUpdate();

    return true;
};

void HarvestScene::update(float dt)
{
};

void HarvestScene::onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event)
{
    if(keyCode == EventKeyboard::KeyCode::KEY_BACK || keyCode == EventKeyboard::KeyCode::KEY_ESCAPE) 
    {
        this->quit(NULL);
        event->stopPropagation();
    }
};
