#pragma once
#ifndef CLOCK_H
#define CLOCK_H

class Clock
{
    public:
        Clock(float threshold);

        float start_time;

        float time_left;
        float threshold;

        void start();
        void start_for_thres(float);
        void reset();
        void set_threshold(float val);

        void update(float dt);
        float get_percentage();

        bool passed_threshold();
        bool is_started();
        bool is_out();
        bool is_active();
};

#endif
