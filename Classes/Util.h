#pragma once
#ifndef UTIL_H
#define UTIL_H

#include <vector>
#include <random>
#include <functional>
#include <string>

//#include "math/vec2.h"

namespace cocos2d {
    class Size;
    class Vec2;
    class Node;

    namespace ui
    {
        class Button;
        class Widget;
    }
}

float sx(float x, bool do_scale=false);
float sy(float y, bool do_scale=false);
cocos2d::Vec2 sv(cocos2d::Vec2 input); //note do_scale is true

cocos2d::Vec2 get_relative(cocos2d::Size input, cocos2d::Vec2 relative_pos);
cocos2d::Vec2 get_relative_middle(cocos2d::Size input);
cocos2d::Vec2 get_center_pos(float offset_x = 0, float offset_y = 0);


template<typename T>
T pick_one(std::vector<T> vec, std::mt19937* generator=NULL)
{
    //create the generator if it doesnt exist
    if (!generator) {
        auto temp_gen = std::mt19937(std::random_device{}());
         generator = &temp_gen;
    };

    int size = vec.size();

    std::uniform_int_distribution<int> distribution(0, size-1);
    auto index = distribution(*generator);

    return vec.at(index);
};

void log_vector(cocos2d::Vec2 vector, std::string message="");

//assumes T is a map
template<typename T>
typename T::mapped_type map_get(const T& input_map, const typename T::key_type& key, const typename T::mapped_type& default_result)
{
    auto&& it = input_map.find(key);
    if (it != input_map.end())
    {
        return it->second;
    }
    else
    {
        return default_result;
    }
};

std::vector<std::string>& split(const std::string &s, char delim, std::vector<std::string> &elems);
std::vector<std::string> split(const std::string &s, char delim);
std::string beautify_double(const long double& value);
void test_beautify_double();

void try_set_enabled(cocos2d::ui::Widget* widget, bool new_value);
void try_set_visible(cocos2d::Node* node, bool new_value);

template<typename number_t>
number_t scale_number(number_t base_number, number_t current_number, number_t rate)
{
    //base * (rate**current)
    number_t result = base_number * std::pow(rate, std::max(static_cast<number_t>(0), current_number));
    if (std::isinf(result))
    {
        //CCLOG("oh no!, %f * (%f ** %f)", base_number, rate, current_number);
        return std::numeric_limits<number_t>::max();
    }
    return result;
};

template<typename number_t>
number_t scale_number_flat(number_t base_number, number_t current_number, number_t rate)
{
    //base * rate * current
    number_t result = base_number * rate * std::max(static_cast<number_t>(0), current_number);
    if (std::isinf(result))
    {
        //CCLOG("oh no!, %f * (%f * %f)", base_number, rate, current_number);
        return std::numeric_limits<number_t>::max();
    }
    return result;
};

template<typename number_t>
number_t scale_number_flat_pow(number_t base_number, number_t current_number, number_t rate)
{
    //base * (rate ** current)
    number_t result = base_number * std::pow(rate, std::max(static_cast<number_t>(0), current_number));
    if (std::isinf(result))
    {
        //CCLOG("oh no!, %f * (%f * %f)", base_number, rate, current_number);
        return std::numeric_limits<number_t>::max();
    }
    return result;
};

template<typename number_t>
number_t scale_number_slow(number_t base_number, number_t current_number, number_t rate, number_t slow_rate=2)
{
    //base * (rate**(current/slow_rate))
    number_t result = base_number * std::pow(rate, std::max(static_cast<number_t>(0), current_number)/slow_rate);
    if (std::isinf(result))
    {
        //CCLOG("oh no!, %f * (%f ** %f)", base_number, rate, current_number);
        return std::numeric_limits<number_t>::max();
    }
    return result;
};

void bind_touch_ended(cocos2d::ui::Widget* widget, std::function<void(void)> callback);

std::string clean_path(std::string input);

#endif
