#include "BuildingsSideBar.h"

#include "constants.h"

#include "cocos2d.h"
#include "cocostudio/ActionTimeline/CSLoader.h"
#include "ui/CocosGUI.h"

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
#include "Serializer.h"
#include "Clock.h"
#include "Modal.h"
#include "Logging.h"
#include "DataManager.h"


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
    this->active_building = building;
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

SideListView::SideListView(Node* parent, spBuilding current_target) : current_target(current_target), parent(parent)
{
    this->tabs = TabManager();

    this->setup_listviews();
    this->setup_tab_buttons();


    this->tabs.button_map = std::map<ui::Button*, std::pair<TabManager::TabTypes, spListviewMap>>{
        { this->tab_shop_btn, { TabManager::TabTypes::ShopTab, this->shop_listviews } },
        { this->tab_detail_btn, { TabManager::TabTypes::DetailTab, this->detail_listviews } },
        { this->tab_building_btn, { TabManager::TabTypes::BuildingTab, this->building_listviews } },
        { this->tab_powers_btn, { TabManager::TabTypes::PowersTab, this->powers_listviews } }
    };

    //prepress the shop button
    toggle_buttons(this->tab_shop_btn, ui::Widget::TouchEventType::ENDED);
};

ui::Button* SideListView::_create_button(std::string node_name)
{
    auto inst = CSLoader::getInstance();
    Node* harvest_scene_editor = inst->createNode("editor/scenes/base_scene.csb");

    ui::Button* button = dynamic_cast<ui::Button*>(harvest_scene_editor->getChildByName(node_name));
    load_default_button_textures(button);
    button->removeFromParent();
    Label* button_lbl = (Label*)button->getTitleRenderer();
    button_lbl->setTextColor(Color4B::WHITE);
    button_lbl->enableOutline(Color4B::BLACK, 2);
    button_lbl->getFontAtlas()->setAliasTexParameters();

    this->parent->addChild(button);

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
                ssize_t index = 123456; //sentinel
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
                if (index != 123456)
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

        do_vibrate(32);

    };
};


void SideListView::setup_tab_buttons()
{
    this->tab_shop_btn = this->_create_button("tab_1_btn");
    this->tab_shop_btn->addTouchEventListener(CC_CALLBACK_2(SideListView::toggle_buttons, this));

    this->tab_detail_btn = this->_create_button("tab_2_btn");
    this->tab_detail_btn->addTouchEventListener(CC_CALLBACK_2(SideListView::toggle_buttons, this));

    this->tab_building_btn = this->_create_button("tab_3_btn");
    this->tab_building_btn->addTouchEventListener(CC_CALLBACK_2(SideListView::toggle_buttons, this));

    this->tab_powers_btn = this->_create_button("tab_4_btn");
    this->tab_powers_btn->addTouchEventListener(CC_CALLBACK_2(SideListView::toggle_buttons, this));

}

void SideListView::setup_listviews()
{
    this->shop_listviews = this->_create_listview("shop_listview");
    this->detail_listviews = this->_create_listview("detail_listview");
    this->building_listviews = this->_create_listview("building_listview");
    this->powers_listviews = this->_create_listview("powers_listview");

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
                shop_listview->setVisible(false);
                return;
            }
            shop_listview->setVisible(true);
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
                std::string child_name = config.node_name;
                int tag = i++;
                auto existing_node = try_push_back(tag, shop_listview);
                if (existing_node)
                {
                    continue;
                }
                else
                {
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
                }

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
                menu_item->setName(child_name);
                menu_item->setTag(tag);

                //since we only set the ing_type of the menu item above, it doesn't
                //change to adapt for the building, so we cheat and do it here.
                //this'll get moved to a json map or something between building and
                //harvest sub types
                auto update_target_and_prereq = [menu_item, building, tab_type](float dt){
                    menu_item->ing_type = building->punched_sub_type;

                    if (menu_item->harv_type != Worker::SubType::One) {
                        Worker::SubType ZERO = Worker::SubType::ZERO;
                        std::pair<Worker::SubType, Ingredient::SubType> key = {
                            map_get(req_map, menu_item->harv_type, ZERO),
                            menu_item->ing_type
                        };

                        res_count_t _def = 0;
                        auto prereq_harvester_found = map_get(building->harvesters, key, _def);

                        if (prereq_harvester_found < 5) {
                            menu_item->button->setVisible(false);
                        }
                        else {
                            menu_item->button->setVisible(true);
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
bool SideListView::try_push_back(int child_tag, ui::ListView* listview)
{
    bool existed = false;

    auto existing_node = listview->getChildByTag(child_tag);
    if (existing_node)
    {
        return true;
        existing_node->removeFromParentAndCleanup(false); //dont remove the callbacks, just move it around
        listview->addChild(existing_node);
        existed = true;
    }

    return existed;
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
                listview->setVisible(false);
                return;
            }
            listview->setVisible(true);

            int i = 0;
            int max_level = 15;
            for (int level = 2; level <= max_level; level++)
            {
                int child_tag = i;
                i++;


                //if the child already exists, put it at the end of the listview, maintaining order as config
                bool existed = this->try_push_back(child_tag, listview);
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
                listview->setVisible(false);
                return;
            }
            listview->setVisible(true);

            struct MenuItemConfig {
                std::string name;
                std::string description;
            };

            enum class DetailType
            {
                Recipe = 0,
                Misc = 1,
                Tech = 2,
                ChangeTargetRecipe = 3
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
                        recipe->description
                    } });
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
                        health->add_to_current_val(5);
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
                        blood_oath->description
                    } });
            };
            if (building->name == "The Graveyard")
            {
                spRecipe blood_oath = std::make_shared<Recipe>("Raise Dead", "One soul, 6 flesh, 9 blood.");
                blood_oath->components = mistIngredient({
                    { Ingredient::SubType::Soul, 1 },
                    { Ingredient::SubType::Flesh, 6 },
                    { Ingredient::SubType::Blood, 9 }
                });
                blood_oath->_callback = [building](){
                    bool can_fit_more_dead = true;
                    if (can_fit_more_dead)
                    {
                        building->create_ingredients(Ingredient::SubType::Undead, 1);
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
                        blood_oath->description
                    } });
            };

            if (building->name == "The Arena")
            {
                spTechnology combat_dmg = std::make_shared<Technology>(Technology::SubType::CombatDamage);
                nuitems_config.push_back({
                    combat_dmg,
                    DetailType::Tech,
                    {
                        "Buy Sword",
                        ""
                    } });

                spTechnology crit_chance = std::make_shared<Technology>(Technology::SubType::CombatCritChance);
                nuitems_config.push_back({
                    crit_chance,
                    DetailType::Tech,
                    {
                        "Coat sword in oil",
                        ""
                    } });

                spTechnology crit_factor = std::make_shared<Technology>(Technology::SubType::CombatCritFactor);
                nuitems_config.push_back({
                    crit_factor,
                    DetailType::Tech,
                    {
                        "Sharpen blade",
                        ""
                    } });
            };

            if (building->name == "The Marketplace")
            {
                spTechnology double_click_pwr = std::make_shared<Technology>(Technology::SubType::ClickDoublePower);
                nuitems_config.push_back({
                    double_click_pwr,
                    DetailType::Tech,
                    {
                        "Double click power",
                        ""
                    } });
            };

            if (building->name == "The Workshop")
            {
                //salesmen boost
                spRecipe salesmen_boost = std::make_shared<Recipe>("Salesmen Boost", "Requires three bread.");
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
                        salesmen_boost->description
                    } });

                //weaken flesh
                spRecipe weaken_flesh = std::make_shared<Recipe>("Dead meat", "Triple fly, flesh, and loaf make monsters weaker");
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
                        weaken_flesh->description
                    } });
            };

            int i = 0;
            for (auto config : nuitems_config)
            {
                int child_tag = i++;

                //if the child already exists, put it at the end of the listview, maintaining order as config
                bool existed = this->try_push_back(child_tag, listview);
                if (existed)
                {
                    continue;
                };

                BuildingNuItem* menu_item;
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

                menu_item->setTag(child_tag);

                menu_item->set_title(config.config.name);
                menu_item->set_description(config.config.description);

                //RecipeNuItem specifics
                if (dynamic_cast<RecipeNuItem*>(menu_item))
                {
                    spRecipe recipe = static_pointer_cast<Recipe>(config.object);
                    dynamic_cast<RecipeNuItem*>(menu_item)->other_init(recipe);
                }
                else if (dynamic_cast<TechNuItem*>(menu_item))
                {
                    spTechnology tech = static_pointer_cast<Technology>(config.object);
                    dynamic_cast<TechNuItem*>(menu_item)->other_init(tech);
                }
                else if (dynamic_cast<TargetRecipeNuItem*>(menu_item))
                {
                    spRecipe recipe = static_pointer_cast<Recipe>(config.object);
                    dynamic_cast<TargetRecipeNuItem*>(menu_item)->other_init(recipe);
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

        ///Open leaderboards
        auto open_leaderboard = [this, listview, building, tab_type](float dt)
        {
            if (this->tabs.is_tab_active(tab_type, building) == false)
            {
                listview->setVisible(false);
                return;
            }
            listview->setVisible(true);

            //if the child already exists, put it at the back 
            std::string child_name = "open_leaderboard";
            auto existing_node = listview->getChildByName(child_name);
            if (existing_node)
            {
                existing_node->removeFromParentAndCleanup(false);
                listview->addChild(existing_node);
                return;
            }

            //clone the new item
            BuildingNuItem* menu_item;
            menu_item = BuildingNuItem::create(listview, building);
            menu_item->setName(child_name);
            menu_item->set_title("Visit Leaderboard");
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
                listview->setVisible(false);
                return;
            }
            listview->setVisible(true);

            //if the child already exists, put it at the back 
            std::string child_name = "sell_all";
            auto existing_node = listview->getChildByName(child_name);
            if (existing_node)
            {
                existing_node->removeFromParentAndCleanup(false);
                listview->addChild(existing_node);
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

            //exclude types of ingredients for selling all
            auto excluded_ing_types = std::vector<Ingredient::SubType> {
                Ingredient::SubType::Soul,
                Ingredient::SubType::Bread,
                Ingredient::SubType::Loaf,
                Ingredient::SubType::Undead
            };


            menu_item->set_touch_ended_callback([excluded_ing_types]()
            {
                CCLOG("selling all by cb");
                res_count_t sale_price = 10; //TODO use proper price
                for (spBuilding building : BUILDUP->city->buildings)
                {
                    for (auto ingredient : building->ingredients)
                    {
                        IngredientSubType ing_type = ingredient.first;
                        //skip if in exclude
                        if (std::find(excluded_ing_types.begin(), excluded_ing_types.end(), ing_type) != excluded_ing_types.end())
                        {
                            continue;
                        }
                        res_count_t _def = 0;
                        res_count_t ing_count = map_get(building->ingredients, ing_type, _def);
                        BEATUP->add_total_coin(ing_count*sale_price);
                        building->ingredients[ing_type] = 0;
                    }
                }

                //start the countdown for the cooldown
                GameLogic::getInstance()->power_sell_all_cooldown->reset();
            });

        };

        listview->schedule(update_sellall, AVERAGE_DELAY, "update_sellall");
        update_sellall(0);



        /// save
        auto update_save = [this, listview, building, tab_type](float dt)
        {
            if (this->tabs.is_tab_active(tab_type, building) == false)
            {
                listview->setVisible(false);
                return;
            }
            listview->setVisible(true);

            //if the child already exists, put it at the back 
            std::string child_name = "save";
            auto existing_node = listview->getChildByName(child_name);
            if (existing_node)
            {
                existing_node->removeFromParentAndCleanup(false);
                listview->addChild(existing_node);
                return;
            }

            //clone the new item
            BuildingNuItem* menu_item;
            menu_item = BuildingNuItem::create(listview, building);
            menu_item->setName(child_name);
            menu_item->set_title("Save Game");
            menu_item->set_description("Autosaves on quit, but you never know.");
            menu_item->set_image("lineDark29.png");

            menu_item->set_touch_ended_callback([]()
            {
                CCLOG("Pressed Save, saving progress");

                GameLogic::save_all();
            });

        };

        listview->schedule(update_save, AVERAGE_DELAY, "update_save");
        update_save(0);


        ///Open log alert
        auto open_log = [this, listview, building, tab_type](float dt)
        {
            if (this->tabs.is_tab_active(tab_type, building) == false)
            {
                listview->setVisible(false);
                return;
            }
            listview->setVisible(true);

            //if the child already exists, put it at the back 
            std::string child_name = "open_log";
            auto existing_node = listview->getChildByName(child_name);
            if (existing_node)
            {
                existing_node->removeFromParentAndCleanup(false);
                listview->addChild(existing_node);
                return;
            }

            //clone the new item
            BuildingNuItem* menu_item;
            menu_item = BuildingNuItem::create(listview, building);
            menu_item->setName(child_name);
            menu_item->set_title("Open log");
            menu_item->set_description("See what's been happening");
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
    auto inst = CSLoader::getInstance();
    Node* harvest_scene_editor = inst->createNode("editor/scenes/base_scene.csb");

    spListviewMap result = std::make_shared<listviewMap>();
    for (spBuilding building : BUILDUP->city->buildings)
    {
        auto orig_listview = static_cast<ui::ListView*>(harvest_scene_editor->getChildByName(node_name));
        auto listview = dynamic_cast<ui::ListView*>(orig_listview->clone());
        listview->removeFromParent();

        listview->setScrollBarAutoHideEnabled(false);
        listview->setScrollBarPositionFromCorner(Vec2(10, 20));
        listview->setScrollBarWidth(20.0f);

        listview->setScrollBarOpacity(255);
        listview->setScrollBarAutoHideEnabled(false);

        listview->setScrollBarColor({ 48, 96, 130 });

        // listview->setMagneticType(ui::ListView::MagneticType::TOP); //scrolls to TOP of current item, if needed
        listview->setMagneticAllowedOutOfBoundary(false); //true is default, doesnt allow overscrolling sort of. if you go out of bounds, it'll scroll it back

        this->parent->addChild(listview);

        (*result)[building->name] = listview;

    }

    return result;
}
