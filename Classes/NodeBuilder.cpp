#include "NodeBuilder.h"

#include "cocostudio/ActionTimeline/CSLoader.h"

std::unordered_map<std::string, cocos2d::Node*> node_cache = {};

cocos2d::Node* get_prebuilt_node_from_csb(std::string path)
{
	auto inst = cocos2d::CSLoader::getInstance();
	cocos2d::Node* node =  inst->createNode(path);
	return node;

	//TODO figure out a way to clone stuff well, and we can use the follow code
	// right now, we don't use the returned node consistently so it gets deleted
	// or something when it goes out of scope
	//if (node_cache.find(path) == node_cache.end())
	//{
	//	auto inst = cocos2d::CSLoader::getInstance();
	//	cocos2d::Node* node =  inst->createNode(path);
	//	node->retain();
	//	node_cache[path] = node;

	//	return node;
	//}
	//else
	//{
	//	return node_cache[path];
	//}

};
