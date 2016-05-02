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

    this->parent->addChild(button);

    return button;
};

void SideListView::setup_tab_buttons()
{
    auto toggle_buttons = [this](Ref* target, ui::Widget::TouchEventType evt) {
        if (evt == ui::Widget::TouchEventType::ENDED) {
            this->shop_listview->setVisible(false);
            this->detail_listview->setVisible(false);

            this->tab_shop_btn->setEnabled(true);
            this->tab_detail_btn->setEnabled(true);

            ui::Button* target_button = dynamic_cast<ui::Button*>(target);
            target_button->setEnabled(false);

            if (target_button == this->tab_shop_btn) { this->shop_listview->setVisible(true); }
            else if (target_button == this->tab_detail_btn) { this->detail_listview->setVisible(true); }
        };
    };

    this->tab_shop_btn = this->_create_button("tab_1_btn");
    this->tab_shop_btn->addTouchEventListener(toggle_buttons);

    this->tab_detail_btn = this->_create_button("tab_2_btn");
    this->tab_detail_btn->addTouchEventListener(toggle_buttons);

    //prepress the shop button
    this->tab_shop_btn->setEnabled(false);

}

void SideListView::setup_listviews()
{
    this->shop_listview = this->_create_listview("shop_listview");
    this->detail_listview = this->_create_listview("detail_listview");

    float update_delay = 0.1f;

    auto clean_children_on_target_change = [this](float dt)
    {
        spBuilding target_building = BUILDUP->get_target_building();
        if (target_building->name != this->current_target->name)
        {
            CCLOG("Changed building!, rebuilding detail and shop listview");
            this->current_target = target_building;
            this->detail_listview->removeAllChildren();
            this->shop_listview->removeAllChildren();
        }
    };
    this->parent->schedule(clean_children_on_target_change, update_delay, "clean_children");
};

void SideListView::setup_shop_listview_as_harvesters()
{
    float update_delay = 0.1f;
    auto update_harvester_listview = [this, update_delay](float dt)
    {
        //placeholder for things we'll need to put in the sidebar
        struct HarvestConfig{
            std::string node_name;
            Harvester::SubType harv_type;
        };

        std::vector<HarvestConfig> nuitems_config = {
            {"harvester_item_one", Harvester::SubType::One},
            {"harvester_item_two", Harvester::SubType::Two},
            {"harvester_item_three", Harvester::SubType::Three},
            {"harvester_item_four", Harvester::SubType::Four},
            {"harvester_item_five", Harvester::SubType::Five}
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
            auto menu_item = HarvesterShopNuItem::create();
            menu_item->my_init(shop_listview, config.harv_type, BUILDUP->get_target_building()->punched_sub_type);
            menu_item->setName(child_name);

            //since we only set the ing_type of the menu item above, it doesn't
            //change to adapt for the building, so we cheat and do it here.
            //this'll get moved to a json map or something between building and
            //harvest sub types
            menu_item->schedule([menu_item](float dt){
                menu_item->ing_type = BUILDUP->get_target_building()->punched_sub_type;
            }, update_delay, "update_ing_type");


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

void SideListView::setup_detail_listview_as_recipes()
{
    float update_delay = 0.1f;

    ///DETAIL LISTVIEW
    auto update_detail_listview = [this, update_delay](float dt)
    {
        spBuilding target_building = BUILDUP->get_target_building();

        struct MenuItemConfig {
            std::string name;
            std::string description;
        };

        struct DetailConfig {
            spRecipe recipe;
            MenuItemConfig config;
        };

        std::vector<DetailConfig> nuitems_config;

        for (spRecipe recipe : target_building->data->get_all_recipes())
        {
            nuitems_config.push_back({
                recipe, {
                    recipe->name,
                    recipe->description
                } });
        };

        int i = 0;
        for (auto config : nuitems_config)
        {
            int child_tag = i;
            i++;

            spRecipe recipe = config.recipe;
            ui::ListView* listview = detail_listview;

            //if the child already exists, put it at the end of the listview, maintaining order as config
            bool existed = this->try_push_back(child_tag, listview);
            if (existed) { continue; };

            auto menu_item = RecipeNuItem::create();
            menu_item->setTag(child_tag);

            menu_item->my_init(recipe, target_building, listview);
            menu_item->set_title(config.config.name);
            menu_item->set_description(config.config.description);

            recipe->_callback = [target_building, recipe]() {
                for (auto pair : recipe->outputs) {
                    Ingredient::SubType ing_type = pair.first;
                    int count = pair.second;
                    target_building->create_ingredients(ing_type, count);
                };
            };


            menu_item->schedule([menu_item](float dt){
                menu_item->building = BUILDUP->get_target_building();
            }, update_delay, "update_ing_type");

        };
    };

    detail_listview->schedule(update_detail_listview, update_delay, "update_listview");
    update_detail_listview(0);
};

ui::ListView* SideListView::_create_listview(std::string node_name)
{
    auto inst = CSLoader::getInstance();
    Node* harvest_scene_editor = inst->createNode("editor/scenes/base_scene.csb");

    auto listview = static_cast<ui::ListView*>(harvest_scene_editor->getChildByName(node_name));
    listview->removeFromParent();
    this->parent->addChild(listview);

    return listview;
}
