#include <stdio.h>

#include "FileOperation.h"

#include "cocos2d.h"
#include <json/stringbuffer.h>
#include <json/writer.h>
#include <json/prettywriter.h>
#include "constants.h"

int read_counts = 0;

std::string FileIO::get_correct_path(const std::string& path, bool builtin_path)
{
    
    std::string corrected_path = path;
    auto file_utils = cocos2d::FileUtils::getInstance();
    if (builtin_path == false)
    {
        corrected_path = cocos2d::FileUtils::getInstance()->getWritablePath() + path;
    }

    return corrected_path;
}

std::string FileIO::get_string_from_file(const std::string& path, bool builtin_path)
{

    std::string corrected_path = FileIO::get_correct_path(path, builtin_path);

    CCLOG("read count %i", read_counts);
    read_counts += 1;

    auto file_utils = cocos2d::FileUtils::getInstance();
    return file_utils->getStringFromFile(corrected_path);
}

rjDocument FileIO::open_json(std::string& json_path, bool builtin_path)
{
    std::string jsonBuffer = FileIO::get_string_from_file(json_path, builtin_path);
    auto jsonDoc = rjDocument();
    jsonDoc.Parse(jsonBuffer.c_str());

    return jsonDoc;
}

void FileIO::save_json(std::string& json_path, rapidjson::GenericDocument<rapidjson::UTF8<>,rapidjson::CrtAllocator>& document, bool overwrite)
{
    auto file_utils = cocos2d::FileUtils::getInstance();
    //Use the same folder UserDefaults is in, otherwise it saves in Resources
    std::string full_path = cocos2d::FileUtils::getInstance()->getWritablePath() + json_path;

    std::string data_string;

    //if overwrite, ignore existing file contents, otherwise read it from disk first
    typedef rapidjson::GenericDocument<rapidjson::UTF8<>,rapidjson::CrtAllocator> Document;
    Document existing = Document();

    if (overwrite == false)
    {
        std::string existing_str = "";
        bool file_exists = file_utils->isFileExist(full_path);
        if (file_exists == false)
        {
            existing_str = "{}";
        }
        else
        {
            existing_str = file_utils->getStringFromFile(full_path);
            if (existing_str == "" || existing_str == "\n")
            {
                existing_str = "{}";
            }

        }

        existing.Parse(existing_str.c_str());

        auto& allocator = document.GetAllocator();

        //looking through existing json values, not adding existing duplicates
        for (auto itr = existing.MemberBegin(); itr != existing.MemberEnd(); ++itr)
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
