#pragma once
#ifndef BUYABLE_H
#define BUYABLE_H

#include <string>

class Beatup;

class Buyable 
{
    public:
    std::string id_key;
    int shop_cost;

    Buyable(std::string id_key) : id_key(id_key) { shop_cost=0; };

    void set_been_bought(bool been_bought);
    bool get_been_bought();
};
#endif
