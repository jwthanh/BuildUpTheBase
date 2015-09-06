#include "Util.h"

float sx (float x)
{
    auto frs = cocos2d::Director::getInstance()->getOpenGLView()->getFrameSize();
    return x * (frs.width / 960.0);
};

float sy (float y)
{
    auto frs = cocos2d::Director::getInstance()->getOpenGLView()->getFrameSize();
    return y * (frs.height / 640.0);
};
