#pragma once
#ifndef HARVESTSCENE_H
#define HARVESTSCENE_H

#include "GameLayer.h"

class HarvestScene : public GameLayer
{
    public:
        CREATE_FUNC(HarvestScene);
        bool init();

        Beatup* beatup;

};

#endif
