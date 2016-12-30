#ifndef __fileoperation__
#define __fileoperation__

#include <string>
#include <json/document.h>
#include "constants.h"

class FileIO
{
    public:
        //builtin paths are in Resources, otherwise its in UserDefaults folder
        static std::string get_correct_path(const std::string& path, bool builtin_path = true);

        static std::string get_string_from_file(const std::string& path, bool builtin_path = true);

        static rjDocument open_json(std::string& json_path, bool builtin_path = true);
        static void save_json(std::string& json_path, rapidjson::GenericDocument<rapidjson::UTF8<>,rapidjson::CrtAllocator>& document, bool overwrite=false);
};

#endif
