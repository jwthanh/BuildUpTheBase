#include "ProgressBar.h"

#include "Util.h"
#include "constants.h"
#include "NuItem.h"

#include "2d/CCSprite.h"
#include "2d/CCProgressTimer.h"
#include "2d/CCLabel.h"
#include "2d/CCActionProgressTimer.h"
#include "2d/CCActionInstant.h"
#include <2d/CCLayer.h>

ProgressBar::ProgressBar(
    std::string front_sprite_frame_path="",
    std::string back_sprite_frame_path=""
    )
{
    this->target_percentage = 100.0f;

    this->base_node = cocos2d::Node::create();

    auto get_sprite = [](std::string sprite_path, std::string frame_path)
    {
        cocos2d::Sprite* result;
        if (sprite_path == "") {
            if (frame_path != "")
                result = cocos2d::Sprite::createWithSpriteFrameName(frame_path);
            else
                result = cocos2d::Sprite::create();
        }
        else {
            result = cocos2d::Sprite::create(sprite_path);
        };

        return result;
    };

    cocos2d::Sprite* front_sprite = get_sprite("", front_sprite_frame_path);
    this->front_timer = cocos2d::ProgressTimer::create(front_sprite);
    cocos2d::Sprite* slider_sprite = get_sprite("whitebar.png", "");
    this->back_timer = cocos2d::ProgressTimer::create(slider_sprite);

    cocos2d::Sprite* back_sprite = get_sprite("", back_sprite_frame_path);
    this->background = back_sprite;
    this->base_node->addChild(this->background);

    this->lbl = cocos2d::Label::createWithTTF("", DEFAULT_FONT, 40);
    this->lbl->setTTFConfig(NuItem::ttf_config);
    this->lbl->setScale(0.5f);
    this->lbl->setAnchorPoint(cocos2d::Vec2::ANCHOR_MIDDLE);
    this->lbl->getFontAtlas()->setAliasTexParameters();

    auto front_size = this->front_timer->getContentSize();
    this->lbl->setPosition(cocos2d::Vec2(
        front_size.width/2.0f,
        front_size.height/2.0f
    ));
    this->front_timer->addChild(this->lbl);

    this->background->setLocalZOrder(99);
    this->back_timer->setLocalZOrder(100); //prog bars on top of everything
    this->front_timer->setLocalZOrder(101);
    this->lbl->setLocalZOrder(102);

    this->base_node->addChild(this->back_timer);
    this->base_node->addChild(this->front_timer);

    this->color_layer = cocos2d::LayerColor::create(cocos2d::Color4B(255, 255, 255, 25));
    this->front_timer->addChild(this->color_layer);
    this->color_layer->setContentSize(this->front_timer->getContentSize());

    this->wait_to_clear = true;


    for (cocos2d::ProgressTimer* timer : {this->front_timer, this->back_timer})
    {
        timer->setType(cocos2d::ProgressTimer::Type::BAR);
        timer->setBarChangeRate(cocos2d::Vec2(1, 0));
        timer->setAnchorPoint(cocos2d::Vec2(0.5, 0.5));
        timer->setPosition(0, 0);
        timer->setVisible(true);
        timer->setPercentage(100);

        auto t_sprite = timer->getSprite();
        if (t_sprite)
        {
            auto t_tex = t_sprite->getTexture();
            t_tex->setAliasTexParameters();
        };

        timer->setMidpoint(cocos2d::Vec2(0, 0));
    };
    this->background->setAnchorPoint(cocos2d::Vec2(0.5,0.5));

    this->do_finish_bump = false;

    this->setScale(sx(4));

    //GameLogic::getInstance()->beatup->addChild(this->base_node);
};

ProgressBar::~ProgressBar()
{
    this->front_timer->removeFromParentAndCleanup(true);
    this->back_timer->removeFromParentAndCleanup(true);
    this->background->removeFromParentAndCleanup(true);
};

void ProgressBar::update_string(std::string msg)
{
    float font_size = 20.0f;

    if (msg == "Knuckle Avalanche") msg = "Knuckle AVs";
    else if (msg == "Impressive Hook") msg = "Impr. Hook";

    if (msg.size() > 15)
    {
        font_size -= 4;
    }
    else if (msg.size() > 13)
    {
        font_size -= 2;
    };
    auto label = cocos2d::Label::createWithTTF(msg, DEFAULT_FONT, font_size);
    this->lbl = label;


    this->lbl->setScale(0.25f);
    this->lbl->setAnchorPoint(cocos2d::Vec2(0.5f, 0.5f));
    auto front_size = this->front_timer->getContentSize();
    this->lbl->setPosition(cocos2d::Vec2(
        front_size.width/2.0f,
        front_size.height/2.0f
    ));

    this->lbl->setTextColor(cocos2d::Color4B::BLACK);
    this->front_timer->addChild(this->lbl);
};

void ProgressBar::setAnchorPoint(cocos2d::Vec2 vec2)
{
    this->base_node->setAnchorPoint(vec2);
    // this->front_timer->setAnchorPoint(vec2);
    // this->back_timer->setAnchorPoint(vec2);
};

void ProgressBar::setPosition(cocos2d::Vec2 vec2)
{
    this->base_node->setPosition(vec2);
    // this->front_timer->setPosition(vec2);
    // this->back_timer->setPosition(vec2);
    // this->background->setPosition(vec2);
};

void ProgressBar::setScale(float scale)
{
    this->scale = scale;
    this->base_node->setScale(scale);

    // this->front_timer->setScale(scale);
    // this->back_timer->setScale(scale);
    // this->background->setScale(scale);
    this->fit_back_to_front();
};

cocos2d::Vec2 ProgressBar::get_back_scale()
{
    auto front_size = this->front_timer->getContentSize();
    auto back_size = this->back_timer->getContentSize();

    auto scale_x = this->front_timer->getScaleX();
    auto scale_y = this->front_timer->getScaleY();

    return cocos2d::Vec2(
            front_size.width*scale_x/back_size.width,
            front_size.height*scale_y/back_size.height
            );
};

void ProgressBar::fit_back_to_front()
{

    cocos2d::Vec2 scale = this->get_back_scale();

    this->back_timer->setScale(scale.x, scale.y);
};

void ProgressBar::set_percentage(float percentage)
{
    //TODO fix this expecting 1f to 100f and scroll_to_percentage expecting 0f to 1f
    this->target_percentage = percentage;
    this->front_timer->setPercentage(percentage);
    this->back_timer->setPercentage(percentage);
};

void ProgressBar::scroll_to_percentage(float percentage)
{
    if (this->target_percentage == percentage) { return; };

    this->target_percentage = percentage;

    // TODO: handle a bar emptying and then filling up again without janking out
    //bool is_emptying = this->target_percentage*100 < this->front_timer->getPercentage();
    bool is_emptying = true;

    cocos2d::ProgressTimer* fore_timer;
    cocos2d::ProgressTimer* rear_timer;

    if ( is_emptying )
    {
        fore_timer = this->front_timer;
        rear_timer = this->back_timer;
    }
    else
    {
        fore_timer = this->back_timer;
        rear_timer = this->front_timer;
    };

    if (this->wait_to_clear)
    {
        rear_timer->stopAllActions();
    };

    cocos2d::ProgressTo* front_prog_to = cocos2d::ProgressTo::create(0.01f, percentage * 100);

    cocos2d::Sequence* front_sequence = NULL;
    if (this->do_finish_bump && percentage >= 1.0f)
    {
        if (this->front_timer == fore_timer)
        {
            front_sequence = cocos2d::Sequence::create(
                this->get_bump_seq(0.125f, 1.25f), NULL
                );
        }
        else
        {
            front_sequence = cocos2d::Sequence::create(
                this->get_back_bump_seq(0.125f, 1.25f), NULL
                );

        };
    }
    else
    {
        front_sequence = cocos2d::Sequence::create(front_prog_to, NULL);
    };

    fore_timer->runAction(front_sequence);

    //TODO renable this once you figure out the white background fighter thing
    cocos2d::DelayTime* delay = cocos2d::DelayTime::create(1);
    cocos2d::ProgressTo* back_prog_to = cocos2d::ProgressTo::create(0.25f, percentage * 100);
    cocos2d::Sequence* back_sequence = cocos2d::Sequence::create(delay, back_prog_to, NULL);
    rear_timer->runAction(back_sequence);
};

void ProgressBar::setVisible(bool val)
{
    this->front_timer->setVisible(val);
    this->back_timer->setVisible(val);
    this->background->setVisible(val);
};

void ProgressBar::bump(float duration, float new_scale, ProgressBar::Layer layer)
{
    if (layer == Both || layer == Front)
    {
        auto seq = this->get_bump_seq(duration, new_scale);
        this->front_timer->runAction(seq);
    }
    if (layer == Both || layer == Back)
    {
        auto seq = this->get_back_bump_seq(duration, new_scale);
        this->back_timer->runAction(seq);
    };
};

cocos2d::Sequence* ProgressBar::get_bump_seq(float duration, float new_scale)
{
    cocos2d::ScaleTo* sb = cocos2d::ScaleTo::create(duration, this->scale*new_scale);
    cocos2d::ScaleTo* bs = cocos2d::ScaleTo::create(duration, this->scale);
    cocos2d::Sequence* seq = cocos2d::Sequence::create(sb, bs, NULL);

    return seq;
};

cocos2d::Sequence* ProgressBar::get_back_bump_seq(float duration, float new_scale)
{
    cocos2d::ScaleTo* sb = cocos2d::ScaleTo::create(duration, this->get_back_scale().x*new_scale);
    cocos2d::ScaleTo* bs = cocos2d::ScaleTo::create(duration, this->get_back_scale().x);
    cocos2d::CallFunc* cf = cocos2d::CallFunc::create(std::function<void()>([this](){ this->fit_back_to_front(); })); //TODO figure out a way that this isn't necessary to fix the background sticking out too far
    cocos2d::Sequence* seq = cocos2d::Sequence::create(sb, bs, cf, NULL);

    return seq;
};

