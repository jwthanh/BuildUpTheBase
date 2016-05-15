#pragma once
#ifndef VISITORS_H
#define VISITORS_H

#include "constants.h"

#include "Updateable.h"

class Visitors : public Updateable
{
    public:
        spBuilding building;

        Visitors(spBuilding building);

        void update(float dt);
};

#endif
