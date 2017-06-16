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

bool SimulateMainLoop::is_simulating = false;

void SimulateMainLoop::simulate(float dt)
{
    SimulateMainLoop::is_simulating = true;
    //TODO maybe simulate it for up to an hour, and multiply that output by
    //the hours remaining. Alternatively, rewrite it somehow so that it
    //accurately estimates it from nothing.
    BUILDUP->city->update(dt);

    SimulateMainLoop::is_simulating = false;
};

void SimulateMainLoop::generate_WIP_welcome_message()
{
    auto running_scene = cocos2d::Director::getInstance()->getRunningScene();

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
        running_scene->addChild(panel);

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
