#include "MiscUI.h"

#include "Util.h"

#include "ui/CocosGUI.h"
#include "base/CCConfiguration.h"
#include "2d/CCActionEase.h"
#include "2d/CCActionInstant.h"
#include "2d/CCSprite.h"
#include "base/CCDirector.h"

cocos2d::Label* do_float(float x, float x_variation, float y, float y_variation, float duration)
{
    auto label = cocos2d::Label::createWithBMFont("pixelmix_24x2.fnt", "");
    //label->enableOutline(Color4B::BLACK, 2);

    float x_scale = sx(x + (x_variation * CCRANDOM_MINUS1_1()));
    float y_scale = sy(y + (y_variation * CCRANDOM_MINUS1_1()));

    cocos2d::ccBezierConfig config = {
        cocos2d::Vec2(x_scale, y_scale),
        cocos2d::Vec2(x_scale, x_scale-y_scale),
        cocos2d::Vec2(y_scale-x_scale, y_scale)
    };


    duration += (CCRANDOM_MINUS1_1()*0.30f*duration);
    auto move_action = cocos2d::EaseIn::create(cocos2d::BezierBy::create(duration, config), 2.3f);

    //idk why this was a thing anyway, pretty sure I didn't like it much
    // label->runAction(TintTo::create(duration*6, Color3B::RED));

    label->setScale(0.10f);
    label->runAction(cocos2d::Sequence::createWithTwoActions(
        cocos2d::ScaleTo::create(0.1f, 1.0f),
        cocos2d::ScaleBy::create(duration*6, 0.35f)
    ));

    label->runAction(cocos2d::FadeOut::create(duration));

    label->runAction(
        cocos2d::Sequence::createWithTwoActions(
            move_action,
            cocos2d::RemoveSelf::create()
        )
    );
    return label;
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

int FLASH_ACTION_TAG = 112233442; //arbitrary number chosen
cocos2d::Sequence* build_flash_action(
    float duration,
    float scale, float original_scale,
    cocos2d::Color3B to_color = cocos2d::Color3B::RED, cocos2d::Color3B end_color = cocos2d::Color3B::WHITE
    )
{
    auto tint = cocos2d::TintTo::create(duration, to_color);
    auto tint_rev = cocos2d::TintTo::create(duration, end_color);

    auto scale_to = cocos2d::ScaleTo::create(duration, scale);
    auto scale_rev = cocos2d::ScaleTo::create(duration, original_scale);
    auto flash_sequence = cocos2d::Sequence::create(
        cocos2d::Spawn::createWithTwoActions(tint, cocos2d::EaseBackOut::create(scale_to)),
        cocos2d::DelayTime::create(duration * 2),
        cocos2d::Spawn::createWithTwoActions(tint_rev, scale_rev),
        NULL
        );
    flash_sequence->setTag(FLASH_ACTION_TAG);
    return flash_sequence;
}

void run_flash_action(
    cocos2d::Node* target, float duration = 0.1f, float scale = 1.2f,
    float original_scale = 1.0f, cocos2d::Color3B to_color = cocos2d::Color3B::RED
    , cocos2d::Color3B end_color = cocos2d::Color3B::WHITE
)
{
    //clear previous flash animation
    target->stopActionByTag(FLASH_ACTION_TAG);
    target->runAction(build_flash_action(duration, scale, original_scale, to_color, end_color));

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

    auto scale_to_end_scale = cocos2d::EaseOut::create(
        cocos2d::ScaleTo::create(duration, end_scale, end_scale),
        1.5f
    );

    if (end_pos == cocos2d::Vec2{-1, -1})
    {
        end_pos = get_center_pos();
    }

    auto move_to_end_pos = cocos2d::MoveTo::create(duration, end_pos);

    cocos2d::Sequence* seq = cocos2d::Sequence::create(
        cocos2d::EaseBackOut::create(
            cocos2d::Spawn::createWithTwoActions(move_to_end_pos, scale_to_end_scale)
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

void set_aliasing(cocos2d::ui::TextBMFont* label, bool is_alias)
{
    set_aliasing(dynamic_cast<cocos2d::Label*>(label->getVirtualRenderer()), is_alias);
}

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
    cocos2d::Label* renderer = (cocos2d::Label*)text->getVirtualRenderer();
    set_aliasing(renderer, val);
};

void set_aliasing(cocos2d::ui::TextField* text, bool val)
{
    cocos2d::Label* renderer = dynamic_cast<cocos2d::Label*>(text->getVirtualRenderer());
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


void set_dimensions(cocos2d::Label* label, float width, float height)
{
    label->setDimensions(width, height);
};

void set_dimensions(cocos2d::ui::Text* text, float width, float height)
{
    set_dimensions(dynamic_cast<cocos2d::Label*>(text->getVirtualRenderer()), width, height);
};

void set_dimensions(cocos2d::ui::TextBMFont* text, float width, float height)
{
    set_dimensions(dynamic_cast<cocos2d::Label*>(text->getVirtualRenderer()), width, height);
};

void image_view_scale9_hack(cocos2d::ui::ImageView* img_view)
{
    img_view->setScale9Enabled(!img_view->isScale9Enabled());
    img_view->setScale9Enabled(!img_view->isScale9Enabled());
};

void prep_button(cocos2d::ui::Button* button)
{
    load_default_button_textures(button);
    button->setTitleFontName("fonts/pixelmix.ttf"); //required
    button->getTitleRenderer()->setTextColor(cocos2d::Color4B::WHITE);
    button->getTitleRenderer()->enableOutline(cocos2d::Color4B::BLACK, 2);
    set_aliasing(button);
}

void prep_back_button(cocos2d::ui::Button* back_button)
{
    prep_button(back_button);
    bind_touch_ended(back_button, [](){
                     cocos2d::Director::getInstance()->popScene();});
}
