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
        auto menu_item = inst->createNode("editor/buttons/menu_item.csb")->getChildByName("menu_item_btn");
        menu_item->removeFromParent();
        scrollview->addChild(menu_item);
    };

    float extra_margins = scrollview->getChildrenCount()*(sy(15.0f)*sy(1.0f));
    int scroll_w = sx(800);
    scrollview->setInnerContainerSize(cocos2d::Size(
        scroll_w,
        scrollview->get_accumulated_size().height + extra_margins
    ));

};
