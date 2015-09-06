#include "Clock.h"

Clock::Clock(float threshold)
{
    this->threshold = threshold;
    this->start_time = 0;
};

void Clock::reset()
{
    this->start_time = 0.0f;
};

void Clock::set_threshold(float val)
{
    this->threshold = val;
};

bool Clock::passed_threshold()
{
    return this->start_time >= this->threshold;
};

void Clock::update(float dt)
{
    this->start_time += dt;
};

float Clock::get_percentage()
{
    if (this->start_time <= 0.0f)
    {
        return 0.0f;
    };

    return this->start_time / this->threshold;
};

bool Clock::is_started()
{
    return this->start_time > 0;
};

bool Clock::is_out()
{
    return this->start_time <= 0;
};

bool Clock::is_active()
{
    return this->is_started() && !this->passed_threshold();
};


void Clock::start()
{
    this->start_time = 0.001f; //ideally this'd be like a bool or something 
};

void Clock::start_for_thres(float val)
{
    this->set_threshold(val);
    this->start();
};
