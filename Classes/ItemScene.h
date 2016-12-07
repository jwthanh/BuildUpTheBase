#ifndef ITEMSCENE_H
#define ITEMSCENE_H

#include "GameLayer.h"

namespace cocos2d
{
	namespace ui
	{
		class Layout;
		class Text;
		class ListView;
		class Button;
	}
}

class ItemScene : public GameLayer
{
	public:
		CREATE_FUNC(ItemScene);
		static cocos2d::Scene* ItemScene::createScene();

		bool init() override;
		cocos2d::ui::Layout* init_panel();
		void init_title_lbl(cocos2d::Node* panel, std::string title);
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
