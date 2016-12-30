#include "Tutorial.h"

Tutorial* Tutorial::_instance = nullptr;

Tutorial::Tutorial()
{
    //TODO serialize this
    this->_show_sidebar = true;
    this->_show_building_buttons = true;
    this->_show_player_info = true;
    this->_show_building_info = true;
};

Tutorial* Tutorial::getInstance()
{
    if (!Tutorial::_instance)
    {
        Tutorial::_instance = new Tutorial();
    };

    return Tutorial::_instance;
}

bool Tutorial::get_show_sidebar()
{
    return this->_show_sidebar;
};

bool Tutorial::get_show_building_buttons()
{
    return this->_show_building_buttons;
};

bool Tutorial::get_show_player_info()
{
    return this->_show_player_info;
};

bool Tutorial::get_show_building_info()
{
    return this->_show_building_info;
};
