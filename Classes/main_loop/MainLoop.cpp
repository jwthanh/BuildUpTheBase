#include "MainLoop.h"
#include <base/CCDirector.h>
#include <base/CCScheduler.h>

MainLoop::MainLoop() {
    this->seconds_played = 0.0f; //set in GameLogic::load_all
};

void MainLoop::schedule() {
    cocos2d::Director::getInstance()->getScheduler()->schedule(
            CC_CALLBACK_1(MainLoop::update, this), this, 1.0f / 60, false, "key");
};

void MainLoop::unschedule() {
    cocos2d::Director::getInstance()->getScheduler()->unschedule("key", this);
};

void MainLoop::update(float dt)
{
    this->seconds_played += dt;
}
