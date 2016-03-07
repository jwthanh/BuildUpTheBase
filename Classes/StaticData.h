#pragma once
#ifndef STATICDATA_H
#define STATICDATA_H

#include <string>

class BaseStaticData
{
    static std::string _filename;

    public:

    static std::string filename()
    {
        return _filename;
    }

    static void set_filename(const std::string& cs)
    {
        _filename = cs;
    }

    static std::string get_data(std::string key_top, std::string key_child, std::string key_grandchild);
};

#endif
