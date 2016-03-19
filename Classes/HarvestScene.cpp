#include "HarvestScene.h"
#include "Harvestable.h"
#include "Beatup.h"

bool HarvestScene::init()
{
    this->beatup = Beatup::create();
    beatup->setName("beatup");
    beatup->retain(); //TODO figure out when beatup gets cleaned up aka why this is needed

    auto harvestable = Harvestable::create(this->beatup);
    harvestable->setPosition(this->get_center_pos());
    this->addChild(harvestable);

    return true;
};
