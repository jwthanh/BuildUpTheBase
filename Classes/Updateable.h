#pragma once
#ifndef UPDATEABLE_H
#define UPDATEABLE_H
#include "Clock.h"


class Updateable
{
    public:
        Clock update_clock;

        Updateable();
        void update(float dt);
};

#endif
