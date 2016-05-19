#include "Serializer.h"


#include <json/document.h>
#include <json/stringbuffer.h>
#include <json/writer.h>

#include "cocos2d.h"

#include "FileOperation.h"

BaseSerializer::BaseSerializer(std::string filename)
    : filename(filename)
{

};

rj::Document BaseSerializer::get_document(std::string filename) const
{
    if (filename == "") filename = this->filename;

    return FileIO::open_json(filename);
}

void BaseSerializer::save_document(std::string filename) const
{
    if (filename == "") filename = this->filename;
}
