#pragma once
#ifndef UTIL_H
#define UTIL_H

#include <vector>

#include "cocos2d.h"

float sx(float x, bool do_scale=false);
float sy(float y, bool do_scale=false);

template<typename T>
T pick_one(std::vector<T> vec)
{
    int size = vec.size();
    int index = rand() % size;
    return vec.at(index);
};

void log_vector(cocos2d::Vec2 vector, std::string message="");

#endif
