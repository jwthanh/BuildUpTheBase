#pragma once
#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <string>
#include <algorithm>

#define TITLE_FONT  "fonts/airstrike.ttf"
#define DEFAULT_FONT  "fonts/pixelmix.ttf"

enum FistHands 
{
    None = -1,
    Left = 0,
    Right = 1,
    LCharge = 2,
    RCharge = 3
};

enum FistWeaponTypes
{
    Flames,
    Psionic,
    Frost,
};

enum ShieldTypes
{
    Human,
    Brick
};

enum BackgroundTypes
{
    Forest,
    Volcano,
    Western
};

class Beatup;

typedef std::function<bool()> BoolFuncNoArgs;
typedef std::function<void()> VoidFuncNoArgs;
typedef std::function<void(Beatup*)> VoidFuncBeatupNoArgs;

const char PATH_SEPARATOR =
#ifdef _WIN32
                            '\\';
#else
                            '/';
#endif

inline std::string clean_path(std::string input) {
    std::replace( input.begin(), input.end(), '\\', PATH_SEPARATOR); // replace all 'x' to 'y'
    return input;
}
#endif
