#ifndef __fileoperation__
#define __fileoperation__

#include <string>
#include <json/document.h>
#include "constants.h"

class FileIO 
{
    public:
        static rjDocument open_json(std::string json_path, bool builtin_path=true);
        static void save_json(std::string json_path, rapidjson::GenericDocument<rapidjson::UTF8<>,rapidjson::CrtAllocator>& document, bool overwrite=false);
};

#endif
