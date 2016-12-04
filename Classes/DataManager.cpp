#include "DataManager.h"
#include <cassert>

#include "base/CCConsole.h"
#include "base/CCUserDefault.h"
#include "external/easylogging.h"


void DataManager::validate_key(std::string key)
{
    if (key == "")
    {
        CCLOG("found empty string: the key \"%s\"", key.c_str());
        assert(false && "Cant have empty keys in userdefault keys");
    };

    auto pos = key.find(" ");
    bool valid_string = pos == std::string::npos;
    if (!valid_string)
    {
        CCLOG("found a space in the key \"%s\"", key.c_str());
        assert(valid_string && "Cant have spaces in userdefault keys");
    }
}


bool DataManager::get_bool_from_data(std::string key, bool _default)
{
    validate_key(key);
    cocos2d::UserDefault* ud = cocos2d::UserDefault::getInstance();
    CCLOG("getting bool %s", key.c_str());
    return ud->getBoolForKey(key.c_str(), _default);
};

void DataManager::set_bool_from_data(std::string key, bool val)
{
    validate_key(key);
    cocos2d::UserDefault* ud = cocos2d::UserDefault::getInstance();
    ud->setBoolForKey(key.c_str(), val);
    CCLOG("setting bool %s for %i", key.c_str(), val);
};

int DataManager::get_int_from_data(std::string key, int _default)
{
    validate_key(key);
    cocos2d::UserDefault* ud = cocos2d::UserDefault::getInstance();
    CCLOG("getting int %s", key.c_str());
    return ud->getIntegerForKey(key.c_str(), _default);
};

void DataManager::set_int_from_data(std::string key, int val)
{
    validate_key(key);
    cocos2d::UserDefault* ud = cocos2d::UserDefault::getInstance();
    CCLOG("setting int %s for %i", key.c_str(), val);
    ud->setIntegerForKey(key.c_str(), val);
};

double DataManager::get_double_from_data(std::string key, double _default)
{
    validate_key(key);
    CCLOG("getting double %s", key.c_str());

    cocos2d::UserDefault* ud = cocos2d::UserDefault::getInstance();

	std::string potential_double = ud->getStringForKey(key.c_str(), "empty");
	if (potential_double == "Infinity")
	{
		LOG(WARNING) << "attempted double from data: " << potential_double;
		LOG(WARNING) << "used 123456789.0 instead";
		return 123456789.0;
	}
	else
	{
		return ud->getDoubleForKey(key.c_str(), _default);
	}
}

void DataManager::set_double_from_data(std::string key, double val)
{
    validate_key(key);
    cocos2d::UserDefault* ud = cocos2d::UserDefault::getInstance();
    CCLOG("setting double %s for %f", key.c_str(), val);
    ud->setDoubleForKey(key.c_str(), val);
};


std::string DataManager::get_string_from_data(std::string key, std::string _default)
{
    validate_key(key);
    cocos2d::UserDefault* ud = cocos2d::UserDefault::getInstance();
    //CCLOG("getting string %s", key.c_str());
    return ud->getStringForKey(key.c_str(), _default);
}

void DataManager::set_string_from_data(std::string key, std::string val)
{
    validate_key(key);
    cocos2d::UserDefault* ud = cocos2d::UserDefault::getInstance();
    //CCLOG("setting string %s for %s", key.c_str(), val.c_str());
    ud->setStringForKey(key.c_str(), val);
};


void DataManager::decr_key(std::string key, int x)
{
    validate_key(key);
    cocos2d::UserDefault* ud = cocos2d::UserDefault::getInstance();
    int val = ud->getIntegerForKey(key.c_str());
    ud->setIntegerForKey(key.c_str(), val - x);
};

void DataManager::decr_key(std::string key, double x)
{
    validate_key(key);
    cocos2d::UserDefault* ud = cocos2d::UserDefault::getInstance();
    double val = ud->getDoubleForKey(key.c_str());
    ud->setDoubleForKey(key.c_str(), val - x);
};

void DataManager::incr_key(std::string key, int x)
{
    validate_key(key);
    cocos2d::UserDefault* ud = cocos2d::UserDefault::getInstance();
    int val = ud->getIntegerForKey(key.c_str());
    ud->setIntegerForKey(key.c_str(), val + x);
};

void DataManager::incr_key(std::string key, double x)
{
    validate_key(key);
    cocos2d::UserDefault* ud = cocos2d::UserDefault::getInstance();
    double val = ud->getDoubleForKey(key.c_str());
    if (val != val + 3)
    {
        CCLOG("we're rounding stuff out now %s", std::to_string(val).c_str());
    }
    ud->setDoubleForKey(key.c_str(), val + x);
};
