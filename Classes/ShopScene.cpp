#include "ShopScene.h"
#include "Fist.h"
#include "Beatup.h"
#include "SoundEngine.h"
#include "Weapons.h"
#include "DataManager.h"
#include "Combo.h"
#include "Clock.h"
#include "Util.h"
#include "MainMenu.h"

USING_NS_CC;

bool ShopMenu::init()
{
#ifdef _WIN32
    FUNC_INIT_WIN32(ShopMenu);
#else
    FUNC_INIT(ShopMenu);
#endif

	this->beatup = NULL;

    ShopMenu::menu_font = DEFAULT_FONT;
    ShopMenu::menu_fontsize = 35;

    SoundEngine::play_music("music\\shop.mp3");

    this->shop_items = new std::vector<ShopItem*>();

    ShopMenu::menu_heightdiff = -1;
    this->last_pos = Vec2(-1, -1);

    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    // add a label shows "Hello World" create and initialize a label
    auto label = Label::createWithTTF("Welcome to the ShopMenu", ShopMenu::menu_font, sx(24));

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

void ShopMenu::update_coins_lbl()
{
    std::stringstream ss;
    ss << this->beatup->get_total_coins() << " coins";
    this->main_lbl->setString(ss.str());
};

void ShopMenu::buy_stuff(int cost)
{
    DataManager::decr_key(Beatup::total_coin_key, cost);
    cocos2d::log("spent %d", cost);
    this->update_coins_lbl();
    // SoundEngine::play_sound("sounds\\old\\coin.mp3");
    SoundEngine::play_sound("sounds\\new\\coin\\C_coin_1.mp3");
};

bool ShopMenu::can_afford(int cost)
{
    if (this->beatup->get_total_coins() >= cost)
    {
        return true;
    };
    return false;
};

bool ShopMenu::buy(ShopItem* shop_item, int cost, BoolFuncNoArgs on_bought)
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



void ShopMenu::menu_init()
{
    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();


    this->combo_menu = Menu::create();
    this->addChild(this->combo_menu);

    // auto combo_scroll = this->create_center_scrollview();
    auto buff_scroll = this->create_center_scrollview();
    // auto fist_scroll = this->create_center_scrollview();

    //combos
    // this->prep_combos(combo_scroll);
    //fist buffs
    this->prep_punch_items(buff_scroll);
    //fist weapons
    // this->prep_fist_weapons(fist_scroll);
    //charged fist
    // this->prep_charged_fist(fist_scroll);
    
    //tab switches
    this->prep_tabs(NULL, buff_scroll, NULL);

    //back button
    this->back_button = MenuItemLabel::create(Label::createWithTTF("Back", ShopMenu::menu_font, ShopMenu::menu_fontsize));
    this->combo_menu->addChild(this->back_button);
    Vec2 corner_pos = Vec2(
        origin.x + visibleSize.width - this->back_button->getContentSize().width/2 ,
        origin.y + this->back_button->getContentSize().height/2
    );

    this->back_button->setPosition(this->convertToNodeSpaceAR(corner_pos));
    this->back_button->setCallback(CC_CALLBACK_1(ShopMenu::pop_scene, this));

    //stat label
    // this->stat_lbl = Label::createWithTTF("total hits: ?", ShopMenu::menu_font, ShopMenu::menu_fontsize);
    // this->stat_lbl->setPosition(Vec2(corner_pos.x-sx(150), corner_pos.y+sy(500)));
    // this->addChild(this->stat_lbl);

    auto pos = this->combo_menu->getPosition();
    pos.y += sy(100);
    this->combo_menu->setPosition(pos);

	this->update_coins_lbl();

    // this->resize_scroll_inner(combo_scroll);
    this->resize_scroll_inner(buff_scroll);
    // this->resize_scroll_inner(fist_scroll);
};

void ShopMenu::prep_tabs(Scrollable* combo_scroll, Scrollable* buff_scroll, Scrollable* fist_scroll)
{

    //float xdiff = sx(300);
    float ydiff = sy(75);

    //MenuItemLabel* combo_mil;
    MenuItemLabel* buff_mil;
    //MenuItemLabel* fist_mil;

    // ccMenuCallback combo_cb = [combo_scroll, buff_scroll, fist_scroll](Ref*) {
    //     combo_scroll->setVisible(true);
    //     buff_scroll->setVisible(false);
    //     fist_scroll->setVisible(false);
    // };

     ccMenuCallback buff_cb = [combo_scroll, buff_scroll, fist_scroll](Ref*) {
         combo_scroll->setVisible(false);
         buff_scroll->setVisible(true);
         fist_scroll->setVisible(false);
     };

    //ccMenuCallback fist_cb = [combo_scroll, buff_scroll, fist_scroll](Ref*) {
    //    combo_scroll->setVisible(false);
    //    buff_scroll->setVisible(false);
    //    fist_scroll->setVisible(true);
    //};


    auto update_with_count = [](MenuItemLabel* lbl, Scrollable* scrollable) {
        std::string old_str = ((Label*)lbl->getLabel())->getString();
        std::stringstream ss;
        ss << old_str << "(" << scrollable->getChildrenCount() << ")";
        ((Label*)lbl->getLabel())->setString(ss.str().c_str());
    };

    //show only the combo scrollable
    // combo_scroll->setVisible(true);
    // combo_mil = MenuItemLabel::create(Label::createWithTTF("COMBOS", ShopMenu::menu_font, ShopMenu::menu_fontsize));
    // this->combo_menu->addChild(combo_mil);
    // combo_mil->setPosition(this->convertToNodeSpaceAR(Vec2(
    //     this->get_center_pos().x - xdiff,
    //     this->get_center_pos().y + ydiff
    // )));
    // combo_mil->setCallback(combo_cb);
    // update_with_count(combo_mil, combo_scroll);

    buff_scroll->setVisible(true);
    buff_mil = MenuItemLabel::create(Label::createWithTTF("BOOSTS", ShopMenu::menu_font, ShopMenu::menu_fontsize));
    this->combo_menu->addChild(buff_mil);
    buff_mil->setPosition(this->convertToNodeSpaceAR(Vec2(
       this->get_center_pos().x,
       this->get_center_pos().y + ydiff
    )));
    buff_mil->setCallback(buff_cb);
    //update_with_count(buff_mil, buff_scroll);

    // fist_scroll->setVisible(false);
    // fist_mil = MenuItemLabel::create(Label::createWithTTF("UNLOCKS", ShopMenu::menu_font, ShopMenu::menu_fontsize));
    // this->combo_menu->addChild(fist_mil);
    // fist_mil->setPosition(this->convertToNodeSpaceAR(Vec2(
    //     this->get_center_pos().x + xdiff,
    //     this->get_center_pos().y + ydiff
    // )));
    // fist_mil->setCallback(fist_cb);
    //update_with_count(fist_mil, fist_scroll);

};

void ShopMenu::prep_combos(cocos2d::ui::ScrollView* scroll)
{
    for (Combo* combo : *this->beatup->combos)
    {
        ComboShopItem* csi = new ComboShopItem(this, this->combo_menu, combo, NULL);
        auto c_btn = create_button(
                csi->get_menu_text(),
                csi->get_callback(this),
                combo->get_been_bought()
            );
        scroll->addChild(c_btn);
        csi->button = c_btn;
    };
};

void ShopMenu::prep_punch_items(cocos2d::ui::ScrollView* scroll)
{

    std::function<BoolFuncNoArgs(int)> get_fist_cb = [this](int cost)
    {
        //runs when the button is clicked
        BoolFuncNoArgs on_click = [this, cost]()
        {
            //build id_string
            std::stringstream id_string;
            id_string << "sell_grain";

            printj1("clicked!");

            //runs after the button is clicked, and the item has been bought
            BoolFuncNoArgs on_bought_cb = [&id_string, this, cost]()
            {
                printj1("bought!");
                //fist->charging_clock->set_threshold(
                //    fist->defaults.charge_threshold/2
                //);

                //set the charged boost as sold
                //DataManager::set_bool_from_data(id_string.str(), true);
                return false;
            };

            //use id_string to find the shop item
            PlainShopItem* psi = this->get_psi_by_id(id_string.str());
            printj1("MADE INFINITE");
            psi->is_infinite_buy = true;
            return this->buy(psi, cost, on_bought_cb);
        };

        return on_click;
    };

    std::vector<PricedItemData> item_data = {
        { "sell_grain", "Sell one piece of grain", get_fist_cb(-1), false, -1},
    };

    this->init_menu_from_priced_data(scroll, item_data);
};

PlainShopItem* ShopMenu::get_psi_by_id(std::string id_key)
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

void ShopMenu::prep_fist_weapons(cocos2d::ui::ScrollView* scroll)
{
    std::function<BoolFuncNoArgs(FistWeapon*)> get_fistweapon_cb = [this](FistWeapon* fist_weapon)
    {
        //runs when the button is clicked
        BoolFuncNoArgs on_click = [this, fist_weapon]()
        {
            //runs after the button is clicked, and the item has been bough
            BoolFuncNoArgs on_bought_cb = [this, fist_weapon]()
            {
                FistWeaponTypes type = fist_weapon->type;
                fist_weapon->set_been_bought(true);

                this->beatup->set_visible_weapon_button(type, true);

                return true;
            };

            PlainShopItem* psi = this->get_psi_by_id(fist_weapon->id_key);
            return this->buy(psi, fist_weapon->shop_cost, on_bought_cb);
        };
        return on_click;
    };

    //fist weapons
    FistWeapon* flame_fist = this->beatup->fist_flame;
    FistWeapon* psionic_fist = this->beatup->fist_psionic;
    FistWeapon* frost_fist = this->beatup->fist_frost;

    std::vector<PricedItemData> item_data = {
        { flame_fist->id_key, flame_fist->name, get_fistweapon_cb(flame_fist), flame_fist->get_been_bought(), flame_fist->shop_cost},
        { psionic_fist->id_key, psionic_fist->name, get_fistweapon_cb(psionic_fist), psionic_fist->get_been_bought(), psionic_fist->shop_cost},
        { frost_fist->id_key, frost_fist->name, get_fistweapon_cb(frost_fist), frost_fist->get_been_bought(), frost_fist->shop_cost}
    };

    this->init_menu_from_priced_data(scroll, item_data);
};

void ShopMenu::prep_charged_fist(cocos2d::ui::ScrollView* scroll)
{
    std::function<BoolFuncNoArgs(std::string)> get_charge_cb = [this](std::string id_key)
    {
        //runs when the button is clicked
        BoolFuncNoArgs on_click = [this, id_key]()
        {
            //runs after the button is clicked, and the item has been bought
            BoolFuncNoArgs on_bought_cb = [id_key]()
            {
                DataManager::set_bool_from_data(id_key, true);
                return true;
            };

            PlainShopItem* psi = this->get_psi_by_id(id_key);
            return this->buy(psi, 15, on_bought_cb);
        };
        return on_click;
    };

    std::string charge_id_key = "charging_enabled"; 
    std::vector<PricedItemData> item_data = {
        { charge_id_key, "Chargeable Punches", get_charge_cb(charge_id_key), DataManager::get_bool_from_data(charge_id_key, false), 15},
    };

    this->init_menu_from_priced_data(scroll, item_data);
};

void ShopMenu::shop_menu_update(float dt)
{
    for (ShopItem* shop_item : *this->shop_items)
    {
        shop_item->update(dt);
    };
};

void ShopMenu::update(float dt)
{
    this->shop_menu_update(dt);
};

void ShopMenu::onKeyReleased(EventKeyboard::KeyCode keyCode, Event *pEvent)
{
    if(keyCode == EventKeyboard::KeyCode::KEY_BACK || keyCode == EventKeyboard::KeyCode::KEY_ESCAPE) 
    {
        pop_scene(NULL);
    }
}

void ShopMenu::pop_scene(Ref* pSender)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WP8) || (CC_TARGET_PLATFORM == CC_PLATFORM_WINRT)
    MessageBox("You pressed the close button. Windows Store Apps do not implement a close button.", "Alert");
    return;
#endif

    this->beatup->update(0);
    Director::getInstance()->pushScene(this->beatup_scene);

}

ShopItem::ShopItem(ShopMenu* shop, Menu* menu, ui::Button* button) : button(button)
{
    this->beatup = shop->beatup;

    this->id_key = "unset_id_key";

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
    return this->beatup->get_total_coins() >= this->get_cost();
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

void ShopMenu::init_menu_from_data(cocos2d::ui::ScrollView* scroll, std::vector<ItemData> item_data)
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

void ShopMenu::init_menu_from_priced_data(cocos2d::ui::ScrollView* scroll, std::vector<PricedItemData> item_data)
{

    for (PricedItemData pid : item_data)
    {
        PlainShopItem* si = new PlainShopItem(this, this->combo_menu, pid.id_string);
        si->set_default_text(pid.default_text);
        si->set_callback(pid.callback);
        si->cost = pid.cost;

        auto c_btn = create_button(
            si->get_menu_text(),
            si->get_callback(this),
            pid.start_disabled
        );

        si->button = c_btn;
        scroll->addChild(c_btn);
    };
};

void ShopMenu::resize_scroll_inner(Scrollable* scroll)
{
    float extra_margins = scroll->getChildrenCount()*(sy(15.0f)*sy(1.0f));
    int scroll_w = sx(800);
    scroll->setInnerContainerSize(Size(
        scroll_w,
        scroll->get_accumulated_size().height + extra_margins
    ));
}


ComboShopItem::ComboShopItem(ShopMenu* shop, Menu* menu, Combo* combo, ui::Button* button) : ShopItem(shop, menu, button)
{
    this->combo = combo;

    if (this->combo->get_been_bought())
    {
        this->is_enabled = false;
    }
};

std::string ComboShopItem::get_menu_text()
{
    if (this->combo != NULL)
    {
        std::stringstream ss;
        ss << combo->name << " " << this->get_cost_string();
        return ss.str();
    }
    else
    {
        return "No combo matched";
    };
};

BoolFuncNoArgs ComboShopItem::get_callback(ShopMenu* shop)
{
    BoolFuncNoArgs callback = [shop, this]()
    {
        auto on_buy_cb = [this](){
            this->combo->set_been_bought(true);
            return true;
        };
        shop->buy(this, combo->shop_cost, on_buy_cb);
        return false;
    };

    return callback;
};

int ComboShopItem::get_cost()
{
    return this->combo->shop_cost;
};

int PunchDamageShopItem::get_cost()
{
    return 70;
};

std::string LeftPunchDamageShopItem::get_menu_text()
{
    return "Left Punch Damage+ "+this->get_cost_string();
};

std::string RightPunchDamageShopItem::get_menu_text()
{
    return "Right Punch Damage+ "+this->get_cost_string();
};

Fist* LeftPunchDamageShopItem::get_fist()
{
    return this->beatup->left_fist;
};

Fist* RightPunchDamageShopItem::get_fist()
{
    return this->beatup->right_fist;
};

BoolFuncNoArgs PunchDamageShopItem::get_callback(ShopMenu* shop)
{
    BoolFuncNoArgs cb = [this, shop]()
    {
        std::function<bool()> qwe = [this]()
        {
            this->get_fist()->punch_dmg += 1.5;
            return true;
        };
        shop->buy(this, this->get_cost(), qwe);

        return false;
    };
    return cb;
};


PlainShopItem::PlainShopItem(ShopMenu* shop, cocos2d::Menu* menu, std::string id_key, ui::Button* button) : ShopItem(shop, menu, button)
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

void PlainShopItem::set_been_bought(bool val)
{
    DataManager::set_bool_from_data(this->id_key, val);
    if (val && !this->is_infinite_buy)
    {
        this->is_enabled = false;
    };
};

bool PlainShopItem::get_been_bought()
{
    return DataManager::get_bool_from_data(this->id_key, false);
};

int PlainShopItem::get_cost()
{
    return this->cost;
};

cocos2d::Size Scrollable::get_accumulated_size()
{
    return this->getLayoutAccumulatedSize();
};

Scrollable* Scrollable::create()
{

    Scrollable* widget = new (std::nothrow) Scrollable();
    if (widget && widget->init())
    {
        widget->autorelease();
        return widget;
    }
    CC_SAFE_DELETE(widget);
    return nullptr;

};
