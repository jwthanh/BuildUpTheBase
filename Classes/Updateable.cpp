#include "Updateable.h"

#include "Clock.h"


// #include <vld.h>

Updateable::Updateable()
{
    this->update_clock = std::make_shared<Clock>(15.0f);
};

void Updateable::update(float dt)
{
    this->update_clock->update(dt);
};

