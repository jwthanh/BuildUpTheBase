#include "Tutorial.h"

Tutorial* Tutorial::_instance = nullptr;

Tutorial::Tutorial()
{

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
    return true;
};

bool Tutorial::get_show_building_buttons()
{
    return true;
};

bool Tutorial::get_show_player_info()
{
    return true;
};

bool Tutorial::get_show_building_info()
{
    return true;
};
