#pragma once
#ifndef NAMEABLE_H
#define NAMEABLE_H

#include <string>

class Nameable
{
    public:
        std::string name;
        std::string short_name;

        Nameable(std::string name)
            : name(name), short_name("") {};

        Nameable(const Nameable& other)
        {
            this->name = other.name;
            this->short_name = other.short_name;
        }
};


#endif
