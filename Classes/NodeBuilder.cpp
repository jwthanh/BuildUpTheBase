#include "NodeBuilder.h"

#include "cocostudio/ActionTimeline/CSLoader.h"

cocos2d::Node* get_prebuilt_node_from_csb(std::string path)
{
    auto inst = cocos2d::CSLoader::getInstance();
    return inst->createNode(path);
};
