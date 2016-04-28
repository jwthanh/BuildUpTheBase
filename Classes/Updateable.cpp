#include "Updateable.h"

#include "Clock.h"

Updateable::Updateable()
{
    this->update_clock = new Clock(15.0f);
};

void Updateable::update(float dt)
{
    this->update_clock->update(dt);
};

