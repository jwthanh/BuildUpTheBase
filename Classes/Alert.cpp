#include "Alert.h"
#include "constants.h"

#include "Util.h"

#include "base/CCDirector.h"
#include "base/CCEventListenerTouch.h"
#include "base/CCEventDispatcher.h"
#include "2d/CCActionInterval.h"
#include "2d/CCActionInstant.h"

USING_NS_CC;

Alert* Alert::create()
{
    //usual cocos create, note that I'm skipping handling errors. should fix that
    Alert* alert = new (std::nothrow) Alert();
    if (alert && alert->init())
    {
        alert->autorelease();
    }

    Size visibleSize = Director::getInstance()->getVisibleSize();
    Size node_size = visibleSize*0.80f;
    alert->setContentSize(node_size);
    alert->setLayoutType(ui::Layout::Type::RELATIVE);

    //alert->setBackGroundColor(Color3B::BLACK);
    //alert->setBackGroundColorOpacity(255/1.5);
    //alert->setBackGroundColorType(ui::LayoutBackGroundColorType::SOLID);

    alert->setBackGroundImage("main_UI_export_10_x4.png", TextureResType::PLIST);
    alert->setBackGroundImageScale9Enabled(true);
    alert->setBackGroundImageColor(Color3B(114, 160, 72));
    //alert->setBackGroundImageOpacity(200.0f);
    //layout->setClippingEnabled(true);

    auto create_txt = [&](std::string msg, ui::RelativeLayoutParameter* param) {
        auto txt = ui::Text::create(msg, DEFAULT_FONT, sx(25.0f));
        Label* lbl = (Label*)txt->getVirtualRenderer();
        lbl->getFontAtlas()->setAliasTexParameters();
        txt->setTextColor(Color4B::BLACK);
        //txt->enableOutline(Color4B::BLACK, 2);

        txt->ignoreContentAdaptWithSize(false); //word wrap or something

        alert->addChild(txt);

        txt->setLayoutParameter(param);
        return txt;
    };

    auto header_param = ui::RelativeLayoutParameter::create();
    header_param->setRelativeName("header_param");
    header_param->setAlign(ui::RelativeAlign::PARENT_TOP_CENTER_HORIZONTAL);
    header_param->setMargin(ui::Margin(sx(10), sy(30), sx(10), sy(10)));

    alert->header_txt = create_txt("Title Here", header_param);

    auto sub_header_param = ui::RelativeLayoutParameter::create();
    sub_header_param->setRelativeName("sub_header_param");
    sub_header_param->setAlign(ui::RelativeAlign::LOCATION_BELOW_CENTER);
    sub_header_param->setMargin(ui::Margin(sx(10), sy(10), sx(10), sy(10)));

    alert->sub_header_txt = create_txt("Sub header", sub_header_param);
    sub_header_param->setRelativeToWidgetName("header_param");

    auto body_param = ui::RelativeLayoutParameter::create();
    body_param->setAlign(ui::RelativeAlign::PARENT_LEFT_CENTER_VERTICAL);
    body_param->setMargin(ui::Margin(sx(30), sy(10), sx(10), sy(10)));
    alert->body_txt = create_txt("Body content", body_param);

    Size body_size = alert->body_txt->getAutoRenderSize();
    alert->body_txt->setTextAreaSize(Size(
        body_size.width,
        body_size.height
    ));


    auto close_btn = ui::Button::create();
    close_btn->addTouchEventListener([alert](Ref*, TouchEventType type)
    {
        if (type == TouchEventType::ENDED)
        {
            Size visibleSize = Director::getInstance()->getVisibleSize();
            Vec2 origin = Director::getInstance()->getVisibleOrigin();

            Vec2 pos = Vec2(
                origin.x + visibleSize.width - 20,
                origin.y + 20
                );
            alert->shrink_close(pos);
        };

    });

    close_btn->setTitleText("X");
    close_btn->setTitleColor(Color3B::RED);
    close_btn->setTitleFontSize(sx(40.0f));
    close_btn->getTitleRenderer()->enableOutline(Color4B::GRAY, 10);
    close_btn->setScaleX(sx(1.0f));
    close_btn->setScaleY(sy(1.0f));
    alert->close_btn = close_btn;

    ui::RelativeLayoutParameter* close_param = ui::RelativeLayoutParameter::create();
    close_param->setAlign(ui::RelativeLayoutParameter::RelativeAlign::PARENT_TOP_RIGHT);
    close_param->setMargin(ui::Margin(sx(30), sy(20), sx(30), sy(30)));
    alert->close_btn->setLayoutParameter(close_param);
    alert->addChild(alert->close_btn);

    auto done_btn = ui::Button::create();
    done_btn->addTouchEventListener([alert](Ref*, TouchEventType type)
    {
        if (type == TouchEventType::ENDED)
        {
            Size visibleSize = Director::getInstance()->getVisibleSize();
            Vec2 origin = Director::getInstance()->getVisibleOrigin();

            Vec2 pos = Vec2(
                origin.x + visibleSize.width - 20,
                origin.y + 20
                );
            alert->shrink_close(pos);
        };

    });

    done_btn->setScale9Enabled(true);
    done_btn->loadTextureNormal("main_UI_export_10_scale9_x4.png", ui::TextureResType::PLIST);
    done_btn->setTitleText("Done");
    done_btn->setTitleFontSize(40.0f);
    done_btn->setTitleFontName(DEFAULT_FONT);
    auto lbl_size = done_btn->getTitleRenderer()->getContentSize();
    done_btn->setContentSize(
            Size(
                lbl_size.width * 1.5f,
                lbl_size.height * 1.5f
                )
            );

    done_btn->ignoreContentAdaptWithSize(false); //word wrap or something

    done_btn->setTitleColor(Color3B::BLACK);
    done_btn->setScaleX(sx(1.0f));
    done_btn->setScaleY(sy(1.0f));
    alert->done_btn = done_btn;


    ui::RelativeLayoutParameter* done_param = ui::RelativeLayoutParameter::create();
    done_param->setAlign(ui::RelativeLayoutParameter::RelativeAlign::PARENT_BOTTOM_CENTER_HORIZONTAL);
    done_param->setMargin(ui::Margin(sx(30), sy(20), sx(30), sy(30)));
    done_param->setRelativeName("done_btn");
    alert->done_btn->setLayoutParameter(done_param);
    alert->addChild(alert->done_btn);

    auto listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);

    listener->onTouchBegan = CC_CALLBACK_2(Alert::onTouchBegan, alert);
    listener->onTouchEnded = CC_CALLBACK_2(Alert::onTouchEnded, alert);

    Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, alert);

    return alert;
};

bool Alert::onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event)
{
    // CCLOG("onTouchBegan alert");
    return Widget::onTouchBegan(touch, event);
};

void Alert::onTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event)
{
    Widget::onTouchEnded(touch, event);

};

Alert* Alert::create(std::string header_msg, std::string sub_header_msg, std::string body_msg)
{
    Alert* alert = Alert::create();

    alert->set_header_str(header_msg);
    alert->set_sub_header_str(sub_header_msg);
    alert->set_body_str(body_msg);

    return alert;
};

void Alert::set_header_str(std::string msg)
{
    this->header_txt->setString(msg);
    this->header_txt->setContentSize(this->header_txt->getVirtualRendererSize());
};

void Alert::set_sub_header_str(std::string msg)
{
    this->sub_header_txt->setString(msg);
    this->sub_header_txt->setContentSize(this->sub_header_txt->getVirtualRendererSize());
};

void Alert::set_body_str(std::string msg)
{
    this->body_txt->setString(msg);
    this->body_txt->setContentSize(this->body_txt->getVirtualRendererSize()); //readjusts content size based on label size

    Size body_size = this->body_txt->getAutoRenderSize();
    this->body_txt->setTextAreaSize(Size( //for wordwrap, i think it takes 80%
        body_size.width,
        body_size.height
    ));
};

void Alert::shrink_close(Vec2 pos)
{
    float duration = 0.25f;

    auto scale = ScaleTo::create(duration, 0.01f, 0.01f);
    auto move = MoveTo::create(duration, pos);
    auto clean = CallFunc::create(
        std::function<void()>(
            std::bind(&Alert::removeFromParentAndCleanup, this, true)
        )
    );
    Sequence* seq = Sequence::create(Spawn::createWithTwoActions(move, scale), clean, NULL);
    this->runAction(seq);

};

void Alert::grow_open(Vec2 pos)
{
    float duration = 0.25f;

    auto scale = ScaleTo::create(duration, 1.0f, 1.0f);
    auto move = MoveTo::create(duration, pos);

    Sequence* seq = Sequence::create(Spawn::createWithTwoActions(move, scale), NULL);
    this->runAction(seq);
};
