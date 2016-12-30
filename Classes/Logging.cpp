#include "Logging.h"

#include "CCFileUtils.h" //if any cocos file is after easylogging.h there's an error
#include "external/easylogging.h"


std::string Logger::get_log_contents()
{
    el::Logger* l = el::Loggers::getLogger("default");

    const std::string filename = l->typedConfigurations()->filename(el::Level::Global);
    return cocos2d::FileUtils::getInstance()->getStringFromFile(filename);
};
