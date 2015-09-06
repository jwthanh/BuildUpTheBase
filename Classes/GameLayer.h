#ifndef GAMELAYER_H
#define GAMELAYER_H

#include "cocos2d.h"
#include "ui/CocosGUI.h"

#include "constants.h"



class Scrollable;

class GameLayer : public cocos2d::Layer
{
    protected:
        bool isTouchDown;

        float initialTouchPos[2];
        float currentTouchPos[2];
    public:
        static std::string menu_font;
        static int menu_fontsize;
        static int menu_heightdiff;

        static cocos2d::Scene* createScene();

        cocos2d::Vec2 get_center_pos(float offset_x = 0, float offset_y = 0);

        virtual void onKeyReleased(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event *pEvent) = 0;
        virtual void pop_scene(cocos2d::Ref* pSender);
        virtual void quit(cocos2d::Ref* pSender);

        virtual bool onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event);
        virtual void onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event);
        virtual void onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event);
        virtual void onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event);

        virtual void onEnter();

        virtual void onSwipeLeft(float dt);
        virtual void onSwipeUp(float dt);
        virtual void onSwipeRight(float dt);
        virtual void onSwipeDown(float dt);

        virtual bool init();
        virtual void update(float dt);

        bool touch_in_node(Node* target, cocos2d::Touch* touch, float scale=1);
        bool vec2_in_node(Node* target, cocos2d::Vec2 pos);
        bool vec2_in_rect(cocos2d::Rect* rect, cocos2d::Vec2 pos);

        typedef std::function<void(cocos2d::ui::Button* btn, bool& current)> VoidFuncBtnBool;
        struct ButtonState
        {
            bool current; //the current state
            VoidFuncBtnBool swap_state; //do something to the button 
        };

        Scrollable* create_center_scrollview();
        cocos2d::ui::Button* create_button(
            std::string text, BoolFuncNoArgs cb, bool start_disabled = false,
            ButtonState* btn_state = NULL
        );

};

void do_vibrate(int milliseconds);

#ifdef __ANDROID__
void vibrate(int milliseconds);
#else
#endif

#define FUNC_INIT(__TYPE__) \
    if ( !Layer::init() ) \
{ \
    return false; \
} \
GameLayer::init(); \
\
\
auto pKeybackListener = EventListenerKeyboard::create(); \
pKeybackListener->onKeyReleased = CC_CALLBACK_2(__TYPE__::onKeyReleased, this); \
\
_eventDispatcher->addEventListenerWithSceneGraphPriority(pKeybackListener, this); \
\
\
auto listener = EventListenerTouchOneByOne::create(); \
listener->setSwallowTouches(true); \
\
listener->onTouchBegan = CC_CALLBACK_2(__TYPE__::onTouchBegan, this); \
listener->onTouchMoved = CC_CALLBACK_2(__TYPE__::onTouchMoved, this); \
listener->onTouchEnded = CC_CALLBACK_2(__TYPE__::onTouchEnded, this); \
listener->onTouchCancelled = CC_CALLBACK_2(__TYPE__::onTouchCancelled, this); \
\
Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this); \
\
\
this->schedule(schedule_selector(__TYPE__::update)); \


#define FUNC_INIT_WIN32(__TYPE__) \
    if ( !Layer::init() ) \
{ \
    return false; \
} \
GameLayer::init(); \
\
\
auto pKeybackListener = EventListenerKeyboard::create(); \
pKeybackListener->onKeyPressed = CC_CALLBACK_2(__TYPE__::onKeyReleased, this); \
\
_eventDispatcher->addEventListenerWithSceneGraphPriority(pKeybackListener, this); \
\
\
auto listener = EventListenerTouchOneByOne::create(); \
listener->setSwallowTouches(true); \
\
listener->onTouchBegan = CC_CALLBACK_2(__TYPE__::onTouchBegan, this); \
listener->onTouchMoved = CC_CALLBACK_2(__TYPE__::onTouchMoved, this); \
listener->onTouchEnded = CC_CALLBACK_2(__TYPE__::onTouchEnded, this); \
listener->onTouchCancelled = CC_CALLBACK_2(__TYPE__::onTouchCancelled, this); \
\
Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this); \
\
\
this->schedule(schedule_selector(__TYPE__::update)); \


#define FUNC_CREATESCENE(__TYPE__) \
    Scene* __TYPE__::createScene() \
{ \
    auto scene = Scene::create(); \
    \
    auto* layer = __TYPE__::create(); \
    \
    scene->addChild(layer); \
    \
    return scene; \
}; \

#endif
