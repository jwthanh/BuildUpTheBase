#include "Buyable.h"

#include "DataManager.h"

using res_count_t = long double;

void Buyable::set_been_bought(bool been_bought)
{
    DataManager::set_bool_from_data(this->id_key, been_bought);
};

bool Buyable::get_been_bought()
{
    return DataManager::get_bool_from_data(this->id_key, false);
};

res_count_t Buyable::get_cost()
{
    return this->_shop_cost;
};
