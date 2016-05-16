#include "MainMenu.h"

#include <algorithm>

#include "Beatup.h"
#include "ui/CocosGUI.h"
#include "SoundEngine.h"
#include "DataManager.h"
#include "Util.h"
#include <editor-support/cocostudio/CCSGUIReader.h>
#include <editor-support/cocostudio/ActionTimeline/CSLoader.h>
#include "attribute.h"
#include "NuMenu.h"
#include "GameLogic.h"
#include "StaticData.h"
#include "MiscUI.h"
#include "Fighter.h"
#include "attribute_container.h"

#include "HouseBuilding.h"
#include "combat.h"

USING_NS_CC;

BuildingNode::BuildingNode()
{
    float width = 50;
    float height = 50;

    this->bg_layer = LayerColor::create(Color4B::GREEN, width, height);
    this->addChild(this->bg_layer);

    this->building_label = Label::createWithTTF("unset bldg", TITLE_FONT, 12);
    this->addChild(this->building_label);


    this->setContentSize(Size(width, height));
};

void BuildingNode::set_building(spBuilding bldg)
{
    this->building = bldg;
};


bool CharacterMenu::init()
{
#ifdef _WIN32
    FUNC_INIT_WIN32(CharacterMenu);
#else
    FUNC_INIT(CharacterMenu);
#endif

    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    // add a label shows "Hello World" create and initialize a label
    std::stringstream ss;
    ss << "Fighter detail: " << this->fighter->name;
    auto label = Label::createWithTTF(ss.str(), menu_font, sx(24));

    // position the label on the center of the screen
    label->setPosition(
        Vec2(
            origin.x + visibleSize.width/2,
            origin.y + visibleSize.height - label->getContentSize().height
        )
    );

    // auto bldg_node = FighterNode::create();
    // bldg_node->setPosition(this->get_center_pos());
    // this->addChild(bldg_node);
    auto resize_btn = [](ui::Button* button) {
        auto lbl_size = button->getTitleRenderer()->getContentSize();

        button->setContentSize(
                Size(
                    lbl_size.width * 1.1f,
                    lbl_size.height * 1.1f
                    )
                );
    };

    auto inst = CSLoader::getInstance();

    int num_cols = 5;

    auto raw_node = inst->createNode("editor/details/character_detail.csb");
    auto original_panel = dynamic_cast<ui::Widget*>(raw_node->getChildByName("Panel_1"));
    original_panel->removeFromParent();

    float panel_width = original_panel->getContentSize().width;
    auto param = ui::LinearLayoutParameter::create();
    auto margin = ui::Margin(0, 10, 10, 0);
    param->setMargin(margin);
    float width = (panel_width+margin.left+margin.right);

    auto character_name = dynamic_cast<ui::Text*>(original_panel->getChildByName("character_name"));
    character_name->setString(fighter->name);

    auto character_art = dynamic_cast<ui::ImageView*>(original_panel->getChildByName("character_art"));
    character_art->loadTexture(fighter->sprite_name, ui::TextureResType::PLIST);

    auto hp_val = dynamic_cast<ui::Text*>(original_panel->getChildByName("hp_val"));
    hp_val->setString(std::to_string((int)fighter->attrs->health->current_val));

    auto xp_val = dynamic_cast<ui::Text*>(original_panel->getChildByName("xp_val"));
    xp_val->setString(std::to_string((int)fighter->xp->total));

    auto dmg_val = dynamic_cast<ui::Text*>(original_panel->getChildByName("dmg_val"));
    dmg_val->setString(std::to_string((int)fighter->attrs->damage->current_val));

    auto bns_val = dynamic_cast<ui::Text*>(original_panel->getChildByName("bns_val"));
    std::string bns_msg = "x1";
    if (this->fighter->has_sword) { bns_msg = "x2"; };
    bns_val->setString(bns_msg);

    auto layout = ui::Layout::create();
    layout->setLayoutType(ui::Layout::Type::VERTICAL);
    layout->setContentSize(Size(width, 500)); 
    layout->setAnchorPoint(Vec2(0.5, 0.5));

    layout->setPosition(this->get_center_pos());

    layout->addChild(original_panel);

    this->addChild(layout);

    // add the label as a child to this layer
    this->addChild(label, 1);

    this->main_lbl = Label::createWithTTF(
        "Character menu",
        this->menu_font,
        this->menu_fontsize
    );
    this->main_lbl->setPosition(
        Vec2(
            origin.x + visibleSize.width/2,
            origin.y + visibleSize.height - this->main_lbl->getContentSize().height-sx(30)
        )
    );
    this->main_lbl->setTextColor(Color4B::WHITE);
    this->addChild(this->main_lbl, 1);

    return true;
};

void CharacterMenu::onKeyPressed(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event *pEvent)
{
    if(keyCode == EventKeyboard::KeyCode::KEY_SPACE 
            || keyCode == EventKeyboard::KeyCode::KEY_ENTER) 
    {
        auto director = Director::getInstance();
        director->popScene();
        pEvent->stopPropagation();
    }
    else  if (keyCode == EventKeyboard::KeyCode::KEY_Q)
    {
    }
};

CharacterMenu* CharacterMenu::create(spFighter fighter)
{
    CharacterMenu *menu = new(std::nothrow) CharacterMenu();
    menu->fighter = fighter; //this should be after init, cause i guess init should fail, but its fine for now.

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

void CharacterMenu::onKeyReleased(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event *pEvent)
{
    Director::getInstance()->popScene();
    pEvent->stopPropagation();
};

void CharacterMenu::onTouchEnded(Touch *touch, Event *event)
{
    GameLayer::onTouchEnded(touch, event);
    Director::getInstance()->popScene();
    event->stopPropagation();
};
