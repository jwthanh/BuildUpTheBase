#ifndef ITEMSCENE_H
#define ITEMSCENE_H

#include "GameLayer.h"

class ItemScene : public GameLayer
{
	public:
		CREATE_FUNC(ItemScene);
		bool init() override;
		static cocos2d::Scene* ItemScene::createScene();
};

class AltarItemScene : public ItemScene
{
	public:
		CREATE_FUNC(AltarItemScene);
		bool init() override;
		static cocos2d::Scene* AltarItemScene::createScene();
};

class EquipItemScene : public ItemScene
{
	public:
		CREATE_FUNC(EquipItemScene);
		bool init() override;
		static cocos2d::Scene* EquipItemScene::createScene();
};

#endif
