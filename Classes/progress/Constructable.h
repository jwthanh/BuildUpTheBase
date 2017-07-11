#ifndef CONSTRUCTABLE_H
#define CONSTRUCTABLE_H

#include <chrono>

using TimePoint = std::chrono::system_clock::time_point;
using Duration = std::chrono::system_clock::duration;
using SysClock = std::chrono::system_clock;

///represents a progress towards building an unlock, ie a new harvestable level 1
class Constructable
{
    public:
        //when constructable is completed
        TimePoint base_end_time;
        //base_end_time after being modified by abilities or manual
        TimePoint current_end_time;

        Constructable(
                TimePoint base_end_time,
                TimePoint current_end_time)
            : base_end_time(base_end_time), current_end_time(current_end_time) {}


        //whether now is currently passed this->current_end_time
        bool passed_threshold() const;
};
#endif
