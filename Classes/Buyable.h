#pragma once
#ifndef BUYABLE_H
#define BUYABLE_H

#include <string>

class Beatup;

class Buyable 
{
    public:
    std::string id_key;
    long double _shop_cost;

    Buyable(std::string id_key) : id_key(id_key) { _shop_cost=0; };

    long double get_cost();
    virtual void set_been_bought(bool been_bought);
    virtual bool get_been_bought();
};
#endif
