#include "bank.h"

Bank* Bank::_instance = NULL;

Bank::Bank()
{
    this->_total_coins_banked = 0.0; //TODO load in from serializer
}

Bank* Bank::getInstance()
{
    if (!Bank::_instance)
    {
        Bank::_instance = new Bank();
    };

    return Bank::_instance;
};

res_count_t Bank::get_total_coins_banked() const
{
    return this->_total_coins_banked;
};

void Bank::add_total_coins_banked(res_count_t added_value)
{
    this->_total_coins_banked += added_value;
};

void Bank::set_total_coins_banked(res_count_t new_total)
{
    this->_total_coins_banked = new_total;
};
