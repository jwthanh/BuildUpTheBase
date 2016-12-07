#ifndef ITEMSCENE_H
#define ITEMSCENE_H

#include "GameLayer.h"

class ItemScene : public GameLayer
{
	public:
		CREATE_FUNC(ItemScene);
		bool init();
		static cocos2d::Scene* ItemScene::createScene();
};
#endif
