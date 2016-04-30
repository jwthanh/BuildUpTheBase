#pragma once
#ifndef BUYABLE_H
#define BUYABLE_H

#include <string>

class Beatup;

class Buyable 
{
    public:
    std::string id_key;
    unsigned long long int _shop_cost;

    Buyable(std::string id_key) : id_key(id_key) { _shop_cost=0; };

    unsigned long long int get_cost();
    void set_been_bought(bool been_bought);
    bool get_been_bought();
};
#endif
