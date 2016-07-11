#include <stdio.h>

#include "FileOperation.h"

#include "cocos2d.h"
#include <json/stringbuffer.h>
#include <json/writer.h>
#include <json/prettywriter.h>


rapidjson::Document FileIO::open_json(std::string json_path)
{
    auto file_utils = cocos2d::FileUtils::getInstance();
    std::string jsonBuffer = file_utils->getStringFromFile(json_path);
    auto jsonDoc = rapidjson::Document();
    jsonDoc.Parse(jsonBuffer.c_str());

    return jsonDoc;
}

void FileIO::save_json(std::string json_path, rapidjson::Document& document, bool overwrite)
{
    auto file_utils = cocos2d::FileUtils::getInstance();
    //Use the same folder UserDefaults is in, otherwise it saves in Resources
    std::string full_path = cocos2d::FileUtils::getInstance()->getWritablePath() + json_path;

    std::string data_string;

    //if overwrite, ignore existing file contents, otherwise read it from disk first
    rapidjson::Document existing = rapidjson::Document();

    if (overwrite == false)
    {
        std::string existing_str = file_utils->getStringFromFile(full_path);
        if (existing_str == "" || existing_str == "\n"){ existing_str = "{}"; }

        existing.Parse(existing_str.c_str());

        auto& allocator = document.GetAllocator();

        //looking through existing json values, not adding existing duplicates
        for (rapidjson::Value::MemberIterator itr = existing.MemberBegin(); itr != existing.MemberEnd(); ++itr)
        {
            auto matches_new_member = document.FindMember(itr->name);
            if (matches_new_member != document.MemberEnd())
            {
                CCLOG("found existing key, not re-adding '%s' to the saved document again", itr->name.GetString());
                continue;
            }
            document.AddMember(itr->name, itr->value, allocator);
        };
    }

    rapidjson::StringBuffer buffer;
    rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
    document.Accept(writer);

    data_string = std::string(buffer.GetString(), buffer.GetSize());

    bool success = file_utils->writeStringToFile(data_string, full_path);
    if (!success)
    {
        CCLOG("%s was not saved", full_path.c_str());
    }
}
