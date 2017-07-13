#pragma once
#ifndef NUMENU_H
#define NUMENU_H

#include "Buyable.h"
#include "SubTypes.h"
#include "constants.h"

#include "ui/UIWidget.h"
#include "HouseBuilding.h"

class PlainShopItem;
class Fist;
class ShopItem;
class Building;
class BuildingShopNuItem;


namespace cocos2d
{
    class MotionStreak;
    class NodeGrid;
    class Sprite;

    struct _ttfConfig;
    typedef _ttfConfig TTFConfig;

    namespace ui
    {
        class TextBMFont;
        class Text;
        class Widget;
        class ImageView;
        class Button;
        class LoadingBar;
    }
}

class NuItem : public cocos2d::ui::Widget
{
    public:

        static NuItem* create(cocos2d::ui::Widget* parent);

        NuItem(){};
        static void init_orig_item();
        virtual bool init(cocos2d::Node* parent);

        static cocos2d::ui::Button* orig_button;
        static cocos2d::TTFConfig ttf_config;

        virtual inline std::string get_currency_prefix() { return "$"; };

        cocos2d::ui::Button* button;
        cocos2d::ui::ImageView* item_icon;
        cocos2d::ui::TextBMFont* title_lbl;
        cocos2d::ui::TextBMFont* desc_lbl;
        cocos2d::ui::TextBMFont* cost_lbl;
        cocos2d::ui::TextBMFont* count_lbl;
        cocos2d::ui::LoadingBar* progress_bar;

        //progress panel for achievements
        cocos2d::ui::TextBMFont* current_lbl;
        cocos2d::ui::TextBMFont* middle_lbl;
        cocos2d::ui::TextBMFont* total_lbl;

        std::string _original_image_path;

        void set_image(std::string path, Widget::TextureResType texture_type = Widget::TextureResType::PLIST);
        void set_original_image(std::string path);
        void set_title(std::string title);
        void set_description(std::string description);
        void set_cost_lbl(std::string cost);
        void set_count_lbl(res_count_t count);

        void try_set_enable(bool is_enable);

        void set_touch_ended_callback(std::function<void(void)> callback);

        //handle stuff like been bought, can be pressed etc
        spClock cooldown;
        virtual void update_func(float dt);

        //only the children not actually the panel
        void set_progress_panel_visible(bool visible);

        //tracks the constructable to show the progressbar
        void connect_to_constructable(spConstructable constructable);
};

class BuildingNuItem : public NuItem
{
    public:
        spBuilding building;

        static BuildingNuItem* create();
        static BuildingNuItem* create(cocos2d::ui::Widget* parent, std::shared_ptr<Building> building);
        virtual bool init(Node* parent, std::shared_ptr<Building> building);
};

class RecipeNuItem : public BuildingNuItem
{
    public:
        static RecipeNuItem* create(cocos2d::ui::Widget* parent, spBuilding building);

        spRecipe recipe;

        virtual void other_init(spRecipe recipe);
        virtual void update_func(float dt) override;
};

class TechNuItem : public BuildingNuItem
{
    public:
        static TechNuItem* create(cocos2d::ui::Widget* parent, spBuilding building);

        spTechnology technology;

        virtual void other_init(spTechnology technology);
        virtual void update_func(float dt) override;
};

class TargetRecipeNuItem : public BuildingNuItem
{
    public:
        static TargetRecipeNuItem* create(cocos2d::ui::Widget* parent, spBuilding building);

        spRecipe recipe;

        /// used to check if the requirements have been met, ie unlocked the Arena for weaken enemies crafting recipe
        BuildingTypes required_building_type;
        bool get_is_locked() const;

        virtual void other_init(spRecipe recipe);
        virtual void update_func(float dt) override;
};

class ShopNuItem : public Buyable, public NuItem
{
    public:
        static ShopNuItem* create(cocos2d::ui::Widget* parent);
        ShopNuItem() : Buyable("unset_in_shopnuitem") {};

        res_count_t _last_shop_cost;

        ///wraps BEATUP->get_total_coins
        virtual res_count_t get_available_coins();
        virtual void add_available_coins(res_count_t);

        virtual bool init(Node* parent, std::string id_key = "");
        virtual bool custom_status_check(float dt);
        virtual void update_func(float dt);
};

class BuildingShopNuItem : public ShopNuItem
{
    private:
        bool _been_bought;

    public:
        static BuildingShopNuItem* create(cocos2d::ui::Widget* parent, spBuilding building);;

        BuildingShopNuItem(){};

        std::shared_ptr<Building> building;
        virtual bool init(Node* parent, std::shared_ptr<Building> building);

        bool get_been_bought() override;
        void set_been_bought(bool val) override;
};

class UpgradeWalletNuItem : public BuildingShopNuItem
{
    public:
        static UpgradeWalletNuItem* create(cocos2d::ui::Widget* parent, spBuilding building);
        virtual res_count_t get_available_coins();
        virtual void add_available_coins(res_count_t);

        inline std::string get_currency_prefix() override { return "CI "; };
        virtual void other_init();
};


class UpgradeBuildingShopNuItem : public BuildingShopNuItem
{

    public:
        static UpgradeBuildingShopNuItem* create(cocos2d::ui::Widget* parent, spBuilding building);
        UpgradeBuildingShopNuItem(){};

        int building_level;
        virtual bool custom_status_check(float dt) override;


        virtual bool my_init(int building_level);
};

class HarvesterShopNuItem : public BuildingShopNuItem
{
    public:
        static HarvesterShopNuItem* create(cocos2d::ui::Widget* parent, spBuilding building);

        std::mt19937 get_generator();

        bool init(Node* parent, spBuilding building);
        virtual void my_init(WorkerSubType harv_type, IngredientSubType ing_type);
        virtual void my_init_title();
        virtual void my_init_sprite();
        virtual void my_init_touch_ended_callback();
        virtual void my_init_update_callback();

        WorkerSubType harv_type;
        IngredientSubType ing_type;
};

class SalesmanShopNuItem : public HarvesterShopNuItem
{
    public:
        static SalesmanShopNuItem* create(cocos2d::ui::Widget* parent, spBuilding building);

        virtual void my_init_title() override;
        virtual void my_init_sprite() override;
        void my_init_touch_ended_callback() override;
        void my_init_update_callback() override;

        virtual bool custom_status_check(float dt) override;
};

class ConsumerShopNuItem : public HarvesterShopNuItem
{
    public:
        static ConsumerShopNuItem* create(cocos2d::ui::Widget* parent, spBuilding building, IngredientSubType consumed_type);

        virtual void my_init_title() override;
        virtual void my_init_sprite() override;
        void my_init_touch_ended_callback() override;
        void my_init_update_callback() override;

        virtual bool custom_status_check(float dt) override;

        IngredientSubType consumed_type;
};

#endif
