#include "HarvestScene.h"

#include <cctype>
#include <numeric>
#include <sstream>

#include <json/stringbuffer.h>
#include <regex>

#include "Harvestable.h"
#include "HarvestableManager.h"

#include "GameLogic.h"
#include "NuMenu.h"
#include "Beatup.h"
#include "Util.h"
#include "StaticData.h"
#include "attribute.h"
#include "FShake.h"
#include "MiscUI.h"
#include "Fighter.h"
#include "attribute_container.h"

#include "BuildingsSideBar.h"

#include "HouseBuilding.h"
#include "DataManager.h"
#include "Network.h"
#include "Modal.h"
#include "PopupPanel.h"

#include "magic_particles/_core/mp.h"
#include "magic_particles/mp_cocos.h"
#include "magic_particles/MagicEmitter.h"

#include "Recipe.h" //spRecipe is used in target_recipe
#include "NodeBuilder.h"

#include "base/CCDirector.h"
#include "base/CCEventListenerKeyboard.h"
#include "base/CCEventDispatcher.h"
#include "base/CCEventListenerTouch.h"
#include "2d/CCActionEase.h"
#include "ui/UIPageView.h"
#include "ui/UIListView.h"
#include "ui/UILayout.h"
#include "ui/UILoadingBar.h"
#include "ui/UIText.h"
#include "ui/UIImageView.h"
#include "ui/UIButton.h"
#include "ui/UIScale9Sprite.h"
#include <cocostudio/CocosStudioExtension.h>
#include "ui/UITextField.h"
#include <2d/CCParticleExamples.h>
#include "debugging/ActionPanel.h"
#include "Tutorial.h"
#include "2d/CCActionInstant.h"
#include "utilities/vibration.h"
#include "RandomWeightMap.h"
#include "CCFileUtils.h"

USING_NS_CC;

Node* BaseScene::_harvest_scene_from_editor = NULL;


bool BaseScene::init()
{
    FUNC_INIT(BaseScene);

    BaseScene::_harvest_scene_from_editor = NULL; //reset this because after it gets GCed, it points to garbage
    Node* harvest_scene_editor = this->get_original_scene_from_editor();

    auto children = harvest_scene_editor->getChildren();
    for (Node* child : children)
    {
        child->removeFromParent();
        this->addChild(child);
    }

    this->create_info_panel();
    this->create_player_info_panel();
    this->create_username_input();

    this->create_inventory_listview();
    this->create_shop_listview();

    this->create_goal_loadingbar();

    this->create_building_choicelist();

    this->create_popup_panel();


    return true;
};

void BaseScene::update(float dt)
{
    GameLayer::update(dt);

    GameLogic::getInstance()->update(dt);

};

void BaseScene::onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event)
{
    if (keyCode == EventKeyboard::KeyCode::KEY_BACK || keyCode == EventKeyboard::KeyCode::KEY_ESCAPE)
    {
        this->quit(NULL);
        event->stopPropagation();
    }
    else if (keyCode == EventKeyboard::KeyCode::KEY_A)
    {
        ActionPanel* action_panel = ActionPanel::create();
        action_panel->set_target(this->sidebar->shop_listviews->at("The Graveyard"));
        this->addChild(action_panel);
    }
    else if (keyCode == EventKeyboard::KeyCode::KEY_F)
    {
        auto director = Director::getInstance();
        director->setDisplayStats(!director->isDisplayStats());
    }
    else if (keyCode == EventKeyboard::KeyCode::KEY_F1)
    {
        auto glView = Director::getInstance()->getOpenGLView();
        glView->setFrameSize(1920, 1080);
        glView->setDesignResolutionSize(1920, 1080, ResolutionPolicy::SHOW_ALL);
    }
    else if (keyCode == EventKeyboard::KeyCode::KEY_F2)
    {
        auto glView = Director::getInstance()->getOpenGLView();
        glView->setFrameSize(960, 640);
        glView->setDesignResolutionSize(960, 640, ResolutionPolicy::SHOW_ALL);
    }
    else if (keyCode == EventKeyboard::KeyCode::KEY_F5)
    {
        BEATUP->reload_resources();
    }
    else if (keyCode == EventKeyboard::KeyCode::KEY_GRAVE)
    {
    }
    else if (keyCode == EventKeyboard::KeyCode::KEY_H)
    {
        GameDirector::switch_to_item_chance_menu();
    }
    else if (keyCode == EventKeyboard::KeyCode::KEY_S)
    {
        GameLogic::save_all();
    }
    else if (keyCode == EventKeyboard::KeyCode::KEY_I)
    {
    }
    else if (keyCode == EventKeyboard::KeyCode::KEY_K)
    {
    }
    else if (keyCode == EventKeyboard::KeyCode::KEY_J)
    {
    }
    else if (keyCode == EventKeyboard::KeyCode::KEY_L)
    {
    }
    else if (keyCode == EventKeyboard::KeyCode::KEY_M)
    {
        GameDirector::switch_to_miner_menu();
    }
    else if (keyCode == EventKeyboard::KeyCode::KEY_C)
    {
        GameDirector::switch_to_city_menu();
    }
    else if (keyCode == EventKeyboard::KeyCode::KEY_I)
    {
        GameDirector::switch_to_items_menu();
    }
    else if (keyCode == EventKeyboard::KeyCode::KEY_P)
    {
        this->generate_WIP_welcome_message();
    }
    else if (keyCode == EventKeyboard::KeyCode::KEY_SPACE)
    {
        // SoundLibrary::getInstance()->play_general_harvest_touched();
        // GameDirector::switch_to_building_detail_menu();


    }
}
void BaseScene::generate_WIP_welcome_message()
{
    //combine ingredient types into one vector
    std::vector<Ingredient::SubType> ing_types;
    ing_types.insert(ing_types.cend(), Ingredient::basic_ingredients.begin(), Ingredient::basic_ingredients.end());
    ing_types.insert(ing_types.cend(), Ingredient::advanced_ingredients.begin(), Ingredient::advanced_ingredients.end());

    auto city_ingredients = BUILDUP->get_all_ingredients();

    int width = 4;
    float cell_width = 150.0f;
    float cell_height = 150.0f;

    float cell_margin = 5.0f;

    cocos2d::Vec2 initial_pos = get_center_pos(-300, 200);
    float x = initial_pos.x;
    float y = initial_pos.y;

    int i = 0;
    for (auto ing_type : ing_types)
    {
        auto panel = get_prebuilt_node_from_csb("editor/details/report_ing_detail.csb")->getChildByName("panel");
        panel->removeFromParent();
        panel->setPosition(x, y);
        this->addChild(panel);

        x += cell_width + cell_margin;
        i++;
        if (i >= width)
        {
            i = 0;
            x = initial_pos.x;
            y -= cell_height + cell_margin;
        }

        IngredientData ingredient_data(ing_type);
        std::string img_path = ingredient_data.get_img_large();

        //title
        auto ing_title = dynamic_cast<ui::Text*>(panel->getChildByName("title_lbl"));
        ing_title->setString(ingredient_data.get_name());

        //count
        auto count_lbl = dynamic_cast<ui::Text*>(panel->getChildByName("count_lbl"));
        count_lbl->setString(beautify_double(map_get(city_ingredients, ing_type, 0)));

        //ingredient icon
        auto ing_icon = dynamic_cast<ui::ImageView*>(panel->getChildByName("ing_icon"));
        image_view_scale9_hack(ing_icon);
        ing_icon->loadTexture(img_path);
        set_aliasing(ing_icon);

    }
}

void BaseScene::scroll_to_target_building()
{
    auto building_pageview = dynamic_cast<ui::PageView*>(this->getChildByName("building_pageview"));
    if (building_pageview)
    {
        vsBuilding buildings = BUILDUP->city->buildings;
        auto find_cb = [](spBuilding building) {
            return building == BUILDUP->get_target_building();
        };
        int index = std::find_if(buildings.begin(),
                                 buildings.end(),
                                 find_cb) - buildings.begin();

        if (index > (int)buildings.size())
        {
            CCLOG("no building found, not moving building pageview");
        } else
        {

            building_pageview->ListView::scrollToItem(index, { 0, 0 }, { 0, 0 }, 0.5f);
        }
    }
}

void BaseScene::onEnter()
{
    GameLayer::onEnter();
    scroll_to_target_building();
};

void BaseScene::onSwipeLeft(float dt)
{
    GameLayer::onSwipeLeft(dt);
};

void BaseScene::onSwipeRight(float dt)
{
    GameLayer::onSwipeRight(dt);
};

void BaseScene::create_goal_loadingbar()
{
    auto progress_panel = (ui::Layout*)this->getChildByName("progress_panel");

    //check visibilty for tutorials
    auto check_visible = [progress_panel](float dt){
        Tutorial* tutorial = Tutorial::getInstance();
        try_set_visible(progress_panel, tutorial->get_show_progress_panel());
    };
    progress_panel->schedule(check_visible, SHORT_DELAY, "check_visible");
    check_visible(0.0f);

    ui::LoadingBar* loading_bar = dynamic_cast<ui::LoadingBar*>(progress_panel->getChildByName("goal_loadingbar"));
    loading_bar->setPercent(0.0f);

    this->upgrade_lbl = dynamic_cast<ui::Text*>(progress_panel->getChildByName("upgrade_lbl"));
    this->upgrade_lbl->setString("");
    set_aliasing(this->upgrade_lbl, true);

    auto progress_callback = [this]() {
            float coin_goal = scale_number(10.0f, (float)BUILDUP->get_target_building()->building_level, 10.5f);
            float percentage = BEATUP->get_total_coins() / coin_goal * 100;
            if (percentage >= 100.0f) {
                this->sidebar->toggle_buttons(this->sidebar->tab_building_btn, ui::Widget::TouchEventType::ENDED);
            }
    };
    bind_touch_ended(progress_panel, progress_callback);


    auto update_loading_bar = [this, loading_bar](float dt)
        {
            spBuilding target_building = BUILDUP->get_target_building();
			if (target_building->building_level == 15)
			{
				this->upgrade_lbl->setString("Max level!");
				try_set_visible(loading_bar, false);
				return;
			}

            float coin_goal = scale_number(10.0f, (float)target_building->building_level, 10.5f);
            float percentage = BEATUP->get_total_coins() / coin_goal * 100;
            loading_bar->setPercent(percentage);
			try_set_visible(loading_bar, true);

            if (percentage >= 100.0f) {
                this->upgrade_lbl->setString("Upgrade available!");
            }
            else {
                std::string short_name = target_building->short_name;
                this->upgrade_lbl->setString(short_name + " upgrade:");
            };

        };
    loading_bar->schedule(update_loading_bar, AVERAGE_DELAY, "loadingbar_update");
    update_loading_bar(0);


    //show progress panel
    auto harvester_progress_panel = this->getChildByName("harvester_progress_panel");

    try_set_visible(harvester_progress_panel, false);
    ui::LoadingBar* harvester_loading_bar = dynamic_cast<ui::LoadingBar*>(harvester_progress_panel->getChildByName("harvester_loadingbar"));
    auto update_harvester_loading_panel = [this, harvester_progress_panel, harvester_loading_bar](float dt) {
        if (BUILDUP->get_target_building()->name != "The Dump")
        {
            try_set_visible(harvester_progress_panel, false);
            harvester_loading_bar->setPercent(0.0f);
        }
        else
        {
            try_set_visible(harvester_progress_panel, true);
            auto harvestable = HarvestableManager::getInstance()->get_active_harvestable();
            if (harvestable)
            {
                float orig_percent = std::min(100.0f, std::floor(harvester_loading_bar->getPercent()));
                float new_percent = std::min(100.0f, std::floor(harvestable->get_click_ratio()*100.0f));

                if (orig_percent != new_percent)
                {
                    harvester_loading_bar->setPercent(new_percent);

                    auto particle = MagicEmitter::create("Sparks");
                    auto scale9_progbar_sprite = dynamic_cast<ui::Scale9Sprite*>(harvester_loading_bar->getVirtualRenderer());
                    auto bar_size = scale9_progbar_sprite->getSprite()->getContentSize();

                    auto pos = harvester_loading_bar->getPosition();
                    auto converted_pos = harvester_progress_panel->convertToWorldSpace(pos);
                    auto x = converted_pos.x + bar_size.width*scale9_progbar_sprite->getScaleX();
                    auto y = converted_pos.y + bar_size.height / 2;
                    particle->setPosition(x, y);

                    this->addChild(particle);
                }
            }
        };
    };
    harvester_progress_panel->schedule(update_harvester_loading_panel, SHORT_DELAY, "visibility_cb");
    update_harvester_loading_panel(0);
}

void BaseScene::create_building_choicelist()
{
    struct ChoiceConfig {
        std::string building_name;
        std::string node_name;
    };
    std::vector<ChoiceConfig> configs = {
        {
            "The Farm", "farm_node"
        }, {
            "The Arena", "arena_node"
        }, {
            "The Underscape", "underscape_node"
        }, {
            "The Marketplace", "marketplace_node"
        }, {
            "The Dump", "dump_node"
        }, {
            "The Workshop", "workshop_node"
        }, {
            "The Mine", "mine_node"
        }, {
            "The Graveyard", "graveyard_node"
        }, {
            "The Forest", "forest_node"
        }
    };


    Node* building_buttons = this->getChildByName("building_buttons");

    for (auto conf : configs)
    {
        spBuilding building = BUILDUP->city->building_by_name(conf.building_name);

        //get initial node from the prebuilt scene
        Node* building_node = building_buttons->getChildByName(conf.node_name);
        ui::Button* panel = (ui::Button*)building_node->getChildByName("building_panel");


        //set building name
        ui::Text* building_name = (ui::Text*)panel->getChildByName("building_name");
        building_name->setString(building->short_name);
        set_aliasing(building_name, true);

        //add building image
        ui::ImageView* building_image = (ui::ImageView*)panel->getChildByName("building_image");
        std::string img_path = "";
        if (building->name == "The Mine") {
            img_path = "mine_icon.png";
        }
        else if (building->name == "The Marketplace") {
            img_path = "marketplace_icon.png";
        }
        else if (building->name == "The Forest") {
            img_path = "forest_icon.png";
        }
        else if (building->name == "The Dump") {
            img_path = "dump_icon.png";
        }
        else if (building->name == "The Workshop") {
            img_path = "workshop_icon.png";
        }
        else if (building->name == "The Arena") {
            img_path = "sword.png";
        }
        else if (building->name == "The Graveyard") {
            img_path = "gravestone_icon.png";
        }
        else if (building->name == "The Underscape") {
            img_path = "pink_monster_icon.png";
        }
        else if (building->name == "The Farm") {
            img_path = "farm_icon.png";
        }
        else {
            img_path = "weapon_ice.png";
        };
        building_image->loadTexture(img_path, ui::Widget::TextureResType::PLIST);

        load_default_button_textures(panel);

        auto update_func = [panel, building, building_image, building_name, img_path](float dt)
        {
            auto target_building = BUILDUP->get_target_building();

            try_set_enabled(panel, target_building != building);

            if (building->is_storage_full_of_ingredients(building->punched_sub_type))
            {
                Color3B reddish = { 243, 162, 173 };
                try_set_node_color(panel, reddish);
                building_name->setString("FULL!");
            }
            else
            {
                try_set_node_color(panel, Color3B::WHITE);
                building_name->setString(building->short_name);
            }
        };
        building_node->schedule(update_func, AVERAGE_DELAY, "update_func");
        update_func(0);

        auto set_target_building = [building]() {
            BUILDUP->set_target_building(building);
        };
        bind_touch_ended(panel, set_target_building);
    };

    /// Build city button separately from the rest of the buildings
    //get initial node from the prebuilt scene
    Node* city_node = building_buttons->getChildByName("city_node");
    ui::Button* panel = (ui::Button*)city_node->getChildByName("building_panel");

    //check visibilty for tutorials
    Tutorial* tutorial = Tutorial::getInstance();
    auto check_visible = [building_buttons, tutorial](float dt){
        try_set_visible(building_buttons, tutorial->get_show_building_buttons());
    };
    building_buttons->schedule(check_visible, SHORT_DELAY, "check_visible");
    check_visible(0.0f);



    //set building name
    ui::Text* building_name = (ui::Text*)panel->getChildByName("building_name");
    building_name->setString("MGMT");
    set_aliasing(building_name, true);

    //add building image
    ui::ImageView* building_image = (ui::ImageView*)panel->getChildByName("building_image");
    std::string img_path = "buildingTiles_002.png";
    building_image->loadTexture(img_path, ui::Widget::TextureResType::PLIST);

    load_default_button_textures(panel);

    auto update_func = [panel, building_image, img_path](float dt)
    {
        auto target_building = BUILDUP->get_target_building();

        std::string tex_name;
        ui::Widget::TextureResType res_type;

        tex_name = img_path;
        res_type = ui::Widget::TextureResType::PLIST;

        ResourceData r_data = building_image->getRenderFile();
        if (r_data.file != tex_name || r_data.type != (int)res_type)
        {
            building_image->loadTexture(tex_name, res_type);
        }

        try_set_node_color(panel, Color3B::WHITE);
    };
    city_node->schedule(update_func, AVERAGE_DELAY, "update_func");
    update_func(0);

    auto touch_handler = [](){ GameDirector::switch_to_city_menu(); };
    bind_touch_ended(panel, touch_handler);
};

void BaseScene::create_popup_panel()
{
    auto panel_node = dynamic_cast<cocos2d::ui::Layout*>(this->getChildByName("popup_panel"));
    this->popup_panel = std::make_shared<PopupPanel>(panel_node);
    this->popup_panel->set_visible(false);
};

void BaseScene::SelectEmitter(int emitter)
{
	if (cur_node)
		cur_node->removeFromParent();

	HM_EMITTER hmEmitter=m_emitter[emitter];
	cur_node=MagicEmitter::create(hmEmitter);

	#ifdef MAGIC_3D
	cur_node->setCameraMask((unsigned short) CameraFlag::USER1);
	#endif

    if (cur_node == NULL)
    {
        CCLOG("magic particle's cur_node is empty, potentially missing particle effect files");
    }
    else
    {
        addChild(cur_node);
    }
}

Node* BaseScene::get_original_scene_from_editor()
{
    if (BaseScene::_harvest_scene_from_editor == NULL)
    {
        BaseScene::_harvest_scene_from_editor = get_prebuilt_node_from_csb("editor/scenes/base_scene.csb");
    };

    return BaseScene::_harvest_scene_from_editor;
}

struct cached_building_info_t
{
    res_count_t ing_count = -1.0;
    std::string ing_count_str = "";

    res_count_t storage_count = -1.0;
    std::string storage_str = "";
};

void BaseScene::create_info_panel()
{
    auto building = BUILDUP->get_target_building();

    ui::Layout* arena_kill_panel = dynamic_cast<ui::Layout*>(this->getChildByName("arena_kill_panel"));
    ui::Text* arena_kill_lbl = dynamic_cast<ui::Text*>(arena_kill_panel->getChildByName("arena_kill_lbl"));
    set_aliasing(arena_kill_lbl);

    auto update_info_display = [this, arena_kill_panel, arena_kill_lbl](float dt){
        auto target_building = BUILDUP->get_target_building();
        if (target_building->name == "The Arena") {
            try_set_visible(arena_kill_panel, true);
            res_count_t kills = (res_count_t)GameLogic::getInstance()->get_total_kills(); //comes through as int though
            arena_kill_lbl->setString("Kills: "+beautify_double(kills));
        } else if (target_building->name == "The Workshop") {
            try_set_visible(arena_kill_panel, true);
            if (this->target_recipe != NULL) {
                arena_kill_lbl->setString(this->target_recipe->name);
            }
            else {
                arena_kill_lbl->setString("Choose a recipe");
            }
        } else if (target_building->name == "The Marketplace") {
            try_set_visible(arena_kill_panel, true);
            if (time(0) % 10 > 5) {
                res_count_t max_coin_storage = BEATUP->get_max_coin_storage();
                arena_kill_lbl->setString("Max Gold: " + beautify_double(max_coin_storage));
            } else
            {
                res_count_t city_investment = GameLogic::getInstance()->get_city_investment();
                arena_kill_lbl->setString("Investments: " + beautify_double(city_investment));
            }
        }
        // else if (target_building->name == "The Mine") { //TODO access miner depth from outside of minerscene
        //     arena_kill_panel->setVisible(true);
        //     arena_kill_lbl->setString("Depth: XXX");
        else {
            try_set_visible(arena_kill_panel, false);
        }
    };
    arena_kill_panel->schedule(update_info_display, AVERAGE_DELAY, "update_info_display");
    update_info_display(0);


    ui::Layout* building_info_panel = dynamic_cast<ui::Layout*>(this->getChildByName("building_info_panel"));
    //check visibilty for tutorials
    Tutorial* tutorial = Tutorial::getInstance();
    auto check_visible = [building_info_panel, tutorial](float dt){
        try_set_visible(building_info_panel, tutorial->get_show_building_info());
    };
    building_info_panel->schedule(check_visible, SHORT_DELAY, "check_visible");
    check_visible(0.0f);

    auto building_name = dynamic_cast<ui::Text*>(building_info_panel->getChildByName("building_name"));
    set_aliasing(building_name);
    auto update_building_name = [building_name](float dt){
        building_name->setString(BUILDUP->get_target_building()->name);
    };
    this->schedule(update_building_name, AVERAGE_DELAY, "building_name_update");
    update_building_name(0);

    auto ing_count = dynamic_cast<ui::Text*>(building_info_panel->getChildByName("ingredient_count"));
    set_aliasing(ing_count);
    cached_building_info_t* cached_building_info = new cached_building_info_t();
    ing_count->setUserData(static_cast<void*>(cached_building_info));

    auto update_ing_count = [ing_count](float dt)
    {
        cached_building_info_t* cached = static_cast<cached_building_info_t*>(ing_count->getUserData()); //cant use dynamic_cast on void*
        if (cached == NULL)
        {
            CCLOG("cached data needs to be initialized on create on widget");
        }

        spBuilding building = BUILDUP->get_target_building();

        //whether to beautify storage and ing counts
        res_count_t storage_space = building->get_storage_space();
        if (cached->storage_count != storage_space)
        {
            cached->storage_count = storage_space;
            cached->storage_str = beautify_double(storage_space);
        }

        res_count_t ing_count_val = BUILDUP->count_ingredients(building->punched_sub_type);
        if (cached->ing_count != ing_count_val)
        {
            cached->ing_count = ing_count_val;
            cached->ing_count_str = beautify_double(ing_count_val);
        }

        char buffer[50];
        sprintf(buffer, "%s/%s", cached->ing_count_str.c_str(), cached->storage_str.c_str());
        ing_count->setString(buffer);

        ing_count->setUserData(cached);
    };
    this->schedule(update_ing_count, AVERAGE_DELAY, "ing_count_update");
    update_ing_count(0);

    auto harvester_count = dynamic_cast<ui::Text*>(building_info_panel->getChildByName("harvester_count"));
    set_aliasing(harvester_count);
    auto update_harvester_count = [harvester_count](float dt)
    {
        spBuilding building = BUILDUP->get_target_building();

        std::stringstream ss;
        res_count_t total_active = 0;
        for (auto&& types_to_count : building->harvesters)
        {
            total_active += types_to_count.second;
        };

        if (total_active >= 1){
            res_count_t harvester_output = building->get_total_harvester_output();
            ss << "Harvesters: " << beautify_double(total_active) << "\n " << beautify_double(harvester_output) << "/sec";
            harvester_count->setString(ss.str());
            try_set_visible(harvester_count, true);
        }
        else {
            try_set_visible(harvester_count, false);
        }
    };
    this->schedule(update_harvester_count, AVERAGE_DELAY, "harvester_count_update");
    update_harvester_count(0);

    auto salesmen_count = dynamic_cast<ui::Text*>(building_info_panel->getChildByName("salesmen_count"));
    set_aliasing(salesmen_count);
    auto update_salesmen_count = [salesmen_count](float dt)
    {
        spBuilding building = BUILDUP->get_target_building();

        std::stringstream ss;
        res_count_t total_active = 0;
        for (auto&& types_to_count : building->salesmen)
        {
            total_active += types_to_count.second;
        };
        if (total_active >= 1){
            res_count_t salesmen_output = building->get_total_salesmen_output();
            ss << "Salesmen: " << beautify_double(total_active) << "\n " << beautify_double(salesmen_output) << "/sec";
            salesmen_count->setString(ss.str());
            try_set_visible(salesmen_count, true);
        }
        else {
            try_set_visible(salesmen_count, false);
        }
    };
    this->schedule(update_salesmen_count, AVERAGE_DELAY, "salesmen_count_update");
    update_salesmen_count(0);
};

void BaseScene::create_username_input()
{
    auto building_panel = this->getChildByName("player_info_panel");

    ui::TextField* username_input = dynamic_cast<ui::TextField*>(building_panel->getChildByName("username_input"));

    username_input->setTextHorizontalAlignment(TextHAlignment::CENTER);
    ui::UICCTextField* renderer = dynamic_cast<ui::UICCTextField*>(username_input->getVirtualRenderer());
    cocos2d::Label* label = dynamic_cast<Label*>(renderer);
    set_aliasing(label);
    label->setTTFConfig(NuItem::ttf_config);

    username_input->setPlaceHolderColor(Color4B::GREEN);

    auto username = DataManager::get_string_from_data("username");
    username_input->setString(username);

    auto is_bad_character = [](char character){
        return !(std::isalnum(character) || character == '_');
    };
    auto clean_username = [is_bad_character, this](std::string username) {
        username.erase(std::remove_if(username.begin(), username.end(), is_bad_character), username.end());
        if (username.length() > 16){
            username.resize(16);

            TextBlobModal* modal = TextBlobModal::create();
            modal->set_title("Name too long!");
            modal->set_body("Names can only be up to 16 characters without special characters of any kind");
            this->addChild(modal);
        };
        return username;
    };

    auto textfield_listener = [username_input, clean_username](Ref* target, ui::TextField::EventType evt)
        {
            if (evt == ui::TextField::EventType::ATTACH_WITH_IME ||
                evt == ui::TextField::EventType::DETACH_WITH_IME)
            {
                std::string text = username_input->getString();
                std::string cleaned_username = clean_username(text);
                username_input->setString(cleaned_username);

                DataManager::set_string_from_data("username", cleaned_username);
            }
        };
    username_input->addEventListener(textfield_listener);
}

struct player_hp_cache_t {
    res_count_t last_cur_hp;
    res_count_t last_max_hp;
};

void BaseScene::create_player_info_panel()
{
    auto building = BUILDUP->get_target_building();

    ui::Layout* player_info_panel = dynamic_cast<ui::Layout*>(this->getChildByName("player_info_panel"));

    //check visibilty for tutorials
    Tutorial* tutorial = Tutorial::getInstance();
    auto check_visible = [player_info_panel, tutorial](float dt){
        try_set_visible(player_info_panel, tutorial->get_show_player_info());
    };

    player_info_panel->schedule(check_visible, SHORT_DELAY, "check_visible");
    check_visible(0.0f);

    ui::Text* player_info_lbl = dynamic_cast<ui::Text*>(player_info_panel->getChildByName("player_info_lbl"));
    set_aliasing(player_info_lbl, true);

    auto manual_heal = [player_info_panel, this]()
    {
        auto gold = BEATUP->get_total_coins();
        int heal_cost = 10;
        if (gold > heal_cost)
        {
            auto health = BUILDUP->fighter->attrs->health;
            if (health->is_full() == false)
            {
                health->add_to_current_val(5);
                BEATUP->add_total_coins(-heal_cost);
            }
        }
        else
        {
            auto player_gold_lbl = dynamic_cast<ui::Text*>(player_info_panel->getChildByName("player_gold_lbl"));
            player_gold_lbl->runAction(FShake::actionWithDuration(0.05f, 10));
        }
    };
    bind_touch_ended(player_info_panel, manual_heal);

    auto player_gold_lbl = dynamic_cast<ui::Text*>(player_info_panel->getChildByName("player_gold_lbl"));
    set_aliasing(player_gold_lbl, true);
    auto player_hp_lbl = dynamic_cast<ui::Text*>(player_info_panel->getChildByName("player_hp_lbl"));
    set_aliasing(player_hp_lbl, true);

    ui::Button* achievement_btn = dynamic_cast<ui::Button*>(player_info_panel->getChildByName("achievement_btn"));
    prep_button(achievement_btn);
    bind_touch_ended(achievement_btn, [](){ GameDirector::switch_to_achievement_menu(); });

    //hp cache, used for animating changes
    player_hp_cache_t* hp_cache = new player_hp_cache_t(); //TODO make sure this gets deleted at some point
    auto hp = BUILDUP->fighter->attrs->health;
    hp_cache->last_cur_hp = hp->current_val;
    hp_cache->last_max_hp = hp->max_val;
    player_hp_lbl->setUserData(hp_cache);

    //check visibilty for tutorials
    auto check_hp_visible = [player_hp_lbl, tutorial](float dt){
        try_set_visible(player_hp_lbl, tutorial->get_show_player_hp_lbl());
    };
    player_hp_lbl->schedule(check_hp_visible, SHORT_DELAY, "check_hp_visible");
    check_visible(0.0f);

    auto update_player_info_lbls = [player_gold_lbl, player_hp_lbl, hp_cache](float dt){
        //set gold
        std::stringstream coin_ss;
        res_count_t total_coins = BEATUP->get_total_coins();
        auto gold = std::round(total_coins);
        coin_ss << "$" << beautify_double(gold);
        std::string coin_msg = coin_ss.str();
        player_gold_lbl->setString(coin_msg);

        //set hp
        std::stringstream hp_ss;
        auto hp = BUILDUP->fighter->attrs->health;

        if (hp->current_val != hp_cache->last_cur_hp)
        {
            hp_ss << "HP: " << hp->current_val << "/" << hp->max_val;
            player_hp_lbl->setString(hp_ss.str());

                if (hp->is_empty()) {
                    try_set_node_color(player_hp_lbl, Color3B::RED);
                } else {
                    player_hp_lbl->stopAllActions();
                    Color3B start_color = Color3B::RED;
                    if (hp->current_val > hp_cache->last_cur_hp)
                    {
                        start_color = Color3B::GREEN;
                    };
                    animate_flash_action(player_hp_lbl, 0.15f, 1.2f, 1.0f, start_color, Color3B::WHITE);
                }

            hp_cache->last_cur_hp = hp->current_val;
        };

    };
    this->schedule(update_player_info_lbls, AVERAGE_DELAY, "update_player_info_lbls");
    update_player_info_lbls(0);

};

void BaseScene::create_inventory_listview()
{
    ui::ListView* inventory_basic_listview = dynamic_cast<ui::ListView*>(this->getChildByName("inventory_basic_listview"));
    ui::ListView* inventory_advanced_listview = dynamic_cast<ui::ListView*>(this->getChildByName("inventory_advanced_listview"));

    //add padding because they bleed together otherwise
    float margin = 5.0f;
    inventory_basic_listview->setItemsMargin(margin);
    inventory_advanced_listview->setItemsMargin(margin);

    auto update_listview = [this, inventory_basic_listview, inventory_advanced_listview](float dt)
    {
        typedef std::pair<Ingredient::SubType, res_count_t> type_count_pair;
        auto order_by_count = [](type_count_pair left, type_count_pair right)
        {
            return left.second > right.second;
        };

        std::vector<type_count_pair> ingredient_counts;
        mistIngredient& city_ingredients = BUILDUP->get_all_ingredients();

        res_count_t _def = -1;
        for (auto&& ts : Ingredient::basic_ingredients)
        {
            Ingredient::SubType ing_type = ts;
            auto count = map_get(city_ingredients, ing_type, _def);
            ingredient_counts.push_back({ ts, count });
        }
        for (auto&& ts : Ingredient::advanced_ingredients)
        {
            Ingredient::SubType ing_type = ts;
            auto count = map_get(city_ingredients, ing_type, _def);
            ingredient_counts.push_back({ ts, count });
        }

        std::sort(
            ingredient_counts.begin(),
            ingredient_counts.end(),
            order_by_count
        );

        for (auto&& type_and_count : ingredient_counts)
        {
            Ingredient::SubType ing_type = type_and_count.first;
            res_count_t& ing_count = type_and_count.second;

            std::string str_type = Ingredient::type_to_string(ing_type);

            auto zero_ingredients = ing_count < 0;

            //if the child already exists, and has 0 or more ingredients, move to next ing type
            ui::Button* existing_basic_node = dynamic_cast<ui::Button*>(inventory_basic_listview->getChildByName(str_type));
            ui::Button* existing_advanced_node = dynamic_cast<ui::Button*>(inventory_advanced_listview->getChildByName(str_type));
            bool existing_node = existing_basic_node || existing_advanced_node;
            if (existing_node)
            {
                if (zero_ingredients == false)
                {
                    continue;
                }
            }
            //dont build the ingredient button if for no ingredients
            else if (existing_node == NULL && zero_ingredients)
            {
                continue;
            }

            //from the inv button take the panel, remove it from the button,
            //clone it and use the clone
            auto raw_node = get_prebuilt_node_from_csb("editor/buttons/inventory_button.csb");
            auto orig_item_panel = dynamic_cast<ui::Button*>(raw_node->getChildByName("item_panel")); //TODO do we need to clone, why not use it
            orig_item_panel->removeFromParent();

            auto new_item_panel = dynamic_cast<ui::Button*>(orig_item_panel->clone());
            load_default_button_textures(new_item_panel);
            new_item_panel->setName(str_type);

            float orig_scale = new_item_panel->getScale();
            auto scale_to = ScaleTo::create(0.15f, orig_scale);
            new_item_panel->setScale(0.0f);
            new_item_panel->runAction(EaseBackOut::create(scale_to));

            //set aliasing on font
            auto item_name_lbl = dynamic_cast<ui::Text*>(new_item_panel->getChildByName("item_name_lbl"));
            set_aliasing(item_name_lbl, true);
            auto item_count_lbl = dynamic_cast<ui::Text*>(new_item_panel->getChildByName("item_count_lbl"));
            set_aliasing(item_count_lbl, true);

            //add ingredient image
            ui::ImageView* item_img = (ui::ImageView*)new_item_panel->getChildByName("item_img");
            IngredientData res_data(str_type);
            item_img->loadTexture(res_data.get_img_large());
            set_aliasing(item_img, true);

            auto on_touch_cb = [ing_type, this, new_item_panel]() {
                auto alert = this->create_ingredient_detail_alert(ing_type);
                this->addChild(alert, 10);

                //animate
                Vec2 start_pos = new_item_panel->getTouchEndPosition();
                alert->setPosition(start_pos);

                alert->setScale(0);

                float duration = 0.25f;
                auto scale = EaseOut::create(ScaleTo::create(duration, 1.0f, 1.0f), 1.5f);

                Vec2 end_pos = get_center_pos();
                auto move = MoveTo::create(duration, end_pos);

                Sequence* seq = Sequence::create(EaseBackOut::create(Spawn::createWithTwoActions(move, scale)), NULL);
                alert->runAction(seq);
            };
            bind_touch_ended(new_item_panel, on_touch_cb);

            auto type_str = Ingredient::type_to_string(ing_type);
            item_name_lbl->setString(type_str);

            auto update_lbl_cb = [new_item_panel, ing_type](float)
            {
                mistIngredient& city_ingredients = BUILDUP->get_all_ingredients();

                res_count_t _def = -1;
                Ingredient::SubType ing_t = ing_type;
                res_count_t count = map_get(city_ingredients, ing_t, _def);

                ui::Text* item_count_lbl = dynamic_cast<ui::Text*>(new_item_panel->getChildByName("item_count_lbl"));
                item_count_lbl->setString(beautify_double(count));

            };

            update_lbl_cb(0); //fire once immediately
            new_item_panel->schedule(update_lbl_cb, MID_DELAY, "item_name_lbl_update");

            auto&& advanced_ingredients = Ingredient::advanced_ingredients;
            bool in_advanced = std::find(advanced_ingredients.begin(), advanced_ingredients.end(), ing_type) != advanced_ingredients.end();
            if (!in_advanced)
            {
                inventory_basic_listview->addChild(new_item_panel);
                inventory_basic_listview->requestDoLayout();
            }
            else
            {
                inventory_advanced_listview->addChild(new_item_panel);
                new_item_panel->setColor({ 200, 200, 200 });
                inventory_advanced_listview->requestDoLayout();
            }
        };
    };

    update_listview(0.0f); //why doesnt this do anything
    inventory_basic_listview->schedule(update_listview, AVERAGE_DELAY, "update_listview");
};

void BaseScene::create_shop_listview()
{

    this->sidebar = std::make_shared<SideListView>(this, BUILDUP->get_target_building());

    this->sidebar->setup_shop_listview_as_harvesters();
    this->sidebar->setup_detail_listview_as_recipes();
    this->sidebar->setup_building_listview_as_upgrades();
    this->sidebar->setup_powers_listview_as_powers();
};

bool HarvestScene::init()
{
    BaseScene::init();

	this->cur_node=NULL;

    this->target_recipe = NULL;

    this->add_harvestable();

    //color layer based on building level
    LayerColor* layer_color = LayerColor::create(Color4B(255, 0, 0, 120));
    layer_color->setName("building_level_color");
    this->addChild(layer_color, -100); //set it behind everything

    this->_layer_building_level = -99; //using some bogus number

    //update layer color when the building's level has changed
    std::function<void(float)> update_layer_color = [this, layer_color](float dt)
    {
        spBuilding target_building = BUILDUP->get_target_building();
        if (target_building->building_level == this->_layer_building_level)
        {
            return;
        }

        this->_layer_building_level = target_building->building_level;

        std::map<int, Color3B> color_map ={
            {1,  {0,   0,   0}},
            {2,  {25,  25,  25}},
            {3,  {50,  50,  50}},
            {4,  {75,  75,  75}},
            {5,  {100, 100, 100}},
            {6,  {125, 125, 125}},
            {7,  {150, 150, 150}},
            {7,  {175, 175, 175}},
            {8,  {200, 200, 200}},
            {9,  {225, 225, 225}},
            {10, {250, 250, 250}},
            {11, {251, 251, 251}},
            {12, {252, 252, 252}},
            {13, {253, 253, 253}},
            {14, {254, 254, 254}},
            {15, {255, 255, 255}},
            {16, {255, 0,   0  }},
            {17, {255, 255, 0  }},
            {18, {255, 255, 255}}
        };

        Color3B _def = Color3B::BLACK;
        // auto color = map_get(color_map, this->_layer_building_level, _def);
        try_set_node_color(layer_color, _def);


    };
    this->schedule(update_layer_color, AVERAGE_DELAY, "update_layer_color");
    update_layer_color(0.0f);

    this->setOnEnterCallback([this]()
    {
        if (GameLogic::getInstance()->is_loaded == false)
        {
            //animate the interface loading up
            float orig_scale = this->getScale();
            auto scale_to = ScaleTo::create(0.3f, orig_scale);

            try_set_visible(this, false);
            this->setScale(0.0f);

            this->runAction(Sequence::create(
                DelayTime::create(0.25f),
                Show::create(),
                EaseBackOut::create(scale_to),
                cocos2d::CallFunc::create([this]()
                {
                    auto loading_bg = this->getParent()->getChildByName("loading_bg");
                    if (loading_bg) {
                        loading_bg->removeFromParent();
                    }
                }),
                NULL
                ));

            //load the game from json
            GameLogic::getInstance()->load_game();
        }
    });

    //autosave every 30s
    this->autosave_clock = new Clock(30.0f);

    auto toggle_visible_listviews_btn = dynamic_cast<ui::Button*>(this->getChildByName("toggle_visible_listviews_btn"));
    prep_button(toggle_visible_listviews_btn);
    bind_touch_ended(toggle_visible_listviews_btn, [this](){
        this->toggle_ingredient_listviews();
    });

    auto tutorial_sidebar_panel = this->getChildByName("tutorial_sidebar_panel")->getChildByName("tutorial_sidebar_panel");
    tutorial_sidebar_panel->setLocalZOrder(-100); //set tutorial panel behind everything

    return true;
}

void HarvestScene::update(float dt)
{
    BaseScene::update(dt);

    spBuilding target_building = BUILDUP->get_target_building();

    Harvestable* harvestable = HarvestableManager::getInstance()->get_active_harvestable();
    if (!harvestable) {
        //FIXME hack when transitioning, the root scene is the transition scene instead of this,
        // so get_active_harvestable is always null because it can't find HarvestScene
        cocos2d::Scene* root_scene = cocos2d::Director::getInstance()->getRunningScene();
        auto harvest_scene = dynamic_cast<HarvestScene*>(root_scene->getChildByName("HarvestScene"));
        if (harvest_scene != NULL)
        {
            this->add_harvestable();
        }
    }
    else if (target_building != harvestable->building) {
        harvestable->removeFromParent();
    };

    this->autosave_clock->update(dt);
    if (this->autosave_clock->passed_threshold())
    {
        this->autosave_clock->reset();

        CCLOG("Autosaving");
        GameLogic::save_all();
    };


}

ui::Widget* BaseScene::create_ingredient_detail_alert(Ingredient::SubType ing_type)
{
    //make sure one doesn't already exist first
    if (this->getChildByName("inventory_detail_panel")) {
        this->getChildByName("inventory_detail_panel")->removeFromParent();
    };

    std::string str_type = Ingredient::type_to_string(ing_type);
    IngredientData res_data(str_type);

    auto raw_node = get_prebuilt_node_from_csb("editor/details/inventory_detail.csb");

    auto alert_panel = dynamic_cast<ui::Layout*>(raw_node->getChildByName("inventory_detail_panel"));
    alert_panel->removeFromParent();

    //background color
    LayerColor* layer_color = cocos2d::LayerColor::create({30, 144, 255, 85});
    layer_color->setName("alert_panel_bg");
    layer_color->setOpacity(0);
    layer_color->runAction(cocos2d::FadeTo::create(0.15f, 85));

    //invisible layout to tap to close
    ui::Layout* exit_layout = ui::Layout::create();
    exit_layout->setName("alert_panel_exit_layout");
    exit_layout->setAnchorPoint(Vec2::ZERO);
    exit_layout->setContentSize({ 1000, 1000 });
    exit_layout->setTouchEnabled(true);
    this->addChild(exit_layout, 9); //9 because alert_panel is added as 10 elsewhere
    exit_layout->addChild(layer_color);

    ui::Layout* close_panel = dynamic_cast<ui::Layout*>(alert_panel->getChildByName("close_panel"));
    auto cb = [this](Ref* target, ui::Widget::TouchEventType type) {
        if (type == ui::Widget::TouchEventType::ENDED)
        {

            do_vibrate(5);

            auto end_pos = static_cast<ui::Widget*>(target)->getTouchEndPosition();
            auto move = MoveTo::create(0.25f, end_pos);

            auto scale_to = ScaleTo::create(0.25f, 0);
            auto fade_out = FadeOut::create(0.1f);
            auto spawn = Spawn::create(
                scale_to,
                fade_out,
                move,
                NULL
            );
            auto layer_color = this->getChildByName("alert_panel_bg");
            if (layer_color){
                layer_color->runAction(cocos2d::Sequence::createWithTwoActions(
                    FadeOut::create(0.1f),
                    RemoveSelf::create()
                ));
            };

            auto alert_panel = this->getChildByName("inventory_detail_panel");
            if (alert_panel){
                Sequence* seq = Sequence::create(spawn, RemoveSelf::create(), NULL);
                alert_panel->runAction(seq);
            };

            auto exit_layout = this->getChildByName("alert_panel_exit_layout");
            if (exit_layout){
                exit_layout->removeFromParent();
            };

        };
    };
    close_panel->addTouchEventListener(cb);
    exit_layout->addTouchEventListener(cb);

    auto resource_name = dynamic_cast<ui::Text*>(alert_panel->getChildByName("resource_name"));
    std::string res_name = Ingredient::type_to_string(ing_type);
    resource_name->setString(res_name);

    IngredientData ing_data(res_name);
    auto resource_icon = dynamic_cast<ui::ImageView*>(alert_panel->getChildByName("resource_icon"));
    resource_icon->loadTexture(ing_data.get_img_large());

    auto resource_description = dynamic_cast<ui::Text*>(alert_panel->getChildByName("resource_description"));
    resource_description->setString(res_data.get_description());

    auto count_desc = dynamic_cast<ui::Text*>(alert_panel->getChildByName("count_desc"));
    auto count_lbl = dynamic_cast<ui::Text*>(alert_panel->getChildByName("count_lbl"));

    alert_panel->schedule([count_lbl, ing_type](float) {
        auto it = ing_type;
        auto& all_ing = BUILDUP->get_all_ingredients();
        res_count_t _def = 0.0;
        res_count_t count = map_get(all_ing, it, _def);
        count_lbl->setString(beautify_double(count));
    }, AVERAGE_DELAY, "alert_count_update");

    res_count_t coins_gained_per = Ingredient::type_to_value.at(ing_type);
    auto create_sell_button = [this, alert_panel, ing_type, coins_gained_per](std::string name, res_count_t amount_sold, float percent_sold)
    {
        ui::Button* sell_btn = dynamic_cast<ui::Button*>(alert_panel->getChildByName(name));
        prep_button(sell_btn);
        sell_btn->getTitleRenderer()->enableOutline({ 0x0a, 0x0a, 0x0a, 255 }, 3);

        auto manual_sell = [this, alert_panel, sell_btn, ing_type, coins_gained_per, amount_sold, percent_sold](){
            mistIngredient& city_ingredients = BUILDUP->get_all_ingredients();

            res_count_t _def = 0;
            auto it = ing_type;
            res_count_t num_sellable = map_get(city_ingredients, it, _def);

            if (num_sellable >= 1)
            {
                res_count_t USE_ABSOLUTE = -1;

                res_count_t original_num_to_sell;
                if (amount_sold != USE_ABSOLUTE) //if amount sold is not -1, use absolute values instead
                {
                    original_num_to_sell = std::min(num_sellable, amount_sold);
                }
                else
                {
                    original_num_to_sell = std::min(num_sellable, num_sellable*percent_sold);
                }

                //make sure enough coin space for sale
                res_count_t coin_storage_left = BEATUP->get_coin_storage_left();
                res_count_t adjusted_num_to_sell = std::min(std::floor(coin_storage_left / coins_gained_per), original_num_to_sell);
                res_count_t final_coins_gained = adjusted_num_to_sell * coins_gained_per;

                BEATUP->add_total_coins(final_coins_gained);

                BUILDUP->remove_shared_ingredients_from_all(ing_type, adjusted_num_to_sell);

                Vec2 pos = sell_btn->getTouchEndPosition();
                cocos2d::Vec2 floating_start_pos = sell_btn->getParent()->convertToNodeSpace(pos);

                Color3B text_color;
                std::stringstream ss;

                if (final_coins_gained != 0)
                {
                    text_color = Color3B::GREEN;
                    ss << "+$" << beautify_double(final_coins_gained);
                }
                else
                {
                    text_color = Color3B::RED;
                    ss << "Max coins stored\nIncrease coin storage\n (The Marketplace)";
                }

                std::string message = ss.str();

                auto floating_label = FloatingLabel::createWithTTF(message, DEFAULT_FONT, 30);
                floating_label->setTextColor(Color4B(text_color));

                pos.x += cocos2d::rand_minus1_1()*20.0f;
                pos.y += cocos2d::rand_minus1_1()*20.0f;

#ifdef __ANDROID__
                pos.y += 75.0f; //dont get hidden by finger
#endif

                floating_label->setPosition(floating_start_pos);
                floating_label->setAnchorPoint(Vec2::ANCHOR_MIDDLE_BOTTOM);
                floating_label->do_float();

                sell_btn->getParent()->addChild(floating_label);

                auto player_gold_lbl = this->getChildByName("player_info_panel")->getChildByName("player_gold_lbl");
                animate_flash_action(player_gold_lbl, 0.1f, 1.2f, 1.0f, text_color);

                do_vibrate(10);
            }
        };
        bind_touch_ended(sell_btn, manual_sell);

        sell_btn->schedule([sell_btn, this, ing_type, amount_sold](float){
            mistIngredient& city_ingredients = BUILDUP->get_all_ingredients();
            res_count_t _def = 0;
            auto it = ing_type;
            if (map_get(city_ingredients, it, _def) < std::max(amount_sold, 1.0L))
            {
                //this doesnt let the button accept touches, so we need to subclass widget or something to fix it
                try_set_enabled(sell_btn, false);
            }
            else
            {
                try_set_enabled(sell_btn, true);
            }
        }, AVERAGE_DELAY, "sell_btn_state_cb");
    };

    create_sell_button("sell_1_btn", 1, 0.0f);
    create_sell_button("sell_10_btn", 10, 0.0f);
    create_sell_button("sell_100_btn", -1, 0.25f);
    create_sell_button("sell_1000_btn", -1, 0.50f);

    auto value_lbl = dynamic_cast<ui::Text*>(alert_panel->getChildByName("value_lbl"));
    auto update_value_lbl = [coins_gained_per, alert_panel, value_lbl](float dt){
        auto cg = coins_gained_per;
        std::stringstream value_ss;
        value_ss << "for " << beautify_double(cg) << "$ each";
        value_lbl->setString(value_ss.str().c_str());
    };
    value_lbl->schedule(update_value_lbl, AVERAGE_DELAY, "update_value_lbl");
    update_value_lbl(0);

    alert_panel->setPosition(get_center_pos());

    alert_panel->setName("inventory_detail_panel");

    return alert_panel;
};

void HarvestScene::add_harvestable()
{
    Harvestable* harvestable = NULL;
    auto target_building = BUILDUP->get_target_building();

    if (target_building->name == "The Mine") {
        harvestable = MiningHarvestable::create();
    } else if (target_building->name == "The Forest") {
        harvestable = TreeHarvestable::create();
    } else if (target_building->name == "The Dump") {
        harvestable = DumpsterHarvestable::create();
    } else if (target_building->name == "The Workshop") {
        harvestable = CraftingHarvestable::create(target_recipe);
    } else if (target_building->name == "The Arena") {
        harvestable = FightingHarvestable::create();
    } else if (target_building->name == "The Graveyard") {
        harvestable = GraveyardHarvestable::create();
    } else if (target_building->name == "The Underscape") {
        harvestable = UndeadHarvestable::create();
    } else if (target_building->name == "The Marketplace") {
        harvestable = MarketHarvestable::create();
    } else if (target_building->name == "The Farm") {
        harvestable = FarmingHarvestable::create();
    } else {
        harvestable = Harvestable::create();
    };

    float orig_scale = harvestable->getScale();
    harvestable->setScale(0);
    auto scale_to = ScaleTo::create(0.2f, orig_scale);
    harvestable->runAction(cocos2d::EaseBackOut::create(scale_to));

    //we use the object in the scene instead of tracking it like this
    //this->harvestable_manager->set_active_harvestable(harvestable);

    this->addChild(harvestable);
};

void HarvestScene::toggle_ingredient_listviews(bool remove_children)
{
    auto toggle_visible_listviews_btn = dynamic_cast<ui::Button*>(this->getChildByName("toggle_visible_listviews_btn"));
    ui::ListView* inventory_advanced_listview = dynamic_cast<ui::ListView*>(this->getChildByName("inventory_advanced_listview"));
    inventory_advanced_listview->setVisible(!inventory_advanced_listview->isVisible());

    ui::ListView* inventory_basic_listview = dynamic_cast<ui::ListView*>(this->getChildByName("inventory_basic_listview"));
    inventory_basic_listview->setVisible(!inventory_basic_listview->isVisible());

    //also clear items out so that they can animate basic in #TODO figure out if this causes lag
    if (inventory_basic_listview->isVisible()) {
        if (remove_children) inventory_basic_listview->removeAllItems();
        toggle_visible_listviews_btn->setTitleText("Basic");
    } else {
        if (remove_children) inventory_advanced_listview->removeAllItems();
        toggle_visible_listviews_btn->setTitleText("Advanced");
    };
};

Node* HarvestScene::show_ingredient_listview_for_ing_type(Ingredient::SubType ing_type)
{
    auto basic_listview = this->getChildByName("inventory_basic_listview");
    auto advanced_listview = this->getChildByName("inventory_advanced_listview");

    //get visible listview
    auto visible_listview = basic_listview->isVisible() ? basic_listview : advanced_listview;
    auto hidden_listview = advanced_listview->isVisible() ? advanced_listview : basic_listview;

    if (visible_listview)
    {
        std::string type_str = Ingredient::type_to_string(ing_type);

        //find matching ing_panel in listview
        auto ing_panel = visible_listview->getChildByName(type_str);

        //if found a panel its the current visible listview
        if (ing_panel)
        {
            return visible_listview;
        }
        //if not found the panel is on the invisible one, make it visible and return that
        else
        {
            this->toggle_ingredient_listviews(false); //needs to not destroy children, so the panel'll exist after
            return hidden_listview;
        };
    }

    return NULL;
};

Node* HarvestScene::get_visible_ing_panel(Ingredient::SubType ing_type)
{
    auto basic_listview = this->getChildByName("inventory_basic_listview");
    auto advanced_listview = this->getChildByName("inventory_advanced_listview");

    //get visible listview
    auto visible_listview = basic_listview->isVisible() ? basic_listview : advanced_listview;
    if (visible_listview)
    {
        std::string string_type = Ingredient::type_to_string(ing_type);

        auto ing_panel = visible_listview->getChildByName(string_type);
        if (ing_panel)
        {
            return ing_panel;
        }
    }

    return NULL;
};

void HarvestScene::spawn_floating_label_for_ing_type(
        Ingredient::SubType ing_type,
        std::string message
    )
{
    auto ing_panel = this->get_visible_ing_panel(ing_type);
    if (ing_panel)
    {
        auto floating_label = FloatingLabel::createWithTTF(message, "pixelmix.ttf", 25);
        floating_label->enableOutline(cocos2d::Color4B::BLACK, 2);
        this->addChild(floating_label);
        floating_label->do_float(50, 1, 50, 10, 1);

        cocos2d::Vec2 pos = {
            ing_panel->getContentSize().width / 2,
            ing_panel->getContentSize().height
        };
        floating_label->setPosition(ing_panel->convertToWorldSpace(pos));
    }
};
