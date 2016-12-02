#include "Updateable.h"


// #include <vld.h>

Updateable::Updateable()
{
    this->update_clock = Clock(15.0f);
};

void Updateable::update(float dt)
{
    this->update_clock.update(dt);
};

