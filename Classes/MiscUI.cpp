#include "MiscUI.h"

#include "Util.h"

#include "ui/CocosGUI.h"
#include "base/CCConfiguration.h"
#include "2d/CCActionEase.h"
#include "2d/CCActionInstant.h"
#include "2d/CCSprite.h"
#include "base/CCDirector.h"

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
    set_aliasing((Label*)this);
    this->enableOutline(Color4B::BLACK, 2);

    float x_scale = sx(x + (x_variation * CCRANDOM_MINUS1_1()));
    float y_scale = sy(y + (y_variation * CCRANDOM_MINUS1_1()));

    ccBezierConfig config = {
        Vec2(x_scale, y_scale),
        Vec2(x_scale, x_scale-y_scale),
        Vec2(y_scale-x_scale, y_scale)
    };

    duration += (CCRANDOM_MINUS1_1()*0.30f*duration);

    auto move_action = EaseIn::create(BezierBy::create(duration, config), 2.3f);

    this->runAction(TintTo::create(duration*6, Color3B::RED));

    this->setScale(0.10f);
    this->runAction(Sequence::createWithTwoActions(
        ScaleTo::create(0.1f, 1.0f),
        ScaleBy::create(duration*6, 0.35f)
    ));

    this->runAction(FadeOut::create(duration));

    this->runAction(
        Sequence::createWithTwoActions(
            move_action,
            RemoveSelf::create()
        )
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

int FLASH_ACTION_TAG = 10; //arbitrarily 10
void animate_flash_action(
    cocos2d::Node* target, float duration, float scale,
    float original_scale, cocos2d::Color3B to_color /* RED */
    , cocos2d::Color3B end_color /* WHITE */
)
{
    //clear previous flash animation
    target->stopActionByTag(FLASH_ACTION_TAG);

    auto tint = TintTo::create(duration, to_color);
    auto tint_rev = TintTo::create(duration, end_color);

    auto scale_to = ScaleTo::create(duration, scale);
    auto scale_rev = ScaleTo::create(duration, original_scale);
    auto flash_sequence = Sequence::create(
        Spawn::createWithTwoActions(tint, EaseBackOut::create(scale_to)),
        DelayTime::create(duration * 2),
        Spawn::createWithTwoActions(tint_rev, scale_rev),
        NULL
        );
    flash_sequence->setTag(FLASH_ACTION_TAG);
    target->runAction(flash_sequence);

};

void animate_modal_open(
        cocos2d::Node* target,
        float duration,
        float end_scale,
        cocos2d::Vec2 start_pos,
        cocos2d::Vec2 end_pos
    )
{
    //animate
    target->setPosition(start_pos);

    target->setScale(0);

    auto scale_to_end_scale = EaseOut::create(
        ScaleTo::create(duration, end_scale, end_scale),
        1.5f
    );

    if (end_pos == Vec2{-1, -1})
    {
        end_pos = get_center_pos();
    }

    auto move_to_end_pos = MoveTo::create(duration, end_pos);

    Sequence* seq = Sequence::create(
        EaseBackOut::create(
            Spawn::createWithTwoActions(move_to_end_pos, scale_to_end_scale)
        ),
        NULL
    );
    target->runAction(seq);
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

void set_aliasing(cocos2d::Label* label, bool set_aliasing)
{
    // label->setTTFConfig(NuItem::ttf_config); //force labels to also use the same fontsize and outline but it doesn't seem to affect the draw calls
    if (set_aliasing)
    {
        label->getFontAtlas()->setAliasTexParameters();
    }
    else
    {
        label->getFontAtlas()->setAntiAliasTexParameters();
    }
};

void set_aliasing(cocos2d::Texture2D* texture, bool val)
{
    if (val)
    {
        texture->setAliasTexParameters();
    }
    else
    {
        texture->setAntiAliasTexParameters();
    }
};

void set_aliasing(cocos2d::Sprite* sprite, bool val)
{
    cocos2d::Texture2D* texture = sprite->getTexture();
    set_aliasing(texture, val);
};

void set_aliasing(cocos2d::ui::Text* text, bool val)
{
    Label* renderer = (Label*)text->getVirtualRenderer();
    set_aliasing(renderer, val);
};

void set_aliasing(cocos2d::ui::TextField* text, bool val)
{
    Label* renderer = dynamic_cast<Label*>(text->getVirtualRenderer());
    set_aliasing(renderer, val);
};

void set_aliasing(cocos2d::ui::ImageView* img_view, bool val)
{
    cocos2d::ui::Scale9Sprite* scale9 = dynamic_cast<cocos2d::ui::Scale9Sprite*>(img_view->getVirtualRenderer());
    if (scale9)
    {
        set_aliasing(scale9, val);
        return;
    }
    cocos2d::Sprite* sprite = dynamic_cast<cocos2d::Sprite*>(img_view->getVirtualRenderer());
    if (sprite)
    {
        set_aliasing(sprite, val);
        return;
    }

    CCLOG("set_aliasing:: Not sprite or scale9");
};

void set_aliasing(cocos2d::ui::Button* button, bool val)
{
    set_aliasing(button->getTitleRenderer(), val);
};

void set_aliasing(cocos2d::ui::Scale9Sprite* scale9, bool val)
{
    cocos2d::Sprite* sprite = scale9->getSprite();
    set_aliasing(sprite, val);
};

void image_view_scale9_hack(cocos2d::ui::ImageView* img_view)
{
    img_view->setScale9Enabled(!img_view->isScale9Enabled());
    img_view->setScale9Enabled(!img_view->isScale9Enabled());
};

void prep_button(ui::Button* button)
{
    load_default_button_textures(button);
    button->setTitleFontName("fonts/pixelmix.ttf"); //required
    button->getTitleRenderer()->setTextColor(Color4B::WHITE);
    button->getTitleRenderer()->enableOutline(Color4B::BLACK, 2);
    set_aliasing(button);
}

void prep_back_button(ui::Button* back_button)
{
    prep_button(back_button);
    bind_touch_ended(back_button, [](){ Director::getInstance()->popScene();});
}
