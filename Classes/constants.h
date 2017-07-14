#pragma once
#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <algorithm>
#include <chrono>
#include <functional>
#include <vector>
#include <memory>
#include <map>

#define TITLE_FONT  "fonts/airstrike.ttf"
#define DEFAULT_FONT  "fonts/pixelmix.ttf"

const char PATH_SEPARATOR =
#ifdef _WIN32
                            '\\';
#else
                            '/';
#endif


class Clock;
class Recipe;

class Beatup;
class Building;
class Visitors;

class Buildup;
class City;
class BuildingData;

class Fighter;
class Battle;

class Worker;
class Harvester;
class Salesman;
class ConsumerHarvester;
class ScavengerHarvester;
class ResourceCondition;
class Harvestable;

struct Ingredient;

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

class Tutorial;
class TutorialStep;

class GameProgress;
class Constructable;
class ConstructableManager;
class Blueprint;

enum class IngredientSubType;
enum class WorkerSubType;
typedef long double res_count_t;



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
MAKE_SP_VS_MI(ScavengerHarvester);
MAKE_SP_VS_MI(ResourceCondition);
MAKE_SP_VS_MI(Recipe);
MAKE_SP_VS_MI(Harvestable);

MAKE_SP_VS_MI(Ingredient);

MAKE_SP_VS_MI(Attribute);
MAKE_SP_VS_MI(AttributeContainer);
MAKE_SP_VS_MI(FighterNode);
MAKE_SP_VS_MI(Fighter);
MAKE_SP_VS_MI(Combat);
MAKE_SP_VS_MI(ProgressBar);
MAKE_SP_VS_MI(Buildup);
MAKE_SP_VS_MI(Experience);
MAKE_SP_VS_MI(City);

MAKE_SP_VS_MI(TechTree);
MAKE_SP_VS_MI(Technology);

MAKE_SP_VS_MI(Item);

MAKE_SP_VS_MI(Tutorial);
MAKE_SP_VS_MI(TutorialStep);

MAKE_SP_VS_MI(Constructable);
MAKE_SP_VS_MI(ConstructableManager);
MAKE_SP_VS_MI(Blueprint);

using mistIngredient = std::map<IngredientSubType, res_count_t>;

using work_ing_t = std::pair<WorkerSubType, IngredientSubType>;
using mistHarvester = std::map<work_ing_t, res_count_t>;
using mistWorkerSubType = std::map<WorkerSubType, res_count_t>;

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

#define GAME_LOGIC GameLogic::getInstance()
#define BEATUP GAME_LOGIC->beatup
#define BUILDUP GAME_LOGIC->buildup
#define CITY BUILDUP->city
#define EQUIPMENT GAME_LOGIC->equipment
#define INVENTORY GAME_LOGIC->equipment->inventory

#define BANK Bank::getInstance()
#define GAME_PROGRESS GameProgress::getInstance()
#define CON_MAN ConstructableManager::getInstance()

//macros because we don't want to include rapidjson everywhere and I couldn't figure out
// how to forward declare templates
#define rjDocument rapidjson::GenericDocument<rapidjson::UTF8<>,rapidjson::CrtAllocator>
#define rjAllocator rjDocument::AllocatorType
#define rjValue rapidjson::GenericValue<rapidjson::UTF8<>, rapidjson::CrtAllocator>
//helper to make rjValues from std::strings
#define RJ_STRING(string) rjValue(string.c_str(), string.length(), allocator)


#define TOUCH_CALLBACK_PARAMS cocos2d::Ref* target, TouchEventType evt

using BoolFuncNoArgs = std::function<bool()>;
using VoidFuncNoArgs = std::function<void()>;
using VoidFuncBeatupNoArgs = std::function<void(Beatup*)>;

using TimePoint = std::chrono::system_clock::time_point;
using Duration = std::chrono::system_clock::duration;
using SysClock = std::chrono::system_clock;


const float FPS_1   = 1.0f /   1.0f;
const float FPS_2   = 1.0f /   2.0f;
const float FPS_4   = 1.0f /   4.0f;
const float FPS_10  = 1.0f /  10.0f;
const float FPS_15  = 1.0f /  15.0f;
const float FPS_30  = 1.0f /  30.0f;
const float FPS_60  = 1.0f /  60.0f;
const float FPS_120 = 1.0f / 120.0f;

#endif
