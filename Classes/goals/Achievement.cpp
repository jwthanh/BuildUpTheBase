#include "Achievement.h"

Achievement::Achievement()
    : _completed(false), _icon_path("")
{

};

bool Achievement::get_completed()
{
    return this->_completed;
};

void Achievement::set_completed(bool completed)
{
    this->_completed = completed;
};

std::string Achievement::get_icon_path()
{
    return this->_icon_path;
};
