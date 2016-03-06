#include "NuMenu.h"
#include <cocos2d/cocos/editor-support/cocostudio/ActionTimeline/CSLoader.h>

#include "BaseMenu.h"
#include "Util.h"

void NuMenu::menu_init()
{
    //setup title
    //setup backbutton

    //setup scrollable
    auto scrollview = this->create_center_scrollview();

    //setup menu items
    auto inst = cocos2d::CSLoader::getInstance();
    for (auto i : {1,2,3,4,5}) {
        cocos2d::ui::Button* menu_item = (cocos2d::ui::Button*)inst->createNode("editor/buttons/menu_item.csb")->getChildByName("menu_item_btn");
        menu_item->loadTextures("main_UI_export_10_x4.png", "main_UI_export_10_x4_pressed.png", "main_UI_export_10_x4_disabled.png", cocos2d::ui::TextureResType::PLIST);
        menu_item->removeFromParent();
        scrollview->addChild(menu_item);

        if (i == 1) { menu_item->setEnabled(false); }
        if (i == 2) { menu_item->setBright(false); }
        if (i == 3) { menu_item->setEnabled(true); }
        if (i == 4) { menu_item->setBright(true); }
    };

    float extra_margins = scrollview->getChildrenCount()*(sy(15.0f)*sy(1.0f));
    int scroll_w = sx(800);
    scrollview->setInnerContainerSize(cocos2d::Size(
        scroll_w,
        scrollview->get_accumulated_size().height + extra_margins
    ));

};
