#pragma once
#ifndef NODEBUILDER_H
#define NODEBUILDER_H
#include <string>

namespace cocos2d
{
    class Node;
}

cocos2d::Node* get_prebuilt_node_from_csb(std::string path);

#endif
