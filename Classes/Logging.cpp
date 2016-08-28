#include "Logging.h"
#include "cocos2d.h"

#include "external/easylogging.h"


std::string Logger::get_log_contents()
{
    el::Logger* l = el::Loggers::getLogger("default");
    return cocos2d::FileUtils::getInstance()->getStringFromFile(l->typedConfigurations()->filename(el::Level::Global));
};
