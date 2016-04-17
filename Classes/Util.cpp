#include "Util.h"

float sx (float x, bool do_scale)
{
    if (!do_scale)
    {
        return x;
    }
    auto frs = cocos2d::Director::getInstance()->getOpenGLView()->getFrameSize();
    return (float)(x * (frs.width / 960.0));
};

float sy (float y, bool do_scale)
{
    if (!do_scale)
    {
        return y;
    }
    auto frs = cocos2d::Director::getInstance()->getOpenGLView()->getFrameSize();
    return (float)(y * (float)(frs.height / 640.0));
};

cocos2d::Vec2 sv(cocos2d::Vec2 input)
{
    return cocos2d::Vec2(sx(input.x, true), sy(input.y, true));
}

cocos2d::Vec2 get_relative(cocos2d::Size input, cocos2d::Vec2 relative_pos)
{
    return cocos2d::Vec2(
        input.width * relative_pos.x,
        input.height * relative_pos.y
    );
};

void log_vector(cocos2d::Vec2 vector, std::string message)
{
    std::stringstream ss;
    ss << message << ": " << vector.x << " " << vector.y;
    CCLOG("%s", ss.str().c_str());
};

