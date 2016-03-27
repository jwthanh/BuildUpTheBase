#pragma once
#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <string>
#include <algorithm>
#include <functional>
#include <vector>
#include <memory>
#include <sstream>

#include "CCPlatformMacros.h"

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


class Recipe;
class Beatup;
class Building;
class Buildup;
class Village;
class Product;
class Ingredient;
class Waste;
class Resource;
class Fighter;
class Battle;
class Worker;
class BuildingData;
class Harvester;
class ResourceCondition;


typedef std::function<void()> VoidFunc;

#define _MAKE_SP(Cls)typedef std::shared_ptr<Cls> sp##Cls
#define _MAKE_VS(Cls)typedef std::vector<sp##Cls> vs##Cls

#define MAKE_SP_VS(Cls)_MAKE_SP(Cls);_MAKE_VS(Cls)

MAKE_SP_VS(Building);
MAKE_SP_VS(Resource);
MAKE_SP_VS(Product);
MAKE_SP_VS(Ingredient);
MAKE_SP_VS(Waste);
MAKE_SP_VS(Battle);
MAKE_SP_VS(Fighter);
MAKE_SP_VS(Worker);
MAKE_SP_VS(BuildingData);
MAKE_SP_VS(Harvester);
MAKE_SP_VS(ResourceCondition);

#undef MAKE_SP_VS
#undef _MAKE_VS
#undef _MAKE_SP

#define printj(msg){std::stringstream ss123; ss123<< msg; CCLOG(ss123.str().c_str(), "");}
#define printj1(msg){std::stringstream ss123; ss123<< "\t" << msg; CCLOG(ss123.str().c_str(), "");}
#define printj2(msg){std::stringstream ss123; ss123<< "\t\t" << msg; CCLOG(ss123.str().c_str(), "");}
#define NO_CB [](){}


//typedef void(*VoidFuncBuilding)(spBuilding);
typedef void(*TaskFunc)(spBuilding, float);
typedef bool(*BoolFuncBuilding)(spBuilding);

#endif
