#ifndef CONSTRUCTABLE_H
#define CONSTRUCTABLE_H

#include <chrono>

#include "constants.h"
#include <unordered_map>

///represents a progress towards building an unlock, ie a new harvestable level 1
class Constructable
{
    private:
        bool _has_celebrated;

    public:
        //when constructable is completed
        TimePoint base_end_time;
        //base_end_time after being modified by abilities or manual
        TimePoint current_end_time;

        Constructable(
                TimePoint base_end_time,
                TimePoint current_end_time)
            : _has_celebrated(false),
            base_end_time(base_end_time),
            current_end_time(current_end_time) {}


        //whether now is currently passed this->current_end_time
        bool passed_threshold() const;

        //whether the constructable has been announced completed
        bool get_has_celebrated() const { return this->_has_celebrated; };
};


///oversees the constructables, responsible for checking them all each frame
class ConstructableManager
{
    public:
        std::unordered_map<std::string, spConstructable> constructables;
};
#endif
