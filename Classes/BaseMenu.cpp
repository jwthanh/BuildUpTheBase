#include "BaseMenu.h"
#include "Beatup.h"
#include "SoundEngine.h"
#include "DataManager.h"
#include "Clock.h"
#include "Util.h"
#include "MainMenu.h"
#include "constants.h"
#include "GameLogic.h"

USING_NS_CC;

bool BaseMenu::init()
{
#ifdef _WIN32
    FUNC_INIT_WIN32(BaseMenu);
#else
    FUNC_INIT(BaseMenu);
#endif

    BaseMenu::menu_font = DEFAULT_FONT;
    BaseMenu::menu_fontsize = 35;

    SoundEngine::play_music("music\\shop.mp3");

    this->shop_items = new std::vector<ShopItem*>();

    BaseMenu::menu_heightdiff = -1;
    this->last_pos = Vec2(-1, -1);

    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    // add a label shows "Hello World" create and initialize a label
    auto label = Label::createWithTTF("Welcome to the BaseMenu", BaseMenu::menu_font, sx(24));

    // position the label on the center of the screen
    label->setPosition(Vec2(origin.x + visibleSize.width/2,
                origin.y + visibleSize.height - label->getContentSize().height));

    // add the label as a child to this layer
    this->addChild(label, 1);

    this->main_lbl = Label::createWithTTF("0 coins", this->menu_font, this->menu_fontsize);
    this->main_lbl->setPosition(Vec2(
        origin.x + visibleSize.width/2,
        origin.y + visibleSize.height - this->main_lbl->getContentSize().height-30
    ));
    this->addChild(this->main_lbl, 1);

    return true;
} ;

void BaseMenu::set_main_lbl()
{
    std::stringstream ss;
    ss << BEATUP->get_total_coins() << " coins";
    this->main_lbl->setString(ss.str());
};

void BaseMenu::buy_stuff(int cost)
{
    DataManager::decr_key(Beatup::total_coin_key, cost);
    cocos2d::log("spent %d", cost);
    this->set_main_lbl();
    // SoundEngine::play_sound("sounds\\old\\coin.mp3");
    SoundEngine::play_sound("sounds\\new\\coin\\C_coin_1.mp3");
};

bool BaseMenu::can_afford(int cost)
{
    if (BEATUP->get_total_coins() >= cost)
    {
        return true;
    };
    return false;
};

bool BaseMenu::buy(ShopItem* shop_item, int cost, BoolFuncNoArgs on_bought)
{
    if (this->can_afford(cost))
    {
        on_bought();

        if (shop_item && !shop_item->is_infinite_buy) {
            shop_item->is_enabled = false;
            this->buy_stuff(cost);
        }
        else {
            this->buy_stuff(cost);
        }
        return shop_item->is_infinite_buy;
    };
    return true;
};



void BaseMenu::menu_init()
{
    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();


    this->combo_menu = Menu::create();
    this->addChild(this->combo_menu);

    auto buff_scroll = this->create_center_scrollview();
    
    //tab switches
    this->prep_tabs(NULL, buff_scroll, NULL);

    //back button
    this->back_button = MenuItemLabel::create(Label::createWithTTF("Back", BaseMenu::menu_font, BaseMenu::menu_fontsize));
    this->combo_menu->addChild(this->back_button);
    Vec2 corner_pos = Vec2(
        origin.x + visibleSize.width - this->back_button->getContentSize().width/2 ,
        origin.y + this->back_button->getContentSize().height/2
    );

    this->back_button->setPosition(this->convertToNodeSpaceAR(corner_pos));
    this->back_button->setCallback(CC_CALLBACK_1(BaseMenu::pop_scene, this));

    auto pos = this->combo_menu->getPosition();
    pos.y += sy(100);
    this->combo_menu->setPosition(pos);

	this->set_main_lbl();

    buff_scroll->resize_to_fit();
};

void BaseMenu::prep_tabs(Scrollable* combo_scroll, Scrollable* buff_scroll, Scrollable* fist_scroll)
{

    float ydiff = sy(75);

    MenuItemLabel* buff_mil;

     ccMenuCallback buff_cb = [combo_scroll, buff_scroll, fist_scroll](Ref*) {
         combo_scroll->setVisible(false);
         buff_scroll->setVisible(true);
         fist_scroll->setVisible(false);
     };

    auto update_with_count = [](MenuItemLabel* lbl, Scrollable* scrollable) {
        std::string old_str = ((Label*)lbl->getLabel())->getString();
        std::stringstream ss;
        ss << old_str << "(" << scrollable->getChildrenCount() << ")";
        ((Label*)lbl->getLabel())->setString(ss.str().c_str());
    };


    buff_scroll->setVisible(true);
    buff_mil = MenuItemLabel::create(Label::createWithTTF("BOOSTS", BaseMenu::menu_font, BaseMenu::menu_fontsize));
    this->combo_menu->addChild(buff_mil);
    buff_mil->setPosition(this->convertToNodeSpaceAR(Vec2(
       this->get_center_pos().x,
       this->get_center_pos().y + ydiff
    )));
    buff_mil->setCallback(buff_cb);

};

PlainShopItem* BaseMenu::get_psi_by_id(std::string id_key)
{
    PlainShopItem* psi = NULL;
    for (ShopItem* si : *this->shop_items)
    {
        if (si->id_key == id_key)
        {
            psi = static_cast<PlainShopItem*>(si);
        }
    }

    return psi;
};

void BaseMenu::shop_menu_update(float dt)
{
    for (ShopItem* shop_item : *this->shop_items)
    {
        shop_item->update(dt);
    };
};

void BaseMenu::update(float dt)
{
    this->shop_menu_update(dt);
};

void BaseMenu::onKeyReleased(EventKeyboard::KeyCode keyCode, Event *pEvent)
{
    if(keyCode == EventKeyboard::KeyCode::KEY_BACK || keyCode == EventKeyboard::KeyCode::KEY_ESCAPE) 
    {
        pop_scene(NULL);
    }
}

void BaseMenu::pop_scene(Ref* pSender)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WP8) || (CC_TARGET_PLATFORM == CC_PLATFORM_WINRT)
    MessageBox("You pressed the close button. Windows Store Apps do not implement a close button.", "Alert");
    return;
#endif

    BEATUP->update(0);
    Director::getInstance()->pushScene(this->beatup_scene);

}

ShopItem::ShopItem(BaseMenu* shop, Menu* menu, ui::Button* button, std::string id_key) : button(button), Buyable(id_key)
{
    this->afford_color = std::shared_ptr<Color3B>(new Color3B(0, 255, 0));
    this->cant_afford_color = std::shared_ptr<Color3B>(new Color3B(255, 0, 0));
    this->disabled_color = std::shared_ptr<Color3B>(new Color3B(25, 25, 25));

    shop->shop_items->push_back(this);

    this->is_enabled = true;

};

std::string ShopItem::get_cost_string()
{
    std::stringstream ss;
    ss << "(" << "$" << this->get_cost() << ")";
    return ss.str();
};

bool ShopItem::can_afford()
{
    return BEATUP->get_total_coins() >= this->get_cost();
};

void ShopItem::update(float dt)
{
    if (this->id_key == "resume_fight")
    {
        this->is_enabled = MainMenu::beatup_scene != NULL;
        if (!this->is_enabled)
        {
            button->setTouchEnabled(false);
            button->setPressedActionEnabled(false);
        }
        else
        {
            button->setTouchEnabled(true);
            button->setPressedActionEnabled(true);
        };
    };

    if (this->is_enabled)
    {
        if (this->can_afford())
        {
            if (this->button)
            {
                this->button->setTitleColor(Color3B::BLACK);
            };
        }
        else
        {
            if (this->button)
            {
                this->button->setTitleColor(Color3B::RED);
            };
        }
    }
    else //is this isn't enabled
    {
        if (this->button)
        {
            this->button->setTitleColor(Color3B::GRAY);
        };
    };

};

void BaseMenu::init_menu_from_data(cocos2d::ui::ScrollView* scroll, std::vector<ItemData> item_data)
{
    for (ItemData id : item_data)
    {
        PlainShopItem* si = new PlainShopItem(this, this->combo_menu, id.id_string);
        si->set_default_text(id.default_text);
        si->set_callback(id.callback);
        si->is_enabled = !id.start_disabled;
        auto c_btn = create_button(
            si->get_menu_text(),
            si->get_callback(this),
            id.start_disabled,
            id.state_changer
        );
        si->button = c_btn;
        scroll->addChild(c_btn);
    };
};

void BaseMenu::init_menu_from_priced_data(cocos2d::ui::ScrollView* scroll, std::vector<PricedItemData> item_data)
{

    for (PricedItemData pid : item_data)
    {
        PlainShopItem* si = new PlainShopItem(this, this->combo_menu, pid.id_string);
        si->set_default_text(pid.default_text);
        si->set_callback(pid.callback);
        si->_shop_cost = pid.cost;

        auto c_btn = create_button(
            si->get_menu_text(),
            si->get_callback(this),
            pid.start_disabled
        );

        si->button = c_btn;
        scroll->addChild(c_btn);
    };
};

PlainShopItem::PlainShopItem(BaseMenu* shop, cocos2d::Menu* menu, std::string id_key, ui::Button* button) : ShopItem(shop, menu, button, id_key)
{
    this->id_key = id_key;
    if (this->get_been_bought() &&
            //boosts are special cases where they are always enabled on first load
            (id_key != "left_fist_charge_boost" && id_key != "right_fist_charge_boost"
             && id_key != "stamina_recharge_boost_1" && id_key != "stamina_recharge_boost_2") &&
             !this->is_infinite_buy) 
    {
        this->is_enabled = false;
    }

    auto default_cb = [this]()
    {
        cocos2d::log("bought PlainShopItem: %s", this->get_default_text().c_str());
        return false;
    };
    this->set_callback(default_cb);
};

cocos2d::Size Scrollable::get_accumulated_size()
{
    return this->getLayoutAccumulatedSize();
};

void Scrollable::resize_to_fit()
{
    //TODO maintain scroll position after this gets called
    float scroll_w = sx(800);
    this->setInnerContainerSize(cocos2d::Size(
        scroll_w,
        this->get_accumulated_size().height
    ));
};
