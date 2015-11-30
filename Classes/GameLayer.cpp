#include "GameLayer.h"
#ifdef __ANDROID__
#include "DataManager.h"
#endif
#include "ShopScene.h"
#include "Util.h"
#include "SoundEngine.h"
USING_NS_CC;



std::string GameLayer::menu_font = DEFAULT_FONT;
int GameLayer::menu_fontsize = 0;
int GameLayer::menu_heightdiff = 0;

bool GameLayer::touch_in_node(Node* target, cocos2d::Touch* touch, float scale)
{
    auto bbox = target->getBoundingBox();
    if (scale != 1)
    {
        Rect new_rect(bbox);
        new_rect.size.width = bbox.size.width*scale;
        new_rect.size.height = bbox.size.height*scale;
        new_rect.origin.x -= (bbox.size.width / 2.0)*(scale - 1.0);// - bbox.size.width );
        new_rect.origin.y -= (bbox.size.height / 2.0)*(scale - 1.0);// - bbox.size.height);
        bbox.setRect(
            new_rect.origin.x,
            new_rect.origin.y,
            new_rect.size.width,
            new_rect.size.height
        );
    }
    if (bbox.containsPoint(this->convertTouchToNodeSpace(touch))) 
    {
        return true;
    }
    return false;
};

bool GameLayer::vec2_in_node(Node* target, cocos2d::Vec2 pos)
{
    if (target->getBoundingBox().containsPoint(pos)) 
    {
        return true;
    }
    return false;
};

bool GameLayer::vec2_in_rect(Rect* rect, cocos2d::Vec2 pos)
{
    if (rect->containsPoint(pos))
    {
        return true;
    }
    return false;
}

bool GameLayer::init()
{

    isTouchDown = false;

    initialTouchPos[0] = 0;
    initialTouchPos[1] = 0;

    return true;
};

void GameLayer::update(float dt)
{
    Size visibleSize = Director::getInstance()->getVisibleSize();
    if (true == isTouchDown)
    {
        if (initialTouchPos[0] - currentTouchPos[0] > visibleSize.width * 0.05)
        {
            this->onSwipeLeft(dt);
            isTouchDown = false;
        }
        else if (initialTouchPos[0] - currentTouchPos[0] < - visibleSize.width * 0.05)
        {
            this->onSwipeRight(dt);
            isTouchDown = false;
        }
        else if (initialTouchPos[1] - currentTouchPos[1] > visibleSize.width * 0.05)
        {
            this->onSwipeDown(dt);
            isTouchDown = false;
        }
        else if (initialTouchPos[1] - currentTouchPos[1] < - visibleSize.width * 0.05)
        {
            this->onSwipeUp(dt);
            isTouchDown = false;
        }
        else if (initialTouchPos[0] == currentTouchPos[0] && initialTouchPos[1] == currentTouchPos[1])
        {
            // CCLOG("no move?");
        }
    }
};

bool GameLayer::onTouchBegan(Touch *touch, Event *event)
{
    initialTouchPos[0] = touch->getLocation().x;
    initialTouchPos[1] = touch->getLocation().y;
    currentTouchPos[0] = touch->getLocation().x;
    currentTouchPos[1] = touch->getLocation().y;

    isTouchDown = true;

    return true;
}

void GameLayer::onTouchMoved(Touch *touch, Event *event)
{
    currentTouchPos[0] = touch->getLocation().x;
    currentTouchPos[1] = touch->getLocation().y;
}

void GameLayer::onTouchEnded(Touch *touch, Event *event)
{
    isTouchDown = false;
}

void GameLayer::onTouchCancelled(Touch *touch, Event *event)
{
    onTouchEnded(touch, event);
}

void GameLayer::onSwipeLeft(float dt)
{
    cocos2d::log("SWIPED LEFT");
};

void GameLayer::onSwipeUp(float dt)
{
    CCLOG("SWIPED UP");
};

void GameLayer::onSwipeRight(float dt)
{
    CCLOG("SWIPED RIGHT");
};

void GameLayer::onSwipeDown(float dt)
{
    CCLOG("SWIPED DOWN");
};

void GameLayer::pop_scene(Ref* pSender)
{
    Director::getInstance()->popToRootScene(); //sloppy way to delete scene, idk a better way yet
};

void GameLayer::quit(Ref* pSender)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WP8) || (CC_TARGET_PLATFORM == CC_PLATFORM_WINRT)
    MessageBox("You pressed the close button. Windows Store Apps do not implement a close button.", "Alert");
    return;
#endif

    Director::getInstance()->end();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif
};


Scrollable* GameLayer::create_center_scrollview()
{
    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    auto scroll = Scrollable::create();
    //scroll->setBackGroundColorType(ui::Layout::BackGroundColorType::SOLID);
    int scroll_w = sx(800);
    int scroll_h = sy(400);
    Size scroll_size = Size(scroll_w, scroll_h);
    scroll->setContentSize(scroll_size);

    scroll->setAnchorPoint(Vec2(0.5, 0.5));
    scroll->setPosition( this->get_center_pos(0, -sy(50) ));
    scroll->setDirection(ui::ScrollView::Direction::VERTICAL);

    this->addChild(scroll);
    //scroll->setBackGroundColor(Color3B::GREEN);

    //auto container_size = Size(scroll_w, scroll_h*3);
    // scroll->setInnerContainerSize(container_size);
    scroll->setLayoutType(ui::Layout::Type::VERTICAL);

    // scroll->setBounceEnabled(true); //this sucks, needs to be smoother
    scroll->setScrollBarAutoHideEnabled(false);

    return scroll;
}

cocos2d::ui::Button* GameLayer::create_button(std::string text, BoolFuncNoArgs cb, bool start_disabled, ButtonState* btn_state)
{
    auto param = ui::LinearLayoutParameter::create();
    param->setGravity(ui::LinearLayoutParameter::LinearGravity::CENTER_HORIZONTAL);

    auto disable_button = [](ui::Button* button)
    {
        button->setTitleColor(Color3B::GRAY);
        button->setTouchEnabled(false);
        button->setPressedActionEnabled(false);
    };

    auto btn = ui::Button::create("main_UI_export_10_x4.png", "", "", ui::TextureResType::PLIST);

    btn->setTitleFontName(menu_font);
    btn->setTitleFontSize(menu_fontsize);
    btn->setTitleColor(Color3B::BLACK);
    btn->getTitleRenderer()->getFontAtlas()->setAliasTexParameters();

    btn->setColor(Color3B(114, 160, 72));

    btn->getVirtualRenderer()->setScale(sx(1.0f));
    btn->setScaleX(sx(1.0f));
    btn->setScaleY(sy(1.0f));

    param->setMargin(ui::Margin(0, sy(15.0f)*btn->getScaleY(), 0, sy(15.0f)*btn->getScaleY()));

    btn->setLayoutParameter(param);
    btn->setTouchEnabled(true);
    btn->setPressedActionEnabled(true);

    if (start_disabled) 
    {
        disable_button(btn);
    };

    btn->setTitleText(text);
    if (btn_state)
    {
        //update the text
        btn_state->swap_state(btn, btn_state->current);
    };

    btn->addTouchEventListener(
        [btn_state, cb, btn, disable_button](Ref* ref, ui::Widget::TouchEventType event)
            {
                if (event == ui::Widget::TouchEventType::ENDED)
                {
                    if (!cb())
                    {
                        disable_button(btn);
                    };
                    if (btn_state)
                    {
                        btn_state->current = !btn_state->current;
                        btn_state->swap_state(btn, btn_state->current);
                    };
                }
            }
    );

    return btn;

};

cocos2d::Vec2 GameLayer::get_center_pos(float offset_x, float offset_y)
{
    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();
    Vec2 center_pos = Vec2(
        origin.x + visibleSize.width / 2.0f,
        origin.y + visibleSize.height / 2.0f
    );

    center_pos.x += offset_x;
    center_pos.y += offset_y;

    return center_pos;
};

void GameLayer::onEnter()
{
    Node::onEnter();
    if (!SoundEngine::is_sound_enabled())
    {
        SoundEngine::stop_all_sound_and_music();
    }
};

void GameLayer::onKeyReleased(EventKeyboard::KeyCode keyCode, Event*)
{
    if(keyCode == EventKeyboard::KeyCode::KEY_BACK || keyCode == EventKeyboard::KeyCode::KEY_ESCAPE) 
    {
        Director::getInstance()->popScene();
    }
};

void do_vibrate(int milliseconds)
{
#ifdef __ANDROID__
    std::string vibrate_key = "vibration_enabled";
    if (DataManager::get_bool_from_data(vibrate_key, true))
    {
        vibrate(milliseconds);
    };
#else
#endif

};
