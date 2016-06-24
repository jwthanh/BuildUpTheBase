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


USING_NS_CC;

SideListView::SideListView(Node* parent, spBuilding current_target) : current_target(current_target), parent(parent)
{
    this->setup_listviews();
    this->setup_tab_buttons();
};

ui::Button* SideListView::_create_button(std::string node_name)
{
    auto inst = CSLoader::getInstance();
    Node* harvest_scene_editor = inst->createNode("editor/scenes/base_scene.csb");

    ui::Button* button = dynamic_cast<ui::Button*>(harvest_scene_editor->getChildByName(node_name));
    load_default_button_textures(button);
    button->removeFromParent();
    ((Label*)button->getTitleRenderer())->getFontAtlas()->setAliasTexParameters();

    this->parent->addChild(button);

    return button;
};

void SideListView::toggle_buttons(Ref* target, ui::Widget::TouchEventType evt)
{
    if (evt == ui::Widget::TouchEventType::ENDED) {
        this->shop_listview->setVisible(false);
        this->detail_listview->setVisible(false);
        this->building_listview->setVisible(false);
        this->powers_listview->setVisible(false);

        this->tab_shop_btn->setEnabled(true);
        this->tab_detail_btn->setEnabled(true);
        this->tab_building_btn->setEnabled(true);
        this->tab_powers_btn->setEnabled(true);

        ui::Button* target_button = dynamic_cast<ui::Button*>(target);
        target_button->setEnabled(false);

        if (target_button == this->tab_shop_btn) { this->shop_listview->setVisible(true); }
        else if (target_button == this->tab_detail_btn) { this->detail_listview->setVisible(true); }
        else if (target_button == this->tab_building_btn) { this->building_listview->setVisible(true); }
        else if (target_button == this->tab_powers_btn) { this->powers_listview->setVisible(true); }
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

    //prepress the shop button
    toggle_buttons(this->tab_shop_btn, ui::Widget::TouchEventType::ENDED);

}

void SideListView::setup_listviews()
{
    this->shop_listview = this->_create_listview("shop_listview");
    this->detail_listview = this->_create_listview("detail_listview");
    this->building_listview = this->_create_listview("building_listview");
    this->powers_listview = this->_create_listview("powers_listview");

    float update_delay = 0.1f;

    auto clean_children_on_target_change = [this](float dt)
    {
        spBuilding target_building = BUILDUP->get_target_building();
        if (target_building->name != this->current_target->name)
        {
            CCLOG("Changed building!, from %s to %s; rebuilding detail and shop listview",
                this->current_target->name.c_str(),
                target_building->name.c_str()
                );
            this->current_target = target_building;

            //TODO this is only necessary when the buildings have different items, like the Detail tab
            this->detail_listview->removeAllChildren();
            this->shop_listview->removeAllChildren();
            this->building_listview->removeAllChildren();
            this->powers_listview->removeAllChildren();

            //scroll to top of all the listviews
            this->detail_listview->scrollToTop(0, false);
            this->shop_listview->scrollToTop(0, false);
            this->building_listview->scrollToTop(0, false);
            this->powers_listview->scrollToTop(0, false);
        }
    };
    this->parent->schedule(clean_children_on_target_change, update_delay, "clean_children");
};

void SideListView::setup_shop_listview_as_harvesters()
{
    float update_delay = 0.1f;
    auto update_harvester_listview = [this, update_delay](float dt)
    {
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
            {WorkerType::Salesman, "salesman_item_one", Worker::SubType::One},
            // {WorkerType::Salesman, "salesman_item_two", Worker::SubType::Two}, //TODO fix sorting before we can add this in
            // {WorkerType::Salesman, "salesman_item_three", Worker::SubType::Three},
            {WorkerType::Harvester, "harvester_item_one", Worker::SubType::One},
            {WorkerType::Harvester, "harvester_item_two", Worker::SubType::Two},
            {WorkerType::Harvester, "harvester_item_three", Worker::SubType::Three},
            {WorkerType::Harvester, "harvester_item_four", Worker::SubType::Four},
            {WorkerType::Harvester, "harvester_item_five", Worker::SubType::Five},
            {WorkerType::Harvester, "harvester_item_six", Worker::SubType::Six}
        };

        auto target_building = BUILDUP->get_target_building();

        if (target_building->name == "The Underscape"){
            nuitems_config.insert(nuitems_config.begin()+1, {WorkerType::Consumer, "consumer_item_one", Worker::SubType::One});
        };

        for (auto config : nuitems_config)
        {
            //if the child already exists, put it at the back 
            std::string child_name = config.node_name;
            auto existing_node = shop_listview->getChildByName(child_name);
            if (existing_node)
            {
                existing_node->removeFromParentAndCleanup(false);
                shop_listview->addChild(existing_node);
                continue;
            }

            //clone the new item
            HarvesterShopNuItem* menu_item;
            if (config.worker_type == WorkerType::Harvester){
                menu_item = HarvesterShopNuItem::create(shop_listview, target_building);
            }
            else if (config.worker_type == WorkerType::Salesman)
            {
                menu_item = SalesmanShopNuItem::create(shop_listview, target_building);
            }
            else if (config.worker_type == WorkerType::Consumer)
            {
                menu_item = ConsumerShopNuItem::create(shop_listview, target_building, Ingredient::SubType::Blood);
            }
            menu_item->my_init(config.harv_type, target_building->punched_sub_type);
            menu_item->setName(child_name);

            //since we only set the ing_type of the menu item above, it doesn't
            //change to adapt for the building, so we cheat and do it here.
            //this'll get moved to a json map or something between building and
            //harvest sub types
            auto update_target_and_prereq = [menu_item](float dt){
                spBuilding target_building = BUILDUP->get_target_building();
                menu_item->ing_type = target_building->punched_sub_type;

                if (menu_item->harv_type != Worker::SubType::One) {
                    std::map<Worker::SubType, Worker::SubType> req_map = {
                        { Worker::SubType::ZERO, Worker::SubType::ZERO },
                        { Worker::SubType::One, Worker::SubType::ZERO },
                        { Worker::SubType::Two, Worker::SubType::One },
                        { Worker::SubType::Three, Worker::SubType::Two },
                        { Worker::SubType::Four, Worker::SubType::Three },
                        { Worker::SubType::Five, Worker::SubType::Four },
                        { Worker::SubType::Six, Worker::SubType::Five },
                        { Worker::SubType::Seven, Worker::SubType::Six },
                        { Worker::SubType::Eigth, Worker::SubType::Seven },
                        { Worker::SubType::Nine, Worker::SubType::Eigth }
                    };

                    std::pair<Worker::SubType, Ingredient::SubType> key = { 
                        map_get(req_map, menu_item->harv_type, Worker::SubType::ZERO),
                        menu_item->ing_type
                    };

                    auto prereq_harvester_found = map_get( target_building->harvesters, key, 0);

                    if (prereq_harvester_found < 5) {
                        menu_item->button->setVisible(false);
                    } else {
                        menu_item->button->setVisible(true);
                    }

                    //hides the item if there's not enough to consume, but this doesnt get run often enough
                    //if (dynamic_cast<ConsumerShopNuItem*>(menu_item)) {
                    //    auto consumer_item = dynamic_cast<ConsumerShopNuItem*>(menu_item);
                    //    if (target_building->count_ingredients(consumer_item->consumed_type) < 1){
                    //        menu_item->button->setVisible(false);
                    //    };
                    //};
                };
            };
            menu_item->schedule(update_target_and_prereq, update_delay, "update_ing_type");
            update_target_and_prereq(0);

        };

    };

    shop_listview->schedule(update_harvester_listview, update_delay, "update_listview");
    update_harvester_listview(0);
};

///tries to push the node at child_tag to the back of the listview. if it didn't exist,
/// it returns false
bool SideListView::try_push_back(int child_tag, ui::ListView* listview)
{
    bool existed = false;

    auto existing_node = listview->getChildByTag(child_tag);
    if (existing_node)
    {
        existing_node->removeFromParentAndCleanup(false); //dont remove the callbacks, just move it around
        listview->addChild(existing_node);
        existed = true;
    }

    return existed;
}

void SideListView::setup_building_listview_as_upgrades()
{
    float update_delay = 0.1f;

    ui::ListView* listview = this->building_listview;

    ///DETAIL LISTVIEW
    auto update_listview = [this, update_delay, listview](float dt)
    {
        spBuilding target_building = BUILDUP->get_target_building();


        int i = 0;
        int max_level = 15;
        for (int level = 2; level <= max_level; level++)
        {
            int child_tag = i;
            i++;


            //if the child already exists, put it at the end of the listview, maintaining order as config
            bool existed = this->try_push_back(child_tag, listview);
            if (existed) { continue; };

            UpgradeBuildingShopNuItem* menu_item = UpgradeBuildingShopNuItem::create(listview, target_building);
            menu_item->my_init(level);

            menu_item->setTag(child_tag);

            menu_item->schedule([menu_item](float dt){
                menu_item->building = BUILDUP->get_target_building();
            }, update_delay, "update_ing_type");

        };
    };

    listview->schedule(update_listview, update_delay, "update_listview");
    update_listview(0);
};

void SideListView::setup_detail_listview_as_recipes()
{
    float update_delay = 0.1f;

    ui::ListView* listview = this->detail_listview;

    ///DETAIL LISTVIEW
    auto update_listview = [this, update_delay, listview](float dt)
    {
        spBuilding target_building = BUILDUP->get_target_building();

        struct MenuItemConfig {
            std::string name;
            std::string description;
        };

        enum class DetailType
        {
            Recipe = 0,
            Misc = 1,
            Tech = 2,
        };

        struct DetailConfig {
            std::shared_ptr<void> object;
            DetailType type;
            MenuItemConfig config;
        };

        std::vector<DetailConfig> nuitems_config;

        for (spRecipe recipe : target_building->data->get_all_recipes())
        {
            nuitems_config.push_back({
                recipe,
                DetailType::Recipe,
                {
                    recipe->name,
                    recipe->description
                } });
        };

        if (target_building->name == "The Underscape")
        {
            spRecipe blood_oath = std::make_shared<Recipe>("Bloodoath", "Gain 5 health\n-- costs 10 blood");
            blood_oath->components = mistIngredient({
                {Ingredient::SubType::Blood, 10}
            });
            blood_oath->_callback = []()
            {
                auto health = BUILDUP->fighter->attrs->health;
                if (health->is_full() == false)
                {
                    health->add_to_current_val(5);
                }
                else
                {
                    //refund the cost
                    spBuilding target_building = BUILDUP->get_target_building();
                    target_building->create_ingredients(Ingredient::SubType::Blood, 10);
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

        if (target_building->name == "The Arena")
        {
            spTechnology combat_dmg = std::make_shared<Technology>(Technology::SubType::CombatDamage);
            spRecipe recipe = std::make_shared<Recipe>("combat_damage", "no desc for tech recipe");

            auto tech_map = target_building->techtree->tech_map;
            res_count_t souls_cost = std::floor(scale_number(2.0L, map_get(tech_map, combat_dmg->sub_type, 0), 1.45L));
            recipe->components = mistIngredient({
                {Ingredient::SubType::Soul, souls_cost}
            });
            combat_dmg->set_ingredient_requirements(recipe);

            std::stringstream ss;
            ss << "Increases combat damage\n-- costs " << souls_cost << " souls";

            nuitems_config.push_back({
                combat_dmg,
                DetailType::Tech,
                {
                    "Buy Sword",
                    ss.str()
                } });
        };

        if (target_building->name == "The Marketplace")
        {
            spTechnology double_click_pwr = std::make_shared<Technology>(Technology::SubType::ClickDoublePower);
            spRecipe recipe = std::make_shared<Recipe>("double_click_power", "no desc for tech recipe");
            recipe->components = mistIngredient({
                {Ingredient::SubType::Seed, 5}
            });
            double_click_pwr->set_ingredient_requirements(recipe);

            nuitems_config.push_back({
                double_click_pwr,
                DetailType::Tech,
                {
                    "Double click power",
                    "Doubles click output\n-- 5 seeds"
                } });
        };

        int i = 0;
        for (auto config : nuitems_config)
        {
            int child_tag = i;
            i++;


            //if the child already exists, put it at the end of the listview, maintaining order as config
            bool existed = this->try_push_back(child_tag, listview);
            if (existed)
            {
                auto existing_node = dynamic_cast<NuItem*>(listview->getChildByTag(child_tag));
                if (existing_node)
                {
                    existing_node->set_title(config.config.name);
                    existing_node->set_description(config.config.description);
                }
                continue;
            };

            BuildingNuItem* menu_item;
            if (config.type == DetailType::Recipe) {
                menu_item = RecipeNuItem::create(listview, target_building);
            }
            else if (config.type == DetailType::Misc) {
                menu_item = BuildingNuItem::create(listview, target_building);
            }
            else if (config.type == DetailType::Tech) {
                menu_item = TechNuItem::create(listview, target_building);
            }

            menu_item->setTag(child_tag);

            menu_item->set_title(config.config.name);
            menu_item->set_description(config.config.description);

            menu_item->schedule([menu_item](float dt){
                menu_item->building = BUILDUP->get_target_building();
            }, update_delay, "update_ing_type");

            //RecipeNuItem specifics
            if (dynamic_cast<RecipeNuItem*>(menu_item))
            {
                spRecipe recipe = static_pointer_cast<Recipe>(config.object);
                dynamic_cast<RecipeNuItem*>(menu_item)->other_init(recipe);
            }
            else if (dynamic_cast<TechNuItem*>(menu_item))
            {
                spTechnology recipe = static_pointer_cast<Technology>(config.object);
                dynamic_cast<TechNuItem*>(menu_item)->other_init(recipe);
            }

        };
    };

    listview->schedule(update_listview, update_delay, "update_listview");
    update_listview(0);
};

void SideListView::setup_powers_listview_as_powers()
{
    ui::ListView* listview = this->powers_listview;

    float update_delay = 0.1f;
    auto update_listview = [this, update_delay](float dt)
    {
        auto target_building = BUILDUP->get_target_building();

        //if the child already exists, put it at the back 
        std::string child_name = "sell_all";
        auto existing_node = this->powers_listview->getChildByName(child_name);
        if (existing_node)
        {
            existing_node->removeFromParentAndCleanup(false);
            this->powers_listview->addChild(existing_node);
            return;
        }

        //clone the new item
        BuildingNuItem* menu_item;
        menu_item = BuildingNuItem::create(this->powers_listview, target_building);
        menu_item->setName(child_name);
        menu_item->set_title("[WIP] Sell all");
        menu_item->set_description("Sells all resources");

        menu_item->set_touch_ended_callback([]()
        {
            CCLOG("selling all");

            res_count_t sale_price = 10; //TODO use proper price
            for (spBuilding building : BUILDUP->city->buildings)
            {
                for (auto ingredient : building->ingredients)
                {
                    IngredientSubType ing_type = ingredient.first;
                    res_count_t ing_count = map_get(building->ingredients, ing_type, 0);
                    BEATUP->add_total_coin(ing_count*sale_price);
                    building->ingredients[ing_type] = 0;
                }
            }
        });

        auto handle_prequistites = [menu_item](float dt){
            bool prereq_satisfied = true;
            if (prereq_satisfied) {
                menu_item->button->setVisible(true);
            } else {
                menu_item->button->setVisible(false);
            }
        };

        menu_item->schedule(handle_prequistites, update_delay, "update_ing_type");
        handle_prequistites(0);

    };

    listview->schedule(update_listview, update_delay, "update_listview");
    update_listview(0);
};

ui::ListView* SideListView::_create_listview(std::string node_name)
{
    auto inst = CSLoader::getInstance();
    Node* harvest_scene_editor = inst->createNode("editor/scenes/base_scene.csb");

    auto listview = static_cast<ui::ListView*>(harvest_scene_editor->getChildByName(node_name));
    listview->removeFromParent();

    listview->setScrollBarAutoHideEnabled(false);
    listview->setScrollBarPositionFromCorner(Vec2(10, 20));
    listview->setScrollBarWidth(20.0f);

    listview->setScrollBarOpacity(255);
    listview->setScrollBarAutoHideEnabled(false);

    listview->setScrollBarColor({48, 96, 130});

    // listview->setMagneticType(ui::ListView::MagneticType::TOP); //scrolls to TOP of current item, if needed
    listview->setMagneticAllowedOutOfBoundary(false); //true is default, doesnt allow overscrolling sort of. if you go out of bounds, it'll scroll it back

    this->parent->addChild(listview);

    return listview;
}
