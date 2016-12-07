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
	private:
		const std::string& get_default_detail_panel_title();
		const std::string& get_default_detail_panel_description();

	public:
		CREATE_FUNC(ItemScene);
		static cocos2d::Scene* ItemScene::createScene();

		bool init() override;
		cocos2d::ui::Layout* init_panel();
		void init_title_lbl(cocos2d::Node* panel, std::string title);
		void init_back_btn(cocos2d::Node* panel);

		cocos2d::ui::Button* init_sell_btn(cocos2d::Node* panel);
};

class AltarItemScene : public ItemScene
{
	private:
		const std::string& get_default_detail_panel_title();
		const std::string& get_default_detail_panel_description();

	public:
		CREATE_FUNC(AltarItemScene);
		bool init() override;
		static cocos2d::Scene* AltarItemScene::createScene();
};

class EquipItemScene : public ItemScene
{
	private:
		const std::string& get_default_detail_panel_title();
		const std::string& get_default_detail_panel_description();

	public:
		CREATE_FUNC(EquipItemScene);
		bool init() override;
		static cocos2d::Scene* EquipItemScene::createScene();
};

#endif
