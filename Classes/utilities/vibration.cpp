#include "vibration.h"

#include "GameLogic.h"

void do_vibrate(int milliseconds)
{
    if (GameLogic::getInstance()->get_can_vibrate() == false)
    {
        CCLOG("NOT vibrating");
        return;
    };

#ifdef __ANDROID__
        vibrate(milliseconds);
#else
        CCLOG("vibrating for %ims", milliseconds);
#endif

};
