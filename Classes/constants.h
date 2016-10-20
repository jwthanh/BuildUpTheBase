#pragma once
#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <algorithm>
#include <functional>
#include <vector>
#include <memory>
#include <string>
#include <map>

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


class Clock;
class Recipe;

class Beatup;
class Building;
class Visitors;

class Buildup;
class Village;
class BuildingData;

class Fighter;
class Battle;

class Worker;
class Harvester;
class Salesman;
class ConsumerHarvester;
class ResourceCondition;
class Harvestable;

class Resource;
class Ingredient;
class Product;
class Waste;

class Attribute;
class AttributeContainer;
class FighterNode;
class Fighter;
class Combat;
class ProgressBar;
class Buildup;
class Experience;

class TechTree;
class Technology;

class Item;

typedef std::function<void()> VoidFunc;

#define _MAKE_SP(Cls)typedef std::shared_ptr<Cls> sp##Cls
#define _MAKE_VS(Cls)typedef std::vector<sp##Cls> vs##Cls
#define _MAKE_MI(Cls)typedef std::map<sp##Cls, unsigned int> mi##Cls

#define MAKE_SP_VS_MI(Cls)_MAKE_SP(Cls);_MAKE_VS(Cls);_MAKE_MI(Cls)

MAKE_SP_VS_MI(Clock);

MAKE_SP_VS_MI(Building);
MAKE_SP_VS_MI(BuildingData);
MAKE_SP_VS_MI(Visitors);

MAKE_SP_VS_MI(Battle);
MAKE_SP_VS_MI(Fighter);
MAKE_SP_VS_MI(Worker);


MAKE_SP_VS_MI(Harvester);
MAKE_SP_VS_MI(Salesman);
MAKE_SP_VS_MI(ConsumerHarvester);
MAKE_SP_VS_MI(ResourceCondition);
MAKE_SP_VS_MI(Recipe);
MAKE_SP_VS_MI(Harvestable);

MAKE_SP_VS_MI(Resource);
MAKE_SP_VS_MI(Product);
MAKE_SP_VS_MI(Ingredient);
MAKE_SP_VS_MI(Waste);

MAKE_SP_VS_MI(Attribute);
MAKE_SP_VS_MI(AttributeContainer);
MAKE_SP_VS_MI(FighterNode);
MAKE_SP_VS_MI(Fighter);
MAKE_SP_VS_MI(Combat);
MAKE_SP_VS_MI(ProgressBar);
MAKE_SP_VS_MI(Buildup);
MAKE_SP_VS_MI(Experience);
MAKE_SP_VS_MI(Village);

MAKE_SP_VS_MI(TechTree);
MAKE_SP_VS_MI(Technology);

MAKE_SP_VS_MI(Item);


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

#define BEATUP GameLogic::getInstance()->beatup
#define BUILDUP GameLogic::getInstance()->buildup

typedef long double res_count_t;

#define rjDocument rapidjson::GenericDocument<rapidjson::UTF8<>,rapidjson::CrtAllocator>
#define rjValue rapidjson::GenericValue<rapidjson::UTF8<>, rapidjson::CrtAllocator>

const float SECOND_DELAY = 1.0f;
const float LONG_DELAY = 0.5f;
const float AVERAGE_DELAY = 0.1f;
const float HALF_DELAY = 0.05f;
const float QUARTER_DELAY = 0.025f;
const float SHORT_DELAY = 0.01f;
const float REALTIME_DELAY = 0.001f;

#endif
