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
    //auto file_utils = cocos2d::FileUtils::getInstance();

    //rapidjson::StringBuffer buffer;
    //rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    ////rapidjson::Writer<rapidjson::StringBuffer, rapidjson::Document::EncodingType, rapidjson::ASCII<>> writer(buffer);
    //document.Accept(writer);
    ////std::string data_str = std::string(buffer.GetString(), buffer.GetSize());
    ////CCLOG("buffer json being written:\n%s", data_str.c_str());
    //cocos2d::Data data;
    //data.copy((const unsigned char*)buffer.GetString(), buffer.GetSize());
    //file_utils->writeDataToFile(data, json_path);
    auto file_utils = cocos2d::FileUtils::getInstance();

    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    //rapidjson::Writer<rapidjson::StringBuffer, rapidjson::Document::EncodingType, rapidjson::ASCII<>> writer(buffer);
    document.Accept(writer);
    //std::string data_str = std::string(buffer.GetString(), buffer.GetSize());
    //CCLOG("buffer json being written:\n%s", data_str.c_str());
    cocos2d::Data data;
    CCLOG("size: %i", buffer.GetSize());
    std::string str(buffer.GetString(), buffer.GetSize());
    //data.copy((const unsigned char*)buffer.GetString(), buffer.GetSize());
    CCLOG("buffer json being written:\n%s", str.c_str());
    file_utils->writeStringToFile(str, json_path);
    //file_utils->writeDataToFile(data, this->filename);
}
