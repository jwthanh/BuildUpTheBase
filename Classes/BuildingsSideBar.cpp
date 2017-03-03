#include "BuildingsSideBar.h"

#include <memory>
#include <sstream>
#include <string>

#include "constants.h"

#include "MiscUI.h"
#include "Worker.h"
#include "NuMenu.h"
#include "GameLogic.h"
#include "HouseBuilding.h"
#include "StaticData.h"
#include "Recipe.h"
#include "Fighter.h"
#include "attribute_container.h"
#include "attribute.h"
#include "Util.h"
#include "Technology.h"
#include "Beatup.h"
#include "Clock.h"
#include "Modal.h"
#include "Logging.h"
#include "DataManager.h"
#include "HarvestScene.h"
#include "Harvestable.h"
#include "HarvestableManager.h"

#include "base/CCDirector.h"
#include "CCApplication.h"
#include "ui/UIButton.h"
#include "ui/UIListView.h"
#include "ui/UIText.h"
#include "ui/CocosGUI.h"
#include <2d/CCActionInterval.h>
#include <2d/CCActionEase.h>
#include "Tutorial.h"
#include "utilities/vibration.h"
#include "goals/Achievement.h"


USING_NS_CC;

std::map<Worker::SubType, Worker::SubType> req_map = {
    { Worker::SubType::ZERO, Worker::SubType::ZERO },
    { Worker::SubType::One, Worker::SubType::ZERO },
    { Worker::SubType::Two, Worker::SubType::One },
    { Worker::SubType::Three, Worker::SubType::Two },
    { Worker::SubType::Four, Worker::SubType::Three },
    { Worker::SubType::Five, Worker::SubType::Four },
    { Worker::SubType::Six, Worker::SubType::Five },
    { Worker::SubType::Seven, Worker::SubType::Six },
    { Worker::SubType::Eight, Worker::SubType::Seven },
    { Worker::SubType::Nine, Worker::SubType::Eight },
    { Worker::SubType::Ten, Worker::SubType::Nine },
    { Worker::SubType::Eleven, Worker::SubType::Ten },
    { Worker::SubType::Twelve, Worker::SubType::Eleven }
};

TabManager::TabManager()
{
    this->active_building = BUILDUP->city->building_by_name("The Farm");
    this->active_tab = TabTypes::ShopTab;
}

bool TabManager::is_tab_active(const TabTypes& tab_type, const std::shared_ptr<Building>& building) const
{
    return this->active_tab == tab_type && this->active_building == building;
}

void TabManager::set_tab_active(TabTypes tab_type, const spBuilding& building)
{
    this->active_tab = tab_type;
    spBuilding old_building = this->active_building;
    this->active_building = building;

    return; //disable sidebar scrolling for nausea reasons

    float distance = 400.0f;
    auto move_by = MoveBy::create(0.75f, { 0, -distance });
    auto eased_action = EaseBackOut::create(move_by);

    //move the given listview upwards, then downwards by the same amount
    auto listview = this->get_active_listview();
    listview->setPosition({listview->getPositionX(), listview->getPositionY() + distance });
    listview->runAction(eased_action);

    //move all the tab buttons
    if (building != old_building)
    {
        for (auto& btn : this->button_map)
        {
            btn.first->setPosition({ btn.first->getPositionX(), btn.first->getPositionY() + distance });
            btn.first->runAction(eased_action->clone());
        }
    }
}

cocos2d::ui::ListView* TabManager::get_active_listview()
{
    for (auto pair : this->button_map)
    {
        TabTypes tab_type = pair.second.first;
        spListviewMap listview_map = pair.second.second;
        if (tab_type == this->active_tab)
        {
            return listview_map->at(this->active_building->name);
        }
    }
    return NULL;
}

cocos2d::ui::Button* TabManager::get_active_button()
{
    for (auto pair : this->button_map)
    {
        ui::Button* button = pair.first;
        TabTypes tab_type = pair.second.first;
        if (tab_type == this->active_tab)
        {
            return button;
        }
    }
    return NULL;
}

SideListView::SideListView(Node* parent, spBuilding current_target)
    : current_target(current_target), parent(parent)
{
    this->tabs = TabManager();

    this->setup_listviews();
    this->setup_tab_buttons();

    Tutorial* tutorial = Tutorial::getInstance();
    Node* sidebar_panel = parent->getChildByName("sidebar_panel");
    auto check_visible = [sidebar_panel, tutorial](float dt){
        try_set_visible(sidebar_panel, tutorial->get_show_sidebar());
    };

    parent->schedule(check_visible, SHORT_DELAY, "check_visible");
    check_visible(0.0f);


    this->tabs.button_map = std::map<ui::Button*, std::pair<TabManager::TabTypes, spListviewMap>>{
        { this->tab_shop_btn, { TabManager::TabTypes::ShopTab, this->shop_listviews } },
        { this->tab_detail_btn, { TabManager::TabTypes::DetailTab, this->detail_listviews } },
        { this->tab_building_btn, { TabManager::TabTypes::BuildingTab, this->building_listviews } },
        { this->tab_powers_btn, { TabManager::TabTypes::PowersTab, this->powers_listviews } }
    };

    //prepress the shop button
    this->toggle_buttons(this->tab_shop_btn, ui::Widget::TouchEventType::ENDED);
};

ui::Button* SideListView::_create_button(std::string node_name)
{
    Node* sidebar_panel = this->parent->getChildByName("sidebar_panel");

    ui::Button* button = dynamic_cast<ui::Button*>(sidebar_panel->getChildByName(node_name));
    set_aliasing(button);
    load_default_button_textures(button);

    Label* button_lbl = (Label*)button->getTitleRenderer();
    button_lbl->setTextColor(Color4B::WHITE);
    button_lbl->enableOutline(Color4B::BLACK, 2);
    button_lbl->getFontAtlas()->setAliasTexParameters();


    return button;
};

void SideListView::toggle_buttons(Ref* target, ui::Widget::TouchEventType evt)
{
    if (evt == ui::Widget::TouchEventType::ENDED) {

        this->tab_shop_btn->setEnabled(true);
        this->tab_detail_btn->setEnabled(true);
        this->tab_building_btn->setEnabled(true);
        this->tab_powers_btn->setEnabled(true);

        ui::Button* target_button = dynamic_cast<ui::Button*>(target);
        target_button->setEnabled(false);

        spBuilding target_building = BUILDUP->get_target_building();

        auto activate_tab = [this, target_building](spListviewMap& listviews, TabManager::TabTypes tab_type)
        {
            ui::ListView* listview = listviews->at(target_building->name);
            listview->requestDoLayout();
            this->tabs.set_tab_active(tab_type, target_building);

            std::map<std::string, std::string> name_map = {
                {"The Farm", "Cook"},
                {"The Arena", "Upgrade"},
                {"The Underscape", "Blood Magic"},
                {"The Marketplace", "Barter"},

                {"The Dump", "Scavenge"},
                {"The Workshop", "Craft"},
                {"The Mine", "Tools"},
                {"The Graveyard", "Necromancy"},
                {"The Forest", "Explore"}
            };

            std::string default_str = "Detail";
            std::string detail_str = map_get(name_map, target_building->name, default_str);
            this->tab_detail_btn->setTitleText(detail_str);


            //jump to active building for this tab
            // FIXME because buttons aren't there the first time due to
            // update function building the items, this doesn't work the first
            // time it happens, it requires a built tab
            if (tab_type == TabManager::TabTypes::BuildingTab)
            {
                int sentinel = 123456;
                int index = sentinel;

                auto building_upgrade_sidebar = this->building_listviews->at(target_building->name);
                for (Node* item : building_upgrade_sidebar->getChildren())
                {
                    auto nuitem = dynamic_cast<ui::Button*>(item);
                    if (nuitem)
                    {
                        //assume enabled means buyable
                        if (nuitem->isEnabled())
                        {
                            index = building_upgrade_sidebar->getIndex(nuitem);
                        }
                    }
                }
                if (index != sentinel)
                {
                    //dont bounce, but dont reenable bounce if it wasn't in the first place
                    bool orig_bounce = building_upgrade_sidebar->isBounceEnabled();
                    building_upgrade_sidebar->setBounceEnabled(false);

                    building_upgrade_sidebar->jumpToItem(index, Vec2::ONE, Vec2::ONE);

                    building_upgrade_sidebar->setBounceEnabled(orig_bounce);
                }
            }
        };

        auto result = this->tabs.button_map[target_button];
        activate_tab(result.second, result.first);

        do_vibrate(10);

    };
};


void SideListView::setup_tab_buttons()
{
    this->tab_shop_btn = this->_create_button("tab_1_btn");
    bind_touch_ended(this->tab_shop_btn, [this](){ this->toggle_buttons(this->tab_shop_btn, ui::Widget::TouchEventType::ENDED); });

    this->tab_detail_btn = this->_create_button("tab_2_btn");
    bind_touch_ended(this->tab_detail_btn, [this](){ this->toggle_buttons(this->tab_detail_btn, ui::Widget::TouchEventType::ENDED); });

    this->tab_building_btn = this->_create_button("tab_3_btn");
    bind_touch_ended(this->tab_building_btn, [this](){ this->toggle_buttons(this->tab_building_btn, ui::Widget::TouchEventType::ENDED); });

    this->tab_powers_btn = this->_create_button("tab_4_btn");
    bind_touch_ended(this->tab_powers_btn, [this](){ this->toggle_buttons(this->tab_powers_btn, ui::Widget::TouchEventType::ENDED); });

}

void SideListView::setup_listviews()
{
	CCLOG("start creating listviews");
    this->shop_listviews = this->_create_listview("shop_listview");
    this->detail_listviews = this->_create_listview("detail_listview");
    this->building_listviews = this->_create_listview("building_listview");
    this->powers_listviews = this->_create_listview("powers_listview");
	CCLOG("done creating listviews");

    auto clean_children_on_target_change = [this](float dt)
    {
        spBuilding target_building = BUILDUP->get_target_building();
        if (target_building->name != this->current_target->name)
        {
            this->current_target = target_building;
            toggle_buttons(this->tabs.get_active_button(), ui::Widget::TouchEventType::ENDED);

        }
    };
    this->parent->schedule(clean_children_on_target_change, AVERAGE_DELAY, "clean_children");
};

void SideListView::setup_shop_listview_as_harvesters()
{
    TabManager::TabTypes tab_type = TabManager::TabTypes::ShopTab;
    for (spBuilding building : BUILDUP->city->buildings)
    {
        ui::ListView* shop_listview = this->shop_listviews->at(building->name);
        auto update_harvester_listview = [this, shop_listview, building, tab_type](float dt)
        {
            if (this->tabs.is_tab_active(tab_type, building) == false)
            {
                try_set_visible(shop_listview, false);
                return;
            }

            try_set_visible(shop_listview, true);
            enum class WorkerType
            {
                Harvester,
                Salesman,
                Consumer
            };

            //placeholder for things we'll need to put in the sidebar
            struct HarvestConfig{
                WorkerType worker_type;
                std::string node_name;
                Worker::SubType harv_type;
            };
            std::vector<HarvestConfig> nuitems_config = {
                { WorkerType::Salesman, "salesman_item_one", Worker::SubType::One },
                // {WorkerType::Salesman, "salesman_item_two", Worker::SubType::Two}, //TODO fix sorting before we can add this in
                // {WorkerType::Salesman, "salesman_item_three", Worker::SubType::Three},
                { WorkerType::Harvester, "harvester_item_one", Worker::SubType::One },
                { WorkerType::Harvester, "harvester_item_two", Worker::SubType::Two },
                { WorkerType::Harvester, "harvester_item_three", Worker::SubType::Three },
                { WorkerType::Harvester, "harvester_item_four", Worker::SubType::Four },
                { WorkerType::Harvester, "harvester_item_five", Worker::SubType::Five },
                { WorkerType::Harvester, "harvester_item_six", Worker::SubType::Six },
                { WorkerType::Harvester, "harvester_item_seven", Worker::SubType::Seven },
                { WorkerType::Harvester, "harvester_item_eight", Worker::SubType::Eight },
                { WorkerType::Harvester, "harvester_item_nine", Worker::SubType::Nine },
                { WorkerType::Harvester, "harvester_item_ten", Worker::SubType::Ten },
                { WorkerType::Harvester, "harvester_item_eleven", Worker::SubType::Eleven }
            };

            if (building->name == "The Underscape"){
                nuitems_config.insert(nuitems_config.begin() + 1, { WorkerType::Consumer, "consumer_item_one", Worker::SubType::One });
            };

            int i = 0;
            for (auto config : nuitems_config)
            {
                //if the child already exists, put it at the back
                int tag = i++;

                auto existing_node = if_tag_exists_in_listview(tag, shop_listview);
                if (existing_node) { continue; }

                //check for prereqs before adding the next item to the menu
                if (config.harv_type != Worker::SubType::One) {
                    Worker::SubType ZERO = Worker::SubType::ZERO;
                    std::pair<Worker::SubType, Ingredient::SubType> key = {
                        map_get(req_map, config.harv_type, ZERO),
                        building->punched_sub_type
                    };

                    res_count_t _def = 0;
                    auto prereq_harvester_found = map_get(building->harvesters, key, _def);

                    //if cant meet prereq, dont try to add the item
                    if (prereq_harvester_found < 5) {
                        continue;
                    }
                    else {
                        //resize scroll bar on new additions to the listview
                        shop_listview->requestDoLayout();

                        //shop_listview->jumpToBottom(); //seems like this adds the next item to the wrong spot
                    };
                };

                //clone the new item
                HarvesterShopNuItem* menu_item;
                if (config.worker_type == WorkerType::Harvester){
                    menu_item = HarvesterShopNuItem::create(shop_listview, building);
                }
                else if (config.worker_type == WorkerType::Salesman)
                {
                    menu_item = SalesmanShopNuItem::create(shop_listview, building);
                }
                else if (config.worker_type == WorkerType::Consumer)
                {
                    menu_item = ConsumerShopNuItem::create(shop_listview, building, Ingredient::SubType::Blood);
                }
                menu_item->my_init(config.harv_type, building->punched_sub_type);
                std::string child_name = config.node_name;
                menu_item->setName(child_name);
                menu_item->setTag(tag);

                //since we only set the ing_type of the menu item above, it doesn't
                //change to adapt for the building, so we cheat and do it here.
                //this'll get moved to a json map or something between building and
                //harvest sub types
                auto update_target_and_prereq = [this, menu_item, building, tab_type](float dt){
                    if (this->tabs.is_tab_active(tab_type, building) == false)
                    {
                        return;
                    }
                    menu_item->ing_type = building->punched_sub_type;

                    if (menu_item->harv_type != Worker::SubType::One) {
                        Worker::SubType ZERO = Worker::SubType::ZERO;
                        std::pair<Worker::SubType, Ingredient::SubType> key = {
                            map_get(req_map, menu_item->harv_type, ZERO),
                            menu_item->ing_type
                        };

                        res_count_t _def = 0;
                        res_count_t prereq_harvester_found = map_get(building->harvesters, key, _def);

                        if (prereq_harvester_found < 5) {
                            try_set_visible(menu_item->button, false);
                        }
                        else {
                            try_set_visible(menu_item->button, true);
                        }

                    };
                };

                menu_item->schedule(update_target_and_prereq, AVERAGE_DELAY, "update_ing_type");
                update_target_and_prereq(0);

            };

        };

        shop_listview->schedule(update_harvester_listview, AVERAGE_DELAY, "update_listview");
        update_harvester_listview(0);
    };
};

///tries to push the node at child_tag to the back of the listview. if it didn't exist,
/// it returns false
bool SideListView::if_tag_exists_in_listview(int child_tag, ui::ListView* listview)
{
    auto existing_node = listview->getChildByTag(child_tag);

    return existing_node != NULL;
}

void SideListView::setup_building_listview_as_upgrades()
{
    TabManager::TabTypes tab_type = TabManager::TabTypes::BuildingTab;

    for (spBuilding building : BUILDUP->city->buildings)
    {
        ui::ListView* listview = this->building_listviews->at(building->name);

        auto update_listview = [this, listview, building, tab_type](float dt)
        {
            if (this->tabs.is_tab_active(tab_type, building) == false)
            {
                try_set_visible(listview, false);
                return;
            }
            try_set_visible(listview, true);

            int i = 0;
            int max_level = 15;
            for (int level = 2; level <= max_level; level++)
            {
                int child_tag = i;
                i++;


                //if the child already exists, put it at the end of the listview, maintaining order as config
                bool existed = this->if_tag_exists_in_listview(child_tag, listview);
                if (existed) { continue; };

                UpgradeBuildingShopNuItem* menu_item = UpgradeBuildingShopNuItem::create(listview, building);
                menu_item->my_init(level);

                menu_item->setTag(child_tag);

            };
        };

        listview->schedule(update_listview, AVERAGE_DELAY, "update_listview");
        update_listview(0);
    };
};

void SideListView::setup_detail_listview_as_recipes()
{
    TabManager::TabTypes tab_type = TabManager::TabTypes::DetailTab;


    for (spBuilding building : BUILDUP->city->buildings)
    {
        ui::ListView* listview = this->detail_listviews->at(building->name);

        ///DETAIL LISTVIEW
        auto update_listview = [this, listview, building, tab_type](float dt)
        {
            if (this->tabs.is_tab_active(tab_type, building) == false)
            {
                try_set_visible(listview, false);
                return;
            }

            try_set_visible(listview, true);

            struct MenuItemConfig {
                std::string name;
                std::string description;
                std::string local_img_path;
            };

            enum class DetailType
            {
                Recipe = 0,
                Misc = 1,
                Tech = 2,
                ChangeTargetRecipe = 3,
                UpgradeWalletCapTech = 4
            };

            struct DetailConfig {
                std::shared_ptr<void> object;
                DetailType type;
                MenuItemConfig config;
            };

            std::vector<DetailConfig> nuitems_config;

            for (spRecipe recipe : building->data->get_all_recipes())
            {
                nuitems_config.push_back({
                    recipe,
                    DetailType::Recipe,
                    {
                        recipe->name,
                        recipe->description,
                        ""
                    }
                    });
            };

            if (building->name == "The Underscape")
            {
                spRecipe blood_oath = std::make_shared<Recipe>("Bloodoath", "Gain 5 health\n-- costs 10 blood");
                blood_oath->components = mistIngredient({
                    { Ingredient::SubType::Blood, 10 }
                });
                blood_oath->_callback = [building]()
                {
                    auto health = BUILDUP->fighter->attrs->health;
                    if (health->is_full() == false)
                    {
                        res_count_t healing = 5;
                        health->add_to_current_val(healing);
                        auto achievement_manager = AchievementManager::getInstance();
                        std::shared_ptr<BaseAchievement> achievement = achievement_manager->getAchievement(AchievementType::TotalHealthRegenerated);
                        achievement->increment_by_n(healing);
                    }
                    else
                    {
                        //refund the cost
                        building->create_ingredients(Ingredient::SubType::Blood, 10);
                    };
                };
                nuitems_config.push_back({
                    blood_oath,
                    DetailType::Recipe,
                    {
                        blood_oath->name,
                        blood_oath->description,
                        "menu_icons/bloodoath.png"
                    }});
            };
            if (building->name == "The Graveyard")
            {
                spRecipe blood_oath = std::make_shared<Recipe>("Raise Dead", "Scavenges at The Dump\n-- 1 soul, 6 flesh, 9 blood");
                blood_oath->components = mistIngredient({
                    { Ingredient::SubType::Soul, 1 },
                    { Ingredient::SubType::Flesh, 6 },
                    { Ingredient::SubType::Blood, 9 }
                });
                blood_oath->_callback = [building](){
                    bool can_fit_more_dead = true;
                    if (can_fit_more_dead)
                    {
                        res_count_t raise_count = 1;
                        building->create_ingredients(Ingredient::SubType::Undead, raise_count);

                        cocos2d::Scene* scene = cocos2d::Director::getInstance()->getRunningScene();
                        auto harvest_scene = dynamic_cast<HarvestScene*>(scene->getChildByName("HarvestScene"));
                        if (harvest_scene)
                        {
                            harvest_scene->show_ingredient_listview_for_ing_type(Ingredient::SubType::Iron);
                            harvest_scene->spawn_floating_label_for_ing_type(Ingredient::SubType::Undead, "+1");
                        }
                    }
                    else
                    {
                        //refund the cost
                        building->create_ingredients(Ingredient::SubType::Soul, 1);
                        building->create_ingredients(Ingredient::SubType::Flesh, 6);
                        building->create_ingredients(Ingredient::SubType::Blood, 9);
                    };
                };
                nuitems_config.push_back({
                    blood_oath,
                    DetailType::Recipe,
                    {
                        blood_oath->name,
                        blood_oath->description,
                        "ingredients/undead.png"
                    }});
            };

            if (building->name == "The Arena")
            {
                spTechnology combat_dmg = std::make_shared<Technology>(Technology::SubType::CombatDamage);
                nuitems_config.push_back({
                    combat_dmg,
                    DetailType::Tech,
                    {
                        "Buy Sword",
                        "",
                        "menu_icons/combat_damage.png"
                    }});

                spTechnology crit_chance = std::make_shared<Technology>(Technology::SubType::CombatCritChance);
                nuitems_config.push_back({
                    crit_chance,
                    DetailType::Tech,
                    {
                        "Oil coating",
                        "",
                        "menu_icons/crit_chance.png"
                    }});

                spTechnology crit_factor = std::make_shared<Technology>(Technology::SubType::CombatCritFactor);
                nuitems_config.push_back({
                    crit_factor,
                    DetailType::Tech,
                    {
                        "Sharpen blade",
                        "",
                        "menu_icons/crit_factor.png"
                    }});
            };

            if (building->name == "The Dump")
            {
                spRecipe send_scavenger = std::make_shared<Recipe>("Undead scavenge", "Send 3 undead scavenging for you.");
                send_scavenger->components = mistIngredient({
                    { Ingredient::SubType::Undead, 3 }
                });
                send_scavenger->_callback = [building]()
                {
                    bool can_fit_more_items = true; //TODO
                    if (can_fit_more_items)
                    {
                        //add a bunch of clicks to the harvestable (assuming
                        // though that its the dumpster harvestable)
                        cocos2d::Scene* scene = cocos2d::Director::getInstance()->getRunningScene();
                        auto harvest_scene = dynamic_cast<HarvestScene*>(scene->getChildByName("HarvestScene"));
                        if (harvest_scene)
                        {
                            Node* raw_harvestable = harvest_scene->getChildByName("harvestable");
                            if (raw_harvestable)
                            {
                                Harvestable* harvestable = dynamic_cast<Harvestable*>(raw_harvestable);
                                if (harvestable)
                                {
                                    res_count_t send_scavenger = 1;
                                    auto achievement_manager = AchievementManager::getInstance();
                                    std::shared_ptr<BaseAchievement> achievement = achievement_manager->getAchievement(AchievementType::TotalUndeadScavenging);
                                    achievement->increment_by_n(send_scavenger);

                                    //adds 33% progress to harvestable
                                    harvestable->add_current_clicks(harvestable->click_limit*.33f);
                                    std::unique_ptr<HarvestableManager>& harvestable_manager = GameLogic::getInstance()->harvestable_manager;
                                    harvestable_manager->stored_dumpster_clicks = harvestable->get_current_clicks();
                                    if (harvestable->should_shatter()) {
                                        harvestable->shatter();
                                    };
                                };
                            };
                        }
                        else
                        {
                            CCLOG("cant send scavenger because i can't find harvest_scene");
                        };
                    }
                    else
                    {
                        //refund the cost
                        building->create_ingredients(Ingredient::SubType::Undead, 3);
                    };
                };
                nuitems_config.push_back({
                    send_scavenger,
                    DetailType::Recipe,
                    {
                        send_scavenger->name,
                        send_scavenger->description,
                        "menu_icons/scavenge.png"
                    }
                    });
            };
            if (building->name == "The Mine")
            {
                //transmute copper
                spRecipe transmute_copper = std::make_shared<Recipe>("Transmute: Iron", "Gain 10 Copper\n -- costs 2 Iron");
                transmute_copper->components = mistIngredient({
                    { Ingredient::SubType::Copper, 10 }
                });
                transmute_copper->_callback = [building]()
                {
                    bool can_fit_more_iron = true; //TODO
                    if (can_fit_more_iron)
                    {
                        building->create_ingredients(Ingredient::SubType::Iron, 2);
                        cocos2d::Scene* scene = cocos2d::Director::getInstance()->getRunningScene();
                        auto harvest_scene = dynamic_cast<HarvestScene*>(scene->getChildByName("HarvestScene"));
                        if (harvest_scene)
                        {
                            harvest_scene->show_ingredient_listview_for_ing_type(Ingredient::SubType::Iron);
                            harvest_scene->spawn_floating_label_for_ing_type(Ingredient::SubType::Iron, "+2");
                        }
                    }
                    else
                    {
                        //refund the cost
                        building->create_ingredients(Ingredient::SubType::Copper, 10);
                    };
                };
                nuitems_config.push_back({
                    transmute_copper,
                    DetailType::Recipe,
                    {
                        transmute_copper->name,
                        transmute_copper->description,
                        "ingredients/copper.png",
                    }});

                //build minecart
                spRecipe build_minecart = std::make_shared<Recipe>("Build Minecart", "Dig deeper\n30 Wood, 8 Iron, 25 Copper");
                build_minecart->components = mistIngredient({
                    { Ingredient::SubType::Wood, 30 },
                    { Ingredient::SubType::Iron, 8 },
                    { Ingredient::SubType::Copper, 25 }
                });
                build_minecart->_callback = [building]()
                {
                    bool can_fit_more_carts = true; //TODO
                    if (can_fit_more_carts)
                    {
                        building->create_ingredients(Ingredient::SubType::Minecart, 1);
                        cocos2d::Scene* scene = cocos2d::Director::getInstance()->getRunningScene();
                        auto harvest_scene = dynamic_cast<HarvestScene*>(scene->getChildByName("HarvestScene"));
                        if (harvest_scene)
                        {
                            harvest_scene->show_ingredient_listview_for_ing_type(Ingredient::SubType::Iron);
                            harvest_scene->spawn_floating_label_for_ing_type(Ingredient::SubType::Minecart, "+1");
                        }
                    }
                    else
                    {
                        //refund the cost
                        building->create_ingredients(Ingredient::SubType::Wood, 30);
                        building->create_ingredients(Ingredient::SubType::Iron, 8);
                        building->create_ingredients(Ingredient::SubType::Copper, 25);
                    };
                };
                nuitems_config.push_back({
                    build_minecart,
                    DetailType::Recipe,
                    {
                        build_minecart->name,
                        build_minecart->description,
                        "ingredients/minecart.png",
                    }});

                //build minerails
                spRecipe build_minerails = std::make_shared<Recipe>("Build Rails", "Place & travel to altars\n-- costs 5 Wood, 15 Copper");
                build_minerails->components = mistIngredient({
                    { Ingredient::SubType::Wood, 5 },
                    { Ingredient::SubType::Copper, 15 }
                });
                build_minerails->_callback = [building]()
                {
                    bool can_fit_more_carts = true; //TODO
                    if (can_fit_more_carts)
                    {
                        building->create_ingredients(Ingredient::SubType::MineRails, 1);
                        cocos2d::Scene* scene = cocos2d::Director::getInstance()->getRunningScene();
                        auto harvest_scene = dynamic_cast<HarvestScene*>(scene->getChildByName("HarvestScene"));
                        if (harvest_scene)
                        {
                            harvest_scene->show_ingredient_listview_for_ing_type(Ingredient::SubType::Iron);
                            harvest_scene->spawn_floating_label_for_ing_type(Ingredient::SubType::MineRails, "+1");
                        }
                    }
                    else
                    {
                        //refund the cost
                        building->create_ingredients(Ingredient::SubType::Wood, 5);
                        building->create_ingredients(Ingredient::SubType::Copper, 15);
                    };
                };
                nuitems_config.push_back({
                    build_minerails,
                    DetailType::Recipe,
                    {
                        build_minerails->name,
                        build_minerails->description,
                        "ingredients/minerails.png",
                    }});
            };

            if (building->name == "The Marketplace")
            {
                spTechnology double_click_pwr = std::make_shared<Technology>(Technology::SubType::ClickDoublePower);
                nuitems_config.push_back({
                    double_click_pwr,
                    DetailType::Tech,
                    {
                        "Boost ouput",
                        "",
                        "ingredients/paper.png"
                    }});

                spTechnology update_wallet_cap = std::make_shared<Technology>(Technology::SubType::RaiseWalletCap);
                nuitems_config.push_back({
                    update_wallet_cap,
                    DetailType::UpgradeWalletCapTech,
                    {
                        "Coin Storage +",
                        "",
                        ""
                    }});

                ///Moved to City Management
                // //open scrap menu button
                // std::string child_name = "open_scrap_menu";
                // auto existing_node = listview->getChildByName(child_name);
                // if (existing_node)
                // {
                //     existing_node->removeFromParentAndCleanup(false);
                //     listview->addChild(existing_node);
                //     return;
                // }
                //
                // //clone the new item
                // BuildingNuItem* menu_item;
                // menu_item = BuildingNuItem::create(listview, building);
                // menu_item->setName(child_name);
                // menu_item->set_title("Open Scrap Menu");
                // menu_item->set_image("lineDark28.png");
                //
                // menu_item->set_description("Convert Items into scrap\nIncreases city investment.");
                //
                // menu_item->set_touch_ended_callback([menu_item]()
                // {
                //     GameDirector::switch_to_scrap_item_menu();
                //     do_vibrate(5);
                // });

            };

            if (building->name == "The Forest")
            {
                spTechnology raise_wood_find = std::make_shared<Technology>(Technology::SubType::RaiseWoodFind);
                nuitems_config.push_back({
                    raise_wood_find,
                    DetailType::Tech,
                    {
                        "Bigger axes",
                        "",
                        "menu_icons/wood_find_upgrade.png"
                    }});
            };

            if (building->name == "The Workshop")
            {
                //salesmen boost
                spRecipe salesmen_boost = std::make_shared<Recipe>("Raise prices", "More value per salesmen.\n-- 3 bread");
                salesmen_boost->components = mistIngredient{
                    { Ingredient::SubType::Bread, 3.0 }
                };
                salesmen_boost->_callback = [](){
                    auto workshop = BUILDUP->city->building_by_name("The Workshop");

                    Technology technology = Technology(TechSubType::SalesmenBaseBoost);

                    technology.set_been_unlocked(true);

                    res_count_t def = 0.0;
                    res_count_t num_researched = map_get(workshop->techtree->tech_map, technology.sub_type, def);
                    workshop->techtree->tech_map[technology.sub_type] = num_researched + 1;
                };

                nuitems_config.push_back({
                    salesmen_boost,
                    DetailType::ChangeTargetRecipe,
                    {
                        salesmen_boost->name,
                        salesmen_boost->description,
                        ""
                    }});

                //weaken flesh
                spRecipe weaken_flesh = std::make_shared<Recipe>("Dead meat", "Weaken your enemies\n-- 3 flies, 3 flesh");
                weaken_flesh->components = mistIngredient{
                    { Ingredient::SubType::Fly, 3.0 },   //TODO not use only three
                    { Ingredient::SubType::Flesh, 3.0 }, // because we've got 3
                    { Ingredient::SubType::Loaf, 3.0 }   // hardcoded
                };
                weaken_flesh->_callback = [](){
                    auto workshop = BUILDUP->city->building_by_name("The Workshop");

                    Technology technology = Technology(TechSubType::CombatWeakenEnemy);

                    technology.set_been_unlocked(true);

                    res_count_t def = 0.0;
                    res_count_t num_researched = map_get(workshop->techtree->tech_map, technology.sub_type, def);
                    workshop->techtree->tech_map[technology.sub_type] = num_researched + 1;
                };

                nuitems_config.push_back({
                    weaken_flesh,
                    DetailType::ChangeTargetRecipe,
                    {
                        weaken_flesh->name,
                        weaken_flesh->description,
                        ""
                    }});
            };

            int i = 0;
            for (auto config : nuitems_config)
            {
                int child_tag = i++;

                if (this->if_tag_exists_in_listview(child_tag, listview)) { continue; }

                NuItem* menu_item;
                if (config.type == DetailType::Recipe) {
                    menu_item = RecipeNuItem::create(listview, building);
                }
                else if (config.type == DetailType::Misc) {
                    menu_item = BuildingNuItem::create(listview, building);
                }
                else if (config.type == DetailType::Tech) {
                    menu_item = TechNuItem::create(listview, building);
                }
                else if (config.type == DetailType::ChangeTargetRecipe) {
                    menu_item = TargetRecipeNuItem::create(listview, building);
                }
                else if (config.type == DetailType::UpgradeWalletCapTech) {
                    menu_item = UpgradeWalletNuItem::create(listview, building);
                }

                menu_item->setTag(child_tag);

                menu_item->set_title(config.config.name);
                menu_item->set_description(config.config.description);
                if (config.config.local_img_path != "")
                {
                    menu_item->set_image(config.config.local_img_path, cocos2d::ui::Widget::TextureResType::LOCAL);
                };

                //RecipeNuItem specifics
                if (dynamic_cast<RecipeNuItem*>(menu_item))
                {
                    spRecipe recipe = std::static_pointer_cast<Recipe>(config.object);
                    dynamic_cast<RecipeNuItem*>(menu_item)->other_init(recipe);
                }
                else if (dynamic_cast<TechNuItem*>(menu_item))
                {
                    spTechnology tech = std::static_pointer_cast<Technology>(config.object);
                    dynamic_cast<TechNuItem*>(menu_item)->other_init(tech);
                }
                else if (dynamic_cast<TargetRecipeNuItem*>(menu_item))
                {
                    spRecipe recipe = std::static_pointer_cast<Recipe>(config.object);
                    dynamic_cast<TargetRecipeNuItem*>(menu_item)->other_init(recipe);
                }
                else if (dynamic_cast<UpgradeWalletNuItem*>(menu_item))
                {
                    dynamic_cast<UpgradeWalletNuItem*>(menu_item)->other_init();
                }

            };
        };

        listview->schedule(update_listview, AVERAGE_DELAY, "update_listview");
        update_listview(0);
    };
};

void SideListView::setup_powers_listview_as_powers()
{
    TabManager::TabTypes tab_type = TabManager::TabTypes::PowersTab;

    for (spBuilding building : BUILDUP->city->buildings)
    {
        ui::ListView* listview = this->powers_listviews->at(building->name);

        ///send feedback
        auto send_feeback = [this, listview, building, tab_type](float dt)
        {
            if (this->tabs.is_tab_active(tab_type, building) == false)
            {
                try_set_visible(listview, false);
                return;
            }

            try_set_visible(listview, true);

            //if the child already exists, do nothing
            std::string child_name = "send_feeback";
            auto existing_node = listview->getChildByName(child_name);
            if (existing_node)
            {
                return;
            }

            //clone the new item
            BuildingNuItem* menu_item;
            menu_item = BuildingNuItem::create(listview, building);
            menu_item->setName(child_name);
            menu_item->set_title("Send feedback");
            menu_item->set_image("lineDark16.png");

            auto game_logic = GameLogic::getInstance();
			menu_item->set_description("Send us feedback!\nAnything goes, bugs, ideas!");

            menu_item->set_touch_ended_callback([menu_item]()
            {
                do_vibrate(5);
                std::stringstream url_ss;
                url_ss << "mailto:tankorsmash@tankorsmash.com?";
                url_ss << "subject=";
                url_ss << "[" << Application::getInstance()->getVersion() << "] ";
                url_ss << "Build Up The Base Feedback&";
                url_ss << "body=Hey there, I've got a feature, bug, quality of life, or enhancement I'd love to see, check it out:";
                std::string url = url_ss.str();

                CCLOG("prepping email: %s", url.c_str());
                Application::getInstance()->openURL(url);
            });

        };

        listview->schedule(send_feeback, AVERAGE_DELAY, "send_feeback");
        send_feeback(0);


        ///Open leaderboards
        auto open_leaderboard = [this, listview, building, tab_type](float dt)
        {
            if (this->tabs.is_tab_active(tab_type, building) == false)
            {
                try_set_visible(listview, false);
                return;
            }
            try_set_visible(listview, true);

            //if the child already exists, do nothing
            std::string child_name = "open_leaderboard";
            auto existing_node = listview->getChildByName(child_name);
            if (existing_node)
            {
                return;
            }

            //clone the new item
            BuildingNuItem* menu_item;
            menu_item = BuildingNuItem::create(listview, building);
            menu_item->setName(child_name);
            menu_item->set_title("Leaderboard");
            menu_item->set_description("See who's better than you");
            menu_item->set_image("leaderboard.png");

            menu_item->set_touch_ended_callback([]()
            {
                CCLOG("Pressed open open leaderboard");

                //TODO use actual url
                auto username = DataManager::get_string_from_data("username", "");
                Application::getInstance()->openURL("http://tankorsmash.webfactional.com/leaderboard?username="+username);
            });

        };

        listview->schedule(open_leaderboard, AVERAGE_DELAY, "open_leaderboard");
        open_leaderboard(0);

        ///sell all power
        auto update_sellall = [this, listview, building, tab_type](float dt)
        {
            if (this->tabs.is_tab_active(tab_type, building) == false)
            {
                try_set_visible(listview, false);
                return;
            }
            try_set_visible(listview, true);

            //if the child already exists, do nothing
            std::string child_name = "sell_all";
            auto existing_node = listview->getChildByName(child_name);
            if (existing_node)
            {
                return;
            }

            //clone the new item
            BuildingNuItem* menu_item;
            menu_item = BuildingNuItem::create(listview, building);
            menu_item->setName(child_name);
            menu_item->set_title("Sell all");
            menu_item->set_description("Sells all basic resources instantly");
            menu_item->set_image("sell_all.png");
            menu_item->set_original_image("sell_all.png");
            menu_item->cooldown = GameLogic::getInstance()->power_sell_all_cooldown;

            menu_item->set_touch_ended_callback([]()
            {
                CCLOG("selling all by cb");
                mistIngredient& all_ingredients = BUILDUP->get_all_ingredients();
                for (auto ingredient : all_ingredients)
                {
                    IngredientSubType ing_type = ingredient.first;

                    //dont sell advanced ingredients
                    auto& excluded_ing_types = Ingredient::advanced_ingredients;
                    if (std::find(excluded_ing_types.begin(), excluded_ing_types.end(), ing_type) != excluded_ing_types.end())
                    {
                        continue;
                    }

                    res_count_t _def = 0;
                    res_count_t ing_count = map_get(all_ingredients, ing_type, _def);
                    res_count_t sale_price = Ingredient::type_to_value.at(ing_type);

                    BEATUP->add_total_coin(ing_count*sale_price);

                    all_ingredients[ing_type] = 0;
                }

                //start the countdown for the cooldown
                GameLogic::getInstance()->power_sell_all_cooldown->reset();
            });

        };

        listview->schedule(update_sellall, AVERAGE_DELAY, "update_sellall");
        update_sellall(0);

        ///sell all advanced
        auto update_sellall_advanced = [this, listview, building, tab_type](float dt)
        {
            if (this->tabs.is_tab_active(tab_type, building) == false)
            {
                try_set_visible(listview, false);
                return;
            }
            try_set_visible(listview, true);

            //if the child already exists do nothing
            std::string child_name = "sell_all_advanced";
            auto existing_node = listview->getChildByName(child_name);
            if (existing_node)
            {
                return;
            }

            //clone the new item
            BuildingNuItem* menu_item;
            menu_item = BuildingNuItem::create(listview, building);
            menu_item->setName(child_name);
            menu_item->set_title("Sell all (ADV)");
            menu_item->set_description("Sells all advanced resources instantly");
            menu_item->set_image("sell_all.png");
            menu_item->set_original_image("sell_all.png");
            menu_item->cooldown = GameLogic::getInstance()->power_sell_all_cooldown;

            menu_item->set_touch_ended_callback([]()
            {
                CCLOG("selling all by cb");
                mistIngredient& all_ingredients = BUILDUP->get_all_ingredients();
                for (auto ingredient : all_ingredients)
                {
                    IngredientSubType ing_type = ingredient.first;

                    //dont sell advanced ingredients
                    auto& excluded_ing_types = Ingredient::basic_ingredients;
                    if (std::find(excluded_ing_types.begin(), excluded_ing_types.end(), ing_type) != excluded_ing_types.end())
                    {
                        continue;
                    }

                    res_count_t _def = 0;
                    res_count_t ing_count = map_get(all_ingredients, ing_type, _def);
                    res_count_t sale_price = Ingredient::type_to_value.at(ing_type);

                    BEATUP->add_total_coin(ing_count*sale_price);

                    all_ingredients[ing_type] = 0;
                }

                //start the countdown for the cooldown
                GameLogic::getInstance()->power_sell_all_cooldown->reset();
            });

        };

        listview->schedule(update_sellall_advanced, AVERAGE_DELAY, "update_sellall_advanced");
        update_sellall_advanced(0);



        /// save
        auto update_save = [this, listview, building, tab_type](float dt)
        {
            if (this->tabs.is_tab_active(tab_type, building) == false)
            {
                try_set_visible(listview, false);
                return;
            }
            try_set_visible(listview, true);

            //if the child already exists, do nothing
            std::string child_name = "save";
            auto existing_node = listview->getChildByName(child_name);
            if (existing_node)
            {
                return;
            }

            //clone the new item
            BuildingNuItem* menu_item;
            menu_item = BuildingNuItem::create(listview, building);
            menu_item->setName(child_name);
            menu_item->set_title("Save Game");
            menu_item->set_description("Autosaves every 30s, but you never know.");
            menu_item->set_image("lineDark29.png");

            menu_item->set_touch_ended_callback([]()
            {
                CCLOG("Pressed Save, saving progress");

                GameLogic::save_all();
            });

        };

        listview->schedule(update_save, AVERAGE_DELAY, "update_save");
        update_save(0);


        ///toggle vibration alert
        auto toggle_vibration = [this, listview, building, tab_type](float dt)
        {
            if (this->tabs.is_tab_active(tab_type, building) == false)
            {
                try_set_visible(listview, false);
                return;
            }
            try_set_visible(listview, true);

            //if the child already exists, do nothing
            std::string child_name = "toggle_vibration";
            auto existing_node = listview->getChildByName(child_name);
            if (existing_node)
            {
                return;
            }

            //clone the new item
            BuildingNuItem* menu_item;
            menu_item = BuildingNuItem::create(listview, building);
            menu_item->setName(child_name);
            menu_item->set_title("Toggle vibration");
            menu_item->set_image("lineDark28.png");

            auto game_logic = GameLogic::getInstance();
            bool existing_vibration = game_logic->get_can_vibrate();
            if (existing_vibration) { menu_item->set_description("Tired of your phone vibrating? Tap this"); }
            else if (!existing_vibration) { menu_item->set_description("Tired of your phone not vibrating? Tap this"); }

            menu_item->set_touch_ended_callback([menu_item]()
            {
                auto game_logic = GameLogic::getInstance();
                bool existing_vibration = game_logic->get_can_vibrate();
                game_logic->set_can_vibrate(!existing_vibration);

                //vibrate if now enabled
                if (game_logic->get_can_vibrate())
                {
                    do_vibrate(5);
                    menu_item->set_description("Tired of your phone vibrating? Tap this.");
                }
                else
                {
                    menu_item->set_description("Tired of your phone not vibrating? Tap this.");
                };

            });

        };

        listview->schedule(toggle_vibration, AVERAGE_DELAY, "toggle_vibration");
        toggle_vibration(0);

        ///Open log alert
        auto open_log = [this, listview, building, tab_type](float dt)
        {
            if (this->tabs.is_tab_active(tab_type, building) == false)
            {
                try_set_visible(listview, false);
                return;
            }
            try_set_visible(listview, true);

            //if the child already exists do nothing
            std::string child_name = "open_log";
            auto existing_node = listview->getChildByName(child_name);
            if (existing_node)
            {
                return;
            }

            //clone the new item
            BuildingNuItem* menu_item;
            menu_item = BuildingNuItem::create(listview, building);
            menu_item->setName(child_name);
            menu_item->set_title("Open log");
            menu_item->set_description("See what's been happening behind the scenes.");
            menu_item->set_image("lineDark28.png");

            menu_item->set_touch_ended_callback([]()
            {
                CCLOG("Pressed open log");

                auto scene = cocos2d::Director::getInstance()->getRunningScene()->getChildByName("HarvestScene");
                auto modal = new TextBlobModal(scene);
                std::string version_string = Application::getInstance()->getVersion();
                if (version_string.empty())
                {
                    version_string = "0.0.0";
                }
                std::stringstream version_ss;
                version_ss << "Log (v" << version_string << ")";
                modal->set_title(version_ss.str());

                std::string raw_log_string = Logger::get_log_contents();

                auto split_messages = split(raw_log_string, '\n');
                std::reverse(split_messages.begin(), split_messages.end());
                std::stringstream joined_ss;

                int i = 0;
                for (auto s : split_messages)
                {
                    joined_ss << s << std::endl;

                    //get 100 last messages
                    if (i++ >= 100) { break; }
                };
                modal->set_body(joined_ss.str());
            });

        };

        listview->schedule(open_log, AVERAGE_DELAY, "open_log");
        open_log(0);

    }
};

spListviewMap SideListView::_create_listview(std::string node_name)
{
    Node* sidebar_panel = this->parent->getChildByName("sidebar_panel");

	auto orig_listview = static_cast<ui::ListView*>(sidebar_panel->getChildByName(node_name));

    spListviewMap result = std::make_shared<listviewMap>();
    for (spBuilding building : BUILDUP->city->buildings)
    {
        auto listview = dynamic_cast<ui::ListView*>(orig_listview->clone());
        sidebar_panel->addChild(listview);

        listview->setScrollBarAutoHideEnabled(false);
        listview->setScrollBarPositionFromCorner(Vec2(10, 20));
        listview->setScrollBarWidth(20.0f);

        listview->setScrollBarOpacity(255);
        listview->setScrollBarAutoHideEnabled(false);

        listview->setScrollBarColor({ 48, 96, 130 });

        listview->setMagneticType(ui::ListView::MagneticType::TOP); //scrolls to TOP of current item, if needed; not sure why but this helps the jumping around as you add new items
        listview->setMagneticAllowedOutOfBoundary(false); //true is default, doesnt allow overscrolling sort of. if you go out of bounds, it'll scroll it back


        (*result)[building->name] = listview;

    }

    return result;
}
