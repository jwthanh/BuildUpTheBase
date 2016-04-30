#include "Buyable.h"

#include "DataManager.h"

void Buyable::set_been_bought(bool been_bought)
{
    DataManager::set_bool_from_data(this->id_key, been_bought);
};

bool Buyable::get_been_bought()
{
    return DataManager::get_bool_from_data(this->id_key, false);
};

unsigned long long int Buyable::get_cost()
{
    return this->_shop_cost;
};
