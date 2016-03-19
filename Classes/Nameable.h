#pragma once
#ifndef NAMEABLE_H
#define NAMEABLE_H

#include <string>

class Nameable
{
    public:
        std::string name;

        Nameable(std::string name) :name(name) {};
        Nameable(const Nameable& other)
        {
            this->name = other.name;
        }
};


#endif
