#include "MiscUI.h"

#include "Util.h"
#include "ui/CocosGUI.h"

USING_NS_CC;

FloatingLabel::FloatingLabel(TextHAlignment hAlignment, TextVAlignment vAlignment)
    : Label(hAlignment, vAlignment)
{
    
};

FloatingLabel* FloatingLabel::createWithTTF(const std::string& text, const std::string& fontFile, float fontSize, const Size& dimensions /* = Size::ZERO */, TextHAlignment hAlignment /* = TextHAlignment::LEFT */, TextVAlignment vAlignment /* = TextVAlignment::TOP */)
{
    auto ret = new (std::nothrow) FloatingLabel(hAlignment,vAlignment);

    if (ret && ret->initWithTTF(text, fontFile, fontSize, dimensions, hAlignment, vAlignment))
    {
        ret->autorelease();
        return ret;
    }

    CC_SAFE_DELETE(ret);
    return nullptr;
}

FloatingLabel* FloatingLabel::createWithTTF(const TTFConfig& ttfConfig, const std::string& text, TextHAlignment hAlignment /* = TextHAlignment::CENTER */, int maxLineWidth /* = 0 */)
{
    auto ret = new (std::nothrow) FloatingLabel(hAlignment);

    if (ret && ret->initWithTTF(ttfConfig, text, hAlignment, maxLineWidth))
    {
        ret->autorelease();
        return ret;
    }

    CC_SAFE_DELETE(ret);
    return nullptr;
}


void FloatingLabel::do_float(float x, float x_variation, float y, float y_variation, float duration)
{
    float x_scale = sx(x + (x_variation * CCRANDOM_0_1()));
    float y_scale = sy(y + (y_variation * CCRANDOM_0_1()));

    ccBezierConfig config = {
        Vec2(x_scale, y_scale),
        Vec2(x_scale, 1),
        Vec2(1, y_scale)
    };


    this->runAction(TintTo::create(duration*3, Color3B::RED));
    this->runAction(ScaleBy::create(duration*3, 0.35f));
    this->runAction(FadeOut::create(duration));
    this->runAction(
        Sequence::createWithTwoActions(
        Repeat::create(
            Sequence::create(
                BezierBy::create(duration, config),
                BezierBy::create(duration, config),
                NULL
            )
            , 10
        ),
        RemoveSelf::create())
    );

};

void load_default_button_textures(cocos2d::ui::Button* button)
{
    button->loadTextures(
        "main_UI_export_10_x4.png",
        "main_UI_export_10_x4_pressed.png",
        "main_UI_export_10_x4_disabled.png",
        cocos2d::ui::TextureResType::PLIST
    );
};

void animate_flash_action(
    cocos2d::Node* target, float duration, float scale,
    cocos2d::Color3B to_color /* RED */, cocos2d::Color3B end_color /* WHITE */
    )
{
    auto tint = TintTo::create(duration, to_color.r, to_color.g, to_color.b);
    auto tint_rev = TintTo::create(duration/2.0f, end_color.r, end_color.g, end_color.b);

    auto scale_to = ScaleTo::create(duration, scale);
    auto scale_rev = ScaleTo::create(duration/2.0f, 1.0f);
    target->runAction(
        Sequence::create(
            Spawn::createWithTwoActions(tint, scale_to),
            Spawn::createWithTwoActions(tint_rev, scale_rev),
            NULL
        )
    );

};

void try_set_text_color(cocos2d::ui::Text* text_node, const cocos2d::Color4B color)
{
    if (text_node->getTextColor() != color)
    {
        text_node->setTextColor(color);
    }
};

void try_set_node_color(cocos2d::Node* node, const cocos2d::Color3B color)
{
    if (node->getColor() != color)
    {
        node->setColor(color);
    }
};
