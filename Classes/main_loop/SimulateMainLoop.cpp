#include "SimulateMainLoop.h"

#include "base/CCDirector.h"
#include "ui/UIText.h"
#include "ui/UIImageView.h"

#include "constants.h"
#include "GameLogic.h"
#include "Buildup.h"
#include "HouseBuilding.h"
#include "Ingredients.h"
#include "Util.h"
#include "StaticData.h"
#include "NodeBuilder.h"
#include "MiscUI.h"
#include "Modal.h"
#include "ui/UIListView.h"

bool SimulateMainLoop::is_simulating = false;

mistIngredient SimulateMainLoop::simulate(float dt)
{
    //TODO(idea) maybe simulate it for up to an hour, and multiply that output by
    //the hours remaining. Alternatively, rewrite it somehow so that it
    //accurately estimates it from nothing.

    //grab a copy of the ingredients before simulation
    auto original_ingredients = BUILDUP->get_all_ingredients();

    SimulateMainLoop::is_simulating = true;
    BUILDUP->city->update(dt);

    mistIngredient updated_ingredients = BUILDUP->get_all_ingredients();

    //ingredients that were added based on the difference between original_ and updated_ingredients
    mistIngredient diff_ingredients;
    for (auto mist_ing : updated_ingredients)
    {
        Ingredient::SubType ing_type = mist_ing.first;
        res_count_t updated_count = mist_ing.second;

        res_count_t original_count = map_get(original_ingredients, ing_type, 0.0L);

        if (updated_count - original_count > 0.0)
        {
            res_count_t gained = updated_count - original_count;
            diff_ingredients[ing_type] = gained;
        }
    }

    SimulateMainLoop::is_simulating = false;

    return diff_ingredients;
};

void SimulateMainLoop::generate_WIP_welcome_message()
{
    auto running_scene = cocos2d::Director::getInstance()->getRunningScene();

    //combine ingredient types into one vector
    std::vector<Ingredient::SubType> ing_types;
    ing_types.insert(ing_types.cend(), Ingredient::basic_ingredients.begin(), Ingredient::basic_ingredients.end());
    ing_types.insert(ing_types.cend(), Ingredient::advanced_ingredients.begin(), Ingredient::advanced_ingredients.end());

    auto city_ingredients = BUILDUP->get_all_ingredients();

    auto welcome_back_modal = BaseModal::create();
    welcome_back_modal->set_title("Welcome back!");
    running_scene->addChild(welcome_back_modal);

    int width = 3;
    float cell_width = 150.0f;
    float cell_height = 150.0f;

    float cell_margin = 5.0f;

    cocos2d::Vec2 initial_pos = cocos2d::Vec2::ZERO;
    float x = initial_pos.x;
    float y = initial_pos.y;

    int i = 0;
    cocos2d::ui::Layout* layout = cocos2d::ui::Layout::create();
    layout->setLayoutType(cocos2d::ui::Layout::Type::HORIZONTAL);
    cocos2d::ui::ListView* body_scroll = welcome_back_modal->get_body_scroll();
    layout->setContentSize({ cell_width*width+cell_margin*width, cell_height+cell_margin});
    body_scroll->addChild(layout);
    for (auto ing_type : ing_types)
    {
        cocos2d::ui::Layout* panel = dynamic_cast<cocos2d::ui::Layout*>(get_prebuilt_node_from_csb("editor/details/report_ing_detail.csb")->getChildByName("panel"));

        panel->removeFromParent();
        layout->addChild(panel);

        i++;
        if (i >= width)
        {
            i = 0;
            layout = cocos2d::ui::Layout::create();
            layout->setLayoutType(cocos2d::ui::Layout::Type::HORIZONTAL);
            layout->setContentSize({ cell_width*width+cell_margin*width, cell_height+cell_margin});
            body_scroll->addChild(layout);
        }

        IngredientData ingredient_data(ing_type);
        std::string img_path = ingredient_data.get_img_large();

        //title
        auto ing_title = dynamic_cast<cocos2d::ui::Text*>(panel->getChildByName("title_lbl"));
        ing_title->setString(ingredient_data.get_name());

        //count
        auto count_lbl = dynamic_cast<cocos2d::ui::Text*>(panel->getChildByName("count_lbl"));
        count_lbl->setString(beautify_double(map_get(city_ingredients, ing_type, 0)));

        //ingredient icon
        auto ing_icon = dynamic_cast<cocos2d::ui::ImageView*>(panel->getChildByName("ing_icon"));
        image_view_scale9_hack(ing_icon);
        ing_icon->loadTexture(img_path);
        set_aliasing(ing_icon);

    }
};
