#include "NuMenu.h"
#include <cocos2d/cocos/editor-support/cocostudio/ActionTimeline/CSLoader.h>

USING_NS_CC;

#include "BaseMenu.h"
#include "Util.h"

NuMenu* NuMenu::create(Beatup* beatup)
{
    NuMenu *menu = new(std::nothrow) NuMenu();
    menu->beatup = beatup; //this should be after init, cause i guess init should fail, but its fine for now.

    if (menu && menu->init()) {
        menu->autorelease();
        return menu;
    }
    else
    {
        delete menu;
        menu = nullptr; 
        return menu;
    }
};

bool NuMenu::init()
{

#ifdef _WIN32
    FUNC_INIT_WIN32(NuMenu);
#else
    FUNC_INIT(NuMenu);
#endif

    //setup title
    //setup backbutton

    //setup scrollable
    auto scrollview = this->create_center_scrollview();

    //setup menu items
    auto inst = cocos2d::CSLoader::getInstance();
    for (auto i : {1,2,3,4,5}) {
        cocos2d::ui::Button* menu_item = static_cast<cocos2d::ui::Button*>(inst->createNode("editor/buttons/menu_item.csb")->getChildByName("menu_item_btn"));
        menu_item->loadTextures("main_UI_export_10_x4.png", "main_UI_export_10_x4_pressed.png", "main_UI_export_10_x4_disabled.png", cocos2d::ui::TextureResType::PLIST);
        menu_item->removeFromParent();
        scrollview->addChild(menu_item);

        if (i == 1) { menu_item->setEnabled(false); }
        if (i == 2) { menu_item->setBright(false); }
        if (i == 3) { menu_item->setEnabled(true); }
        if (i == 4) { menu_item->setBright(true); }
    };

    scrollview->resize_to_fit();

    return true;
};
