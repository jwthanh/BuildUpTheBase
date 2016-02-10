#include <stdio.h>

#include "FileOperation.h"

#include "cocos2d.h"


rapidjson::Document FileIO::open_json(std::string json_path)
{
    auto file_utils = cocos2d::FileUtils::getInstance();
    std::string jsonBuffer = file_utils->getStringFromFile(json_path);
    auto jsonDoc = rapidjson::Document();
    jsonDoc.Parse(jsonBuffer.c_str());

    return jsonDoc;
}
