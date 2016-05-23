#include <stdio.h>

#include "FileOperation.h"

#include "cocos2d.h"
#include <json/stringbuffer.h>
#include <json/writer.h>


rapidjson::Document FileIO::open_json(std::string json_path)
{
    auto file_utils = cocos2d::FileUtils::getInstance();
    std::string jsonBuffer = file_utils->getStringFromFile(json_path);
    auto jsonDoc = rapidjson::Document();
    jsonDoc.Parse(jsonBuffer.c_str());

    return jsonDoc;
}

void FileIO::save_json(std::string json_path, rapidjson::Document& document)
{
    auto file_utils = cocos2d::FileUtils::getInstance();

    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);

    document.Accept(writer);
    std::string str(buffer.GetString(), buffer.GetSize());

    file_utils->writeStringToFile(str, json_path);
}
