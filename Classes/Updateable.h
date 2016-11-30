#pragma once
#ifndef UPDATEABLE_H
#define UPDATEABLE_H
#include <memory>


class Clock;

class Updateable
{
    public:
        std::shared_ptr<Clock> update_clock;

        Updateable();
        void update(float dt);
};

#endif
