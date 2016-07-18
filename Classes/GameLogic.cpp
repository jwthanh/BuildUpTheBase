#include "GameLogic.h"

#include "Beatup.h"
#include "HouseBuilding.h"
#include "DataManager.h"
#include "NuMenu.h"
#include "Serializer.h"
#include "Util.h"
#include "cocostudio/ActionTimeline/CSLoader.h"

GameLogic* GameLogic::_instance = NULL;

GameLogic::GameLogic()
{

};

void GameLogic::post_load()
{
    CCLOG("loading game on startup, this should only happen once");
    for (spBuilding building : BUILDUP->city->buildings)
    {
        auto bldg_serializer = BuildingSerializer("test_building.json", building);
        bldg_serializer.load();
    };

    std::stringstream ss;
    auto hours_since_last_login = BEATUP->hours_since_last_login();


    auto original_ingredients = BUILDUP->get_all_ingredients();
    auto seconds = std::chrono::duration_cast<std::chrono::seconds>(hours_since_last_login);
    BUILDUP->city->update(seconds.count());
    auto new_ingredients = BUILDUP->get_all_ingredients();

    for (auto mist_ing : new_ingredients)
    {
        Ingredient::SubType ing_type = mist_ing.first;
        res_count_t new_count = mist_ing.second;

        res_count_t old_count = map_get(original_ingredients, ing_type, 0);

        ss << "gained " << beautify_double(new_count - old_count) << " " << Ingredient::type_to_string(ing_type) << std::endl;
    }

    auto scene = cocos2d::Director::getInstance()->getRunningScene()->getChildByName("HarvestScene");

    auto inst = cocos2d::CSLoader::getInstance();
    cocos2d::Node* root_message_node = inst->CSLoader::createNode("editor/details/message_detail.csb");
    cocos2d::Node* message_panel = root_message_node->getChildByName("message_panel");
    message_panel->removeFromParent();
    message_panel->setPosition(GameLayer::get_center_pos());

    auto title_lbl = dynamic_cast<cocos2d::ui::Text*>(message_panel->getChildByName("title_lbl"));
    title_lbl->setString("Welcome Back!");
    auto body_lbl = dynamic_cast<cocos2d::ui::Text*>(message_panel->getChildByName("body_lbl"));
    body_lbl->setString("This is the body");
    scene->addChild(message_panel);


    ss << "It's been " << hours_since_last_login.count() << " hours since last login";
    CCLOG(ss.str().c_str());

    //set the last login time
    BEATUP->set_last_login();
}

bool GameLogic::init()
{
    GameLogic* instance = GameLogic::getInstance();
    instance->beatup = Beatup::create();
    instance->beatup->setName("beatup");
    instance->beatup->retain();
    instance->buildup = instance->beatup->buildup;

    return true;
};

void GameLogic::update(float dt)
{
    this->beatup->update_buildup(dt);
};

GameLogic* GameLogic::getInstance()
{
    if (!GameLogic::_instance)
    {
        GameLogic::_instance = new GameLogic();
        GameLogic::_instance->init();
    };

    return GameLogic::_instance;
};

void GameLogic::add_total_harvests(int value)
{
    DataManager::incr_key("total_harvests", value);
};

int GameLogic::get_total_harvests()
{
    return DataManager::get_int_from_data("total_harvests");
};


void GameDirector::switch_to_building_menu()
{
    auto scene = cocos2d::Scene::create();
    BuildingNuMenu* building_menu = BuildingNuMenu::create(
        BUILDUP->get_target_building()
        );
    scene->addChild(building_menu);

    auto director = cocos2d::Director::getInstance();
    director->pushScene(scene);
};
