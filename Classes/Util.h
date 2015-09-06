#pragma once
#ifndef UTIL_H
#define UTIL_H

#include <vector>

#include "cocos2d.h"

float sx(float x);
float sy(float y);

template<typename T>
T pick_one(std::vector<T> vec)
{
    int size = vec.size();
    int index = rand() % size;
    return vec.at(index);
};

#endif
