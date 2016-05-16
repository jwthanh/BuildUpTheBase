#include "DataManager.h"

#include "cocos2d.h"

void DataManager::validate_key(std::string key)
{
    if (key == "")
    {
        cocos2d::log("found empty string: the key \"%s\"", key.c_str());
        assert(false && "Cant have empty keys in userdefault keys");
    };

    auto pos = key.find(" ");
    bool valid_string = pos == std::string::npos;
    if (!valid_string)
    {
        cocos2d::log("found a space in the key \"%s\"", key.c_str());
        assert(valid_string && "Cant have spaces in userdefault keys");
    }
}


bool DataManager::get_bool_from_data(std::string key, bool _default)
{
    validate_key(key);
    cocos2d::UserDefault* ud = cocos2d::UserDefault::getInstance();
    return ud->getBoolForKey(key.c_str(), _default);
};

void DataManager::set_bool_from_data(std::string key, bool val)
{
    validate_key(key);
    cocos2d::UserDefault* ud = cocos2d::UserDefault::getInstance();
    ud->setBoolForKey(key.c_str(), val);
};

int DataManager::get_int_from_data(std::string key)
{
    validate_key(key);
    cocos2d::UserDefault* ud = cocos2d::UserDefault::getInstance();
    return ud->getIntegerForKey(key.c_str());
};

double DataManager::get_double_from_data(std::string key)
{
    validate_key(key);
    cocos2d::UserDefault* ud = cocos2d::UserDefault::getInstance();
    return ud->getDoubleForKey(key.c_str());
}

void DataManager::set_double_from_data(std::string key, double val)
{
    validate_key(key);
    cocos2d::UserDefault* ud = cocos2d::UserDefault::getInstance();
    ud->setDoubleForKey(key.c_str(), val);
};

void DataManager::set_int_from_data(std::string key, int val)
{
    validate_key(key);
    cocos2d::UserDefault* ud = cocos2d::UserDefault::getInstance();
    ud->setIntegerForKey(key.c_str(), val);
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
    //CCASSERT(val != val + 3, "how does math work????");
    if (val != val + 3)
    {
        CCLOG("we're rounding stuff out now %s", std::to_string(val));
    }
    ud->setDoubleForKey(key.c_str(), val + x);
};
