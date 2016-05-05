#ifndef SHOPSCENE_H
#define SHOPSCENE_H
#include "cocos2d.h"
#include "GameLayer.h"
#include "Buyable.h"

class PlainShopItem;
class Fist;
class ShopItem;
class Beatup;
class Scrollable;

struct ItemData {
    std::string id_string;
    std::string default_text;
    BoolFuncNoArgs callback;
    bool start_disabled;
    GameLayer::ButtonState* state_changer;

    ItemData(
        std::string id_string,
        std::string default_text,
        BoolFuncNoArgs callback,
        bool start_disabled,
        GameLayer::ButtonState* state_changer = NULL
    ) : id_string(id_string), default_text(default_text), callback(callback), start_disabled(start_disabled), state_changer(state_changer) {};
};

struct PricedItemData : public ItemData {
    long double cost;
    PricedItemData(
        std::string id_string, 
        std::string default_text,
        BoolFuncNoArgs callback,
        bool start_disabled,
        long double cost
    ) : ItemData(id_string, default_text, callback, start_disabled) , cost(cost){};
};

class BaseMenu : public GameLayer
{
    public:
        cocos2d::Vec2 last_pos;

        bool buy(ShopItem* shop_item, long double cost, BoolFuncNoArgs on_bought);
        void buy_stuff(long double cost);
        bool can_afford(long double cost);

        void menu_init();
        void init_menu_from_data(cocos2d::ui::ScrollView* scroll, std::vector<ItemData> item_data);
        void init_menu_from_priced_data(cocos2d::ui::ScrollView* scroll, std::vector<PricedItemData> item_data);

        virtual void onKeyReleased(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event *pEvent) override;
        virtual void pop_scene(Ref* pSender);

        virtual bool init() override;
        void prep_tabs(Scrollable* combo_scroll, Scrollable* buff_scroll, Scrollable* fist_scroll);

        PlainShopItem* get_psi_by_id(std::string id_key);

        virtual void update(float dt) override;
        void shop_menu_update(float dt);

        CREATE_FUNC(BaseMenu);

        cocos2d::Label* main_lbl;
        void set_main_lbl();

        cocos2d::Scene* beatup_scene;

        std::vector<ShopItem*>* shop_items;
        cocos2d::Menu* combo_menu;

        cocos2d::MenuItemLabel* back_button;
        cocos2d::Label* stat_lbl;
};

class ShopItem :  public Buyable
{
    private:
        std::string _default_text = "Default Menu Text";
        BoolFuncNoArgs _callback;

    public:
        bool is_enabled = true;
        bool is_infinite_buy = true;

        ShopItem(BaseMenu* shop, cocos2d::Menu* menu, cocos2d::ui::Button* button, std::string id_key);

        cocos2d::ui::Button* button;

        virtual BoolFuncNoArgs  get_callback(BaseMenu* shop)
        {
	        return this->_callback;
        };
        void set_callback(BoolFuncNoArgs callback) 
        {
            this->_callback=callback; 
            //this->menu_lbl->setCallback(this->get_callback(shop));
        };

        std::shared_ptr<cocos2d::Color3B> afford_color;
        std::shared_ptr<cocos2d::Color3B> cant_afford_color;
        std::shared_ptr<cocos2d::Color3B> disabled_color;

        virtual void update(float dt);
        virtual bool can_afford();

        virtual std::string get_cost_string();

        virtual std::string get_menu_text()
        {
            std::string text = this->get_default_text();

            if (this->get_cost() != 0)
            {
                text = text + " " + this->get_cost_string();
            }

            return text;
        };

        virtual void set_default_text(std::string text) { _default_text = text; };
        virtual std::string get_default_text() { return _default_text; };

};

class PlainShopItem : public ShopItem
{
    public:
        PlainShopItem(BaseMenu* shop, cocos2d::Menu* menu, std::string id_key, cocos2d::ui::Button* button = NULL);
};

class Scrollable : public cocos2d::ui::ScrollView
{
    public:
    cocos2d::Size get_accumulated_size();
    CREATE_FUNC(Scrollable);

    void resize_to_fit();
};

#endif
