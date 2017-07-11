#include "Constructable.h"

bool Constructable::passed_threshold() const
{
    TimePoint now_time_point = SysClock::now();
    Duration diff = now_time_point - this->current_end_time;

    //if its 0 or lower, the threshold has passed
    //if its above 0, its still in the future
    return diff <= Duration(0);
}
