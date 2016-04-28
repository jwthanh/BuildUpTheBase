#pragma once
#ifndef UPDATEABLE_H
#define UPDATEABLE_H

class Clock;

class Updateable
{
    public:
        Clock* update_clock;

        Updateable();
        void update(float dt);
};

#endif
