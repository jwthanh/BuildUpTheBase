#ifndef __fileoperation__
#define __fileoperation__

#include <string>
#include <json/document.h>

class FileIO 
{
    public:
        static rapidjson::Document open_json(std::string json_path);
        static void save_json(std::string json_path, rapidjson::Document& document, bool overwrite=false);
};

#endif
