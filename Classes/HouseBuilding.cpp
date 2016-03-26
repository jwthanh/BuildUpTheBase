// ConsoleApplication1.cpp : Defines the entry point for the console application.
//

#include <memory>
#include <string>
#include <vector>
#include <iostream>
#include <ctime>
#include <assert.h>
#include <sstream>
#include <algorithm>
#include <locale>

#include "HouseBuilding.h"
#include "Recipe.h"
#include "Worker.h"
#include "../../Classes/RandomWeightMap.h"
#include "../../Classes/attribute.h"

#include "../../Classes/Beatup.h"
#include <Util.h>
#include <FShake.h>

#include "cocos2d.h"
#include <tinyxml2/tinyxml2.h>
#include <json/document.h>
#include "FileOperation.h"

USING_NS_CC;


Buildup::Buildup()
{
    this->city = NULL;
    this->target_building = NULL;
};

Updateable::Updateable()
{
    this->update_clock = new Clock(15.0f);
};

void Updateable::update(float dt)
{
    this->update_clock->update(dt);
};

void Fighter::update(float dt)
{
    Updateable::update(dt);
};

std::string Fighter::get_stats()
{
    std::stringstream ss;
    ss << this->name << " ";
    double effective_dmg = this->attrs->damage->current_val;
    ss << this->attrs->health->current_val << "/" << this->attrs->health->max_val << "/" << effective_dmg;
    // for (std::string attr : this->attrs->PrettyVector())
    // {
    //     ss << attr;
    // }

    return ss.str();
}


FighterNode* FighterNode::create(Beatup* beatup, spFighter fighter)
{

    FighterNode* node = FighterNode::create();
    node->setTouchEnabled(true);

    node->beatup = beatup;
    node->fighter = fighter;

    node->sprite = ui::ImageView::create(fighter ? fighter->sprite_name : "townsmen8x8.png", TextureResType::PLIST);
    node->sprite->setScale(8);
    node->sprite->setPosition(Vec2(0, 100));
    node->addChild(node->sprite);

    node->sprite->setTouchEnabled(true);
    node->sprite->addTouchEventListener([node](Ref*, TouchEventType)
    {
        node->beatup->switch_to_character_menu(node->fighter);
    });

    //hp bar
    node->hp_bar = new ProgressBar(node->beatup, "enemy_healthbar_bar.png", "enemy_healthbar_bar_white.png");
    node->hp_bar->back_timer->setVisible(false);
    node->hp_bar->setPosition(Vec2(0, 0));
    node->hp_bar->setAnchorPoint(Vec2(0.5, 0.5));
    node->hp_bar->setScale(2);
    node->hp_bar->base_node->removeFromParent();
    if (fighter != NULL) {
        node->hp_bar->set_percentage(fighter->attrs->health->get_val_percentage());
    };
    node->addChild(node->hp_bar->base_node);

    //xp bar
    node->xp_bar = new ProgressBar(node->beatup, "enemy_healthbar_bar.png", "enemy_healthbar_bar_white.png");
    node->xp_bar->back_timer->setVisible(false);
    node->xp_bar->setPosition(Vec2(0, -25));
    node->xp_bar->setAnchorPoint(Vec2(0.5, 0.5));
    node->xp_bar->setScale(2);
    node->xp_bar->base_node->removeFromParent();
    node->xp_bar->front_timer->setColor(Color3B::BLUE);
    node->addChild(node->xp_bar->base_node);

    node->setPosition(node->beatup->get_center_pos(sx(300)));
    node->setAnchorPoint(Vec2(0.5, 0.5));
    node->beatup->addChild(node);

    return node;
};

void FighterNode::update(float dt)
{
    if (this->fighter == NULL) return;

    auto hp_percentage = this->fighter->attrs->health->get_val_percentage();

    if (this->hp_bar->target_percentage != hp_percentage)
    {
        FShake* shake = FShake::actionWithDuration(0.1f, 1.0f);
        this->hp_bar->front_timer->runAction(shake);
        this->hp_bar->back_timer->runAction(shake->clone());
        this->hp_bar->background->runAction(shake->clone());
    }

    this->hp_bar->set_percentage(hp_percentage);
    this->xp_bar->set_percentage(this->fighter->xp->get_progress_percentage()*100);
};

void FighterNode::load_new_sprite(std::string name)
{
    this->sprite->loadTexture(name, TextureResType::PLIST);
};

void FighterNode::set_fighter(spFighter fighter)
{
    this->fighter = fighter;

    this->load_new_sprite(fighter->sprite_name);

    if (fighter != NULL) {
        this->hp_bar->set_percentage(fighter->attrs->health->get_val_percentage());
        this->xp_bar->set_percentage(fighter->xp->get_progress_percentage());
    };

};

void FighterNode::clear_fighter()
{
    this->fighter = NULL;

    this->load_new_sprite("back_button.png"); //placeholder

    this->hp_bar->set_percentage(0);
    this->xp_bar->set_percentage(0);
};

vsFighter Battle::combatants_by_team(Fighter::TeamsType team)
{
    vsFighter team_combatants = vsFighter();

    for (auto fighter : this->combatants)
    {
        if (fighter->team == team)
        {
            team_combatants.push_back(fighter);
        }
    }
    return team_combatants;
};

void Battle::fight(spFighter left, spFighter right)
{
    if (left->attrs->health->current_val < 1 || right->attrs->health->current_val < 1) {
        printj("someone is dead, skipping");
        return;
    };

    std::stringstream ss;
    ss << "\tA fight! ";
    ss << left->name << " vs " << right->name;

    double& health = right->attrs->health->current_val;
    auto dmg = left->attrs->damage->current_val;

    if (left->has_sword)
    {
        dmg *= 2;
    };
    health -= ceil(cocos2d::rand_0_1() * dmg);

    ss << " " << right->name << " at " << right->attrs->health->current_val << " hp;";

    if (right->combat->is_dead())
    {
        ss << " and it died!";

        if (this->buildup->beatup->enemy_node->fighter == right) {
            printj("matched fighternode, removing");
            this->buildup->beatup->enemy_node->clear_fighter();
        };

        //only give exp to killer
        left->combat->give_exp(right->xp->value);
        // this->distribute_exp(right);
    }

    //printj(ss.str());
};

void Battle::do_battle()
{
    if (this->combatants.size() > 1)
    {
        vsFighter team_1 = this->combatants_by_team(Fighter::TeamOne);
        vsFighter team_2 = this->combatants_by_team(Fighter::TeamTwo);

        //team1 attacks team2
        for (auto fighter : team_1)
        {
            for (auto enemy : team_2) {
                this->fight(fighter, enemy);
            }
        }

        //team2 attacks team1 after
        for (auto fighter : team_2)
        {
            for (auto enemy : team_1) {
                this->fight(fighter, enemy);
            }
        }
    }
    else
    {
        printj1("no one to fight");
    }
    return;
}

///Give every other combatant exp
void Battle::distribute_exp(spFighter dead_fighter)
{
    float live_combatants = 0;
    for (auto fighter : this->combatants) {
        if (!fighter->combat->is_dead()) {
            live_combatants++;
        };
    };

    for (auto fighter : this->combatants) {
        if (!fighter->combat->is_dead()) {
            fighter->combat->give_exp(dead_fighter->xp->value / live_combatants);

        };
    };
};

void farm_task(spBuilding farm, float dt)
{
    RandomWeightMap<std::string> farm_spawn_map = RandomWeightMap<std::string>();
    farm_spawn_map.add_item("grain", 10);
    farm_spawn_map.add_item("seed", 5);
    std::string ing_type = farm_spawn_map.get_item();


    RandomWeightMap<int> farm_count_map = RandomWeightMap<int>();
    farm_count_map.add_item(10, 15);
    farm_count_map.add_item(5, 5);
    farm_count_map.add_item(6, 5);
    farm_count_map.add_item(7, 5);
    int new_products = farm_count_map.get_item();

    farm->create_resources(Resource::Ingredient, new_products, "grain");

    Recipe recipe = Recipe("pileofgrain");
    recipe.components[Ingredient::IngredientType::Grain] = 10;
    if (recipe.is_satisfied(farm->ingredients))
    {
        farm->create_resources(Resource::Ingredient, 1, "PileOfGrain");
        printj1("Created a pile of grain (no todo)");
        recipe.consume(farm->ingredients);
    };

    Animal animal = Animal("Horse");
    animal.b2b_transfer(
        farm,
        farm->city->building_by_name("The Workshop"),
        Resource::ResourceType::Ingredient,
        new_products/2
    );
    
};

void workshop_task(spBuilding workshop, float dt)
{
    if (workshop->ingredients.size() > 0)
    {
        {
            Recipe recipe = Recipe("sword");
            recipe.components[Ingredient::IngredientType::Iron] = 2;

            bool can_make_sword = recipe.is_satisfied(workshop->ingredients);
            if (can_make_sword)
            {
                printj1("convert recipe's ingredient to 1 Sword");
                workshop->create_resources(Resource::Product, 1, "Sword");
                printj1("creating one ruined iron");
                workshop->create_resources(Resource::Waste, 1, "wasted_iron");
                recipe.consume(workshop->ingredients);
                workshop->city->buildup->beatup->fighter_node->setColor(Color3B::RED);
                workshop->city->buildup->fighter->has_sword = true;
            }
            else 
            {
                // printj1("can't make sword");
            };
        };
        {

            Recipe shield_rcp = Recipe("shield");
            shield_rcp.components[Ingredient::IngredientType::Wood] = 3;

            bool can_make_sword = shield_rcp.is_satisfied(workshop->ingredients);
            if (can_make_sword)
            {
                printj1("convert shield_rcp's ingredient to 1 shield");
                workshop->create_resources(Resource::Product, 1, "Shield");
                shield_rcp.consume(workshop->ingredients);
                printj1("creating one ruined iron");
                workshop->create_resources(Resource::Waste, 1, "wasted_iron");
                shield_rcp.consume(workshop->ingredients);
            }
            else 
            {
                // printj1("can't make shield");
            };
        };
    }

    // if (workshop->products.size() > 0)
    // {
    //     workshop->products.pop_back();
    //     printj1("One product wasted away");
    //     workshop->create_resources(Resource::Waste, 1, "Wasted product");
    // }

    VoidFunc pay = [workshop](){
        printj1("paying 3 coins for 5 products");
        workshop->city->buildup->player->coins += 3;
    };
    remove_if_sized(workshop->products, 5, 5, pay);

    std::cout << "\tDoing workshop stuff" << std::endl;

    move_if_sized(Resource::Waste,
        2, 2,
        workshop, workshop->city->building_by_name("The Dump"),
        NO_CB
    );
};

void necro_task(spBuilding necro, float dt)
{
    Village* city = necro->city;
    //looks for waste bodies, converts to skeletons
    spBuilding grave = necro->city->building_by_name("The Graveyard");

    Recipe skeleton_recipe = Recipe("Skeletons");
    skeleton_recipe.components[Ingredient::IngredientType::Flesh] = 7;
    if (skeleton_recipe.is_satisfied(necro->ingredients))
    {
        printj1("creating skeleton!");
        auto skelly = std::make_shared<Fighter>(city, "Skeleton");
        skelly->attrs->health->set_vals(15);
        city->building_by_name("The Arena")->fighters.push_back(skelly);
        skeleton_recipe.consume(necro->ingredients);
    };

    if (necro->wastes.size() > 0)
    {
        necro->create_resources(Resource::Ingredient, 5, "Flesh");
        necro->wastes.pop_back();
    };

    // printj1("thinking about necro");
    move_if_sized(Resource::Waste,
            2, 2,
            grave, necro,
            NO_CB
        );
};

void grave_task(spBuilding grave, float dt)
{
    //takes waste bodies from arena and buries them
};

void dump_task(spBuilding dump, float dt)
{
    std::cout << "\tDoing dump stuff" << std::endl;

    if (dump->wastes.size() > 5)
    {
        printj1("more than 5 dump wastes, flies are gathering");
        dump->create_resources(Resource::Ingredient, 2, "fly");
    };
};

void marketplace_task(spBuilding building, float dt)
{
    std::cout << "\tDoing marketplace stuff" << std::endl;
};

void arena_task(spBuilding arena, float dt)
{
    auto city = arena->city;
    //expect two allies or dont spawn more
    if (arena->fighters.size() <= 2)
    {
        // printj1("creating squirrel!");
        auto squirrel = std::make_shared<Fighter>(city, "Squirrel");
        squirrel->xp->value = 25;
        squirrel->attrs->health->set_vals(80);
        squirrel->attrs->damage->set_vals(10);
        arena->fighters.push_back(squirrel);

        if (arena->city->buildup->beatup->enemy_node->fighter == NULL) {
            arena->city->buildup->beatup->enemy_node->fighter = squirrel;
            arena->city->buildup->beatup->enemy_node->load_new_sprite(squirrel->sprite_name);
        }
    };

    std::cout << "\tDoing arena stuff" << std::endl;
    auto battle = std::make_shared<Battle>(arena->city->buildup);
    for (spFighter fighter : arena->fighters)
    {
        if (!fighter->attrs->health->is_empty())
        {
            battle->combatants.push_back(fighter);
        };
    };

    battle->do_battle();

    int bodies_to_create = 0;
    auto remove_dead = [&bodies_to_create](vsFighter& fighters)
    {
        return std::remove_if(
            fighters.begin(),
            fighters.end(),
            [&](spFighter fighter) -> bool {
            // Do "some stuff", then return true if element should be removed.
            HealthAttribute* health = fighter->attrs->health;
            if (health->is_empty())
            {
                bodies_to_create++;
                return true;
            }
            return false;
        });
    };
    auto removed_it = remove_dead(battle->combatants);
    battle->combatants.erase(
            removed_it,
            battle->combatants.end()
            );

    //printj1(bodies_to_create << " bodies to create");
    for (int i = 0; i < bodies_to_create; i++)
    {
        auto grave = arena->city->building_by_name("The Graveyard");
        grave->create_resources(Resource::Waste, 1, "Corpse");
    }

    //needs to be after because bodies_to_create gets incremented in this too
    auto remove_fighter_it = remove_dead(arena->fighters);
    arena->fighters.erase(
            remove_fighter_it,
            arena->fighters.end()
            );

};

void mine_task(spBuilding mine, float dt)
{
    std::cout << "\tDoing mine stuff" << std::endl;

    RandomWeightMap<std::string> mine_spawn_map = RandomWeightMap<std::string>();
    mine_spawn_map.add_item("iron", 30);
    mine_spawn_map.add_item("copper", 70);
    std::string ing_type = mine_spawn_map.get_item();
    mine->create_resources(Resource::Ingredient, 9, ing_type);

    move_if_sized(Resource::Ingredient,
        2, 2,
        mine, mine->city->building_by_name("The Workshop"),
        NO_CB);
};

void forest_task(spBuilding forest, float dt)
{
    std::cout << "\tDoing forest stuff" << std::endl;
    forest->create_resources(Resource::Ingredient, 3, "berry");
    forest->create_resources(Resource::Ingredient, 1, "wood");

    if (forest->spawn_clock->passed_threshold())
    {
        printj1("creating bunny");
        auto bunny = std::make_shared<Fighter>(forest->city, "bunny");
        bunny->attrs->health->set_vals(5);
        forest->fighters.push_back(bunny);
        forest->spawn_clock->reset();
    };

    move_if_sized(Resource::Ingredient,
        5, 5,
        forest, forest->city->building_by_name("The Workshop"),
        NO_CB);
};

void Village::update(float dt)
{
    Updateable::update(dt);

    this->update_buildings(dt);
};


void Village::update_buildings(float dt)
{
    // printj("\n\nupdating buildings");
    for (std::shared_ptr<Building> building : this->buildings)
    {
        building->update(dt);
    };

};

spBuilding Village::building_by_name(std::string name)
{
    for (auto bldg : this->buildings)
    {
        if (bldg->name == name)
            return bldg;
    };

    return NULL;
};

template<typename from_V>
void remove_if_sized(from_V& from_vs, unsigned int condition_size, unsigned int remove_count, VoidFunc callback )
{
    if (from_vs.size() > condition_size)
    {

        if (remove_count == 0) { remove_count = condition_size; };
        printj1("removing " << remove_count);
        from_vs.erase(from_vs.begin(), from_vs.begin()+remove_count);
        callback();
    };
};

void move_if_sized(Resource::ResourceType res_type,
        unsigned int condition_size, unsigned int move_count,
        spBuilding from_bldg, spBuilding to_bldg, VoidFunc callback )
{
    unsigned int from_size;
    if (res_type == Resource::Ingredient) from_size = from_bldg->ingredients.size();
    else if (res_type == Resource::Product) from_size = from_bldg->products.size();
    else if (res_type == Resource::Waste) from_size = from_bldg->wastes.size();

    if (from_size >= condition_size)
    {
        if (move_count == 0) { move_count = condition_size; };
        Animal animal = Animal("Horse");
        animal.b2b_transfer(
            from_bldg,
            to_bldg,
            res_type,
            move_count
            );
        callback();
    };
};

template<typename from_V, typename to_V>
void transfer(from_V& from_vs, to_V& to_vs, unsigned int quantity)
{
    if (from_vs.size() < quantity)
    {
        std::cout << quantity << " is too many.";
        quantity = from_vs.size();
        std::cout << " new size is" << quantity << std::endl;
    }

    if (quantity > 0)
    {
        auto it = std::next(from_vs.begin(), quantity);
        std::move(from_vs.begin(), it, std::back_inserter(to_vs));
        from_vs.erase(from_vs.begin(), it);
    }
};

void Animal::b2b_transfer(spBuilding from_bldg, spBuilding to_bldg, Resource::ResourceType res_type, int quantity)
{
    //printj1("moving x" << quantity << " " << Resource::type_str(res_type) << " from " << from_bldg->name << " to " << to_bldg->name);
    if (res_type == Resource::Ingredient)
    {
        transfer(from_bldg->ingredients, to_bldg->ingredients, quantity);
    }
    else if (res_type == Resource::Product)
    {
        transfer(from_bldg->products, to_bldg->products, quantity);
    }
    else if (res_type == Resource::Waste)
    {
        transfer(from_bldg->wastes, to_bldg->wastes, quantity);
    }
    else
    {
        std::cout << "type not recognized" << std::endl;
    }

};

template<typename T>
std::shared_ptr<T> create_one(std::string name)
{
    return std::make_shared<T>(name);
};

template<>
std::shared_ptr<Ingredient> create_one(std::string name)
{ //TODO do this for all types of IPW
    auto ing =  std::make_shared<Ingredient>(name);
    ing->ingredient_type = Ingredient::string_to_type(name);
    return ing;
};

template<typename T, typename vectorT>
void create(vectorT& vec, int quantity, std::string name)
{
    vectorT created_resources = vectorT();

    for (int i = 0; i < quantity; i++)
    {
        std::shared_ptr<T> p = create_one<T>(name);
        created_resources.push_back(p);
    };

    vec.insert(
        vec.end(),
        created_resources.begin(),
        created_resources.end()
    );

};

Building::Building(Village* city, std::string name, std::string id_key, TaskFunc task) :
             task(task), Nameable(name), Buyable(id_key), Updateable(), city(city)
{
    num_workers = 1;

    update_clock->set_threshold(1.0f);
    spawn_clock = new Clock(3);

    products = vsProduct();
    wastes = vsWaste();
    ingredients = vsIngredient();

    fighters = vsFighter();
    workers = vsWorker();

    menu_items = {};

    std::stringstream ss;

    //auto file_utils = FileUtils::getInstance();
    // std::string xmlBuffer = file_utils->getStringFromFile("joshtest.dat");
    //tinyxml2::XMLDocument* xmlDoc = new tinyxml2::XMLDocument();
    //xmlDoc->Parse(xmlBuffer.c_str(), xmlBuffer.size());
    //auto root = xmlDoc->RootElement();

    auto jsonDoc = FileIO::open_json("buildings_data.json");
    assert(jsonDoc.HasMember("buildings"));
    if (jsonDoc["buildings"].HasMember(name.c_str()))
    {
        auto all_buildings = &jsonDoc["buildings"];
        auto this_building = &(*all_buildings)[name.c_str()];

        if (this_building->HasMember("task_name")) {
            auto task_name = &(*this_building)["task_name"];
            ss << task_name->GetString() << " task";
        } else {
            ss << name << " task";
        };
    }
    else {
        ss << name << " task";
    };

     menu_items = {
         {"default", ss.str(), [this,task](){
             task(shared_from_this(), 0);
             return true;
         }, false},
     };

     this->data = std::make_shared<BuildingData>(name);
    this->_shop_cost = atoi(this->data->get_gold_cost().c_str());

};


void Building::create_resources(Resource::ResourceType type, int quantity, std::string name)
{

    std::cout << "creating " << name << "("<<Resource::type_str(type) << ") x" << quantity << std::endl;
    if (type == Resource::Ingredient)
    {
        create<Ingredient>(this->ingredients, quantity, name);
    }
    else if (type == Resource::Product)
    {
        create<Product>(this->products, quantity, name);
    }
    else if (type == Resource::Waste)
    {
        create<Waste>(this->wastes, quantity, name);
    }
    else
    {
        std::cout << "type not recognized" << std::endl;
    }
};

void Building::consume_recipe(Recipe* recipe)
{
    if (recipe->is_satisfied(this->ingredients))
    {
        recipe->consume(this->ingredients);
        recipe->callback(NULL); //TODO add beatup
    }
};

void Building::update(float dt)
{
    Updateable::update(dt);

    // if (update_clock->passed_threshold())
    // {
    //     this->spawn_clock->update(dt);
    //     this->do_task(dt);
    //     update_clock->reset();
    // }
    // else
    // {
    // }
};
std::string Building::get_specifics()
{
    return this->get_ingredients() + " " +
    this->get_products() + " " +
    this->get_wastes();
    // this->get_fighters();

};

void Building::print_specifics()
{
    this->print_ingredients();
    this->print_products();
    this->print_wastes();
    this->print_fighters();
};

#define PRINT_RESOURCE(Rtype, Rlowertype) \
std::map<Rtype::Rtype##Type,int> Building::get_##Rlowertype##_count()\
{\
    std::map<Rtype::Rtype##Type, int> Rlowertype_map = std::map<Rtype::Rtype##Type, int>();\
    for (auto type_str : Rtype::type_map) \
    { \
        Rtype::Rtype##Type type = type_str.first; \
 \
        auto type_matches = [type](sp##Rtype ing){ \
            return ing->Rlowertype##_type == type;\
        }; \
        int count = std::count_if(\
            this->Rlowertype##s.begin(),\
            this->Rlowertype##s.end(),\
            type_matches\
        ); \
        \
        Rlowertype_map[type] = count;\
    };\
    return Rlowertype_map;\
};\
\
std::string Building::get_##Rlowertype##s() \
{ \
    std::stringstream ss; \
 \
    for (auto type_str : Rtype::type_map) \
    { \
        Rtype::Rtype##Type type = type_str.first; \
        std::string str = type_str.second; \
 \
        auto type_matches = [type](sp##Rtype ing){ \
            return ing->Rlowertype##_type == type;\
        }; \
        int count = std::count_if(\
            this->Rlowertype##s.begin(),\
            this->Rlowertype##s.end(),\
            type_matches\
        ); \
 \
        if (count != 0) \
        { \
            ss << str << "(x" << count << ") "; \
        } \
    }; \
    return ss.str();\
 \
};\
\
void Building::print_##Rlowertype##s() \
{ \
    std::string ss = this->get_##Rlowertype##s(); \
    if (!ss.empty()) \
    { \
        printj("   " << #Rtype << ": " << ss); \
    }\
};

PRINT_RESOURCE(Ingredient, ingredient);
PRINT_RESOURCE(Product, product);
PRINT_RESOURCE(Waste, waste);

void Building::print_fighters()
{
    std::stringstream ss;
    for (spFighter fighter : this->fighters) 
    {
        ss << fighter->get_stats() << " ";
    };

    if (!ss.str().empty()) 
    {
        std::cout << "   " << ss.str() << std::endl;
    };
};


std::string Building::get_inventory()
{
    std::stringstream ss;
    ss << "ING: " << this->ingredients.size();
    ss << " PDT: " << this->products.size();
    ss << " WST: " << this->wastes.size();
    ss << " FIT: " << this->fighters.size();
    // ss << std::endl;

    return ss.str();
};

void Building::print_inventory()
{
    std::string inventory = this->get_inventory();
    printj("   " << inventory);
    this->print_specifics();

};

void Building::do_task(float dt)
{
    if (this->task)
    {
        this->task(shared_from_this(), dt);
    };
};

void test_conditions()
{
    vsIngredient inputs = {
        std::make_shared<Ingredient>("grain"),
        std::make_shared<Ingredient>("grain"),
        std::make_shared<Ingredient>("iron")
    };

    auto city = new Village(NULL, "The Test City");
    auto farm = std::make_shared<Building>(city, "The Test Farm", "test_farm", TaskFunc());
    farm->ingredients = inputs;
    
    ResourceCondition* rc = ResourceCondition::create_ingredient_condition(Ingredient::Grain, 2, "test condition");
    // IngredientCondition rc = IngredientCondition(
    //         IngredientCondition::TypeChoices::Ingredient,
    //         Ingredient::Grain,
    //         2,
    //         "test condition");

    assert(rc->is_satisfied(farm) == true);

    farm->ingredients = {
        std::make_shared<Ingredient>("iron")
    };
    assert(rc->is_satisfied(farm) == false);

    rc->ing_type = Ingredient::Iron;
    assert(rc->is_satisfied(farm) == false);
    rc->quantity = 1;
    assert(rc->is_satisfied(farm) == true);

    delete rc;

};

void test_recipe()
{
    vsIngredient inputs = {
        std::make_shared<Ingredient>("grain"),
        std::make_shared<Ingredient>("grain"),
        std::make_shared<Ingredient>("iron")
    } ;

    Recipe recipe = Recipe("test recipe");
    recipe.components = ComponentMap();
    recipe.components[Ingredient::IngredientType::Grain] = 2;
    recipe.components[Ingredient::IngredientType::Iron] = 1;
    bool result = recipe.is_satisfied(inputs);
    assert(result && "everythings there");
    std::cout << "is the recipe satisfied? " << std::boolalpha << result << std::endl << std::endl;

    recipe.components = ComponentMap();
    recipe.components[Ingredient::IngredientType::Grain] = 3;
    result = recipe.is_satisfied(inputs);
    assert(!result && " missing one type but has the other");

    recipe.components = ComponentMap();
    recipe.components[Ingredient::IngredientType::Grain] = 1;
    result = recipe.is_satisfied(inputs);
    assert(result && "over shoots reqs");

    recipe.components = ComponentMap();
    recipe.components[Ingredient::IngredientType::Fly] = 5;
    result = recipe.is_satisfied(inputs);
    assert(!result && "looks for ingredient not in input");

}

void Buildup::update(float dt)
{
    this->city->update(dt);
    this->player->update(dt);
};

//this isn't used anymore
void Buildup::main_loop()
{
    Clock game_clock = Clock(CLOCKS_PER_SEC);
    clock_t start_time = clock() / CLOCKS_PER_SEC;

    printj("starting tests...");
    test_recipe();
    test_conditions();
    printj("...done tests");
    int total_loops = 0;

    int current_ticks = 0;
    while (true)
    {
        total_loops++;
        game_clock.update((float)current_ticks);

        //current_ticks = clock() / CLOCKS_PER_SEC - start_time;
        current_ticks += 1000;
        if (game_clock.passed_threshold())
        {
            this->city->update(game_clock.start_time);
            this->player->update(game_clock.start_time);

            game_clock.reset();

            current_ticks = 0;
            start_time = clock() / CLOCKS_PER_SEC;

            std::string temp;
            std::cout << "enter to continue " << std::endl;
            std::getline(std::cin, temp);
        }
    }


}

Village* Buildup::init_city(Buildup* buildup)
{
    auto city = new Village(buildup, "Burlington");

    auto farm = std::make_shared<Building>(city, "The Farm", "the_farm", farm_task);
    farm->update_clock->set_threshold(2);
    farm->workers.push_back(std::make_shared<Worker>(farm, "Farmer"));
    farm->punched_ingredient_type = "grain";
    buildup->target_building = farm;

    auto dump = std::make_shared<Building>(city, "The Dump", "the_dump", dump_task);
    dump->punched_ingredient_type = "fly";

    auto workshop = std::make_shared<Building>(city, "The Workshop", "the_workshop", workshop_task);
    workshop->punched_ingredient_type = "wood";

    auto marketplace = std::make_shared<Building>(city, "The Marketplace", "the_marketplace", marketplace_task);

    auto arena = std::make_shared<Building>(city, "The Arena", "the_arena", arena_task);
    arena->punched_ingredient_type = "sand";

    auto mine = std::make_shared<Building>(city, "The Mine", "the_mine", mine_task);
    mine->punched_ingredient_type = "copper";

    auto grave = std::make_shared<Building>(city, "The Graveyard", "the_graveyard", grave_task);
    grave->punched_ingredient_type = "flesh";

    auto necro = std::make_shared<Building>(city, "The Underscape", "the_underscape", necro_task);
    mine->update_clock->set_threshold(3);

    auto forest = std::make_shared<Building>(city, "The Forest", "the_forest", forest_task);
    forest->punched_ingredient_type = "berry";

    buildup->fighter = std::make_shared<Fighter>(arena->city, "Fighter");
    buildup->fighter->sprite_name = "townsmen8x8.png";
    buildup->fighter->team = Fighter::TeamOne;
    buildup->fighter->attrs->health->set_vals(100);

    buildup->brawler = std::make_shared<Fighter>(arena->city, "Brawler");
    buildup->brawler->team = Fighter::TeamOne;
    buildup->brawler->sprite_name = "ogre10x10.png";
    buildup->brawler->attrs->health->set_vals(200);


    arena->fighters.push_back(buildup->fighter);
    arena->fighters.push_back(buildup->brawler);


    city->buildings.push_back(farm);
    city->buildings.push_back(dump);
    city->buildings.push_back(workshop);
    city->buildings.push_back(marketplace);
    city->buildings.push_back(arena);
    city->buildings.push_back(mine);
    city->buildings.push_back(grave);
    city->buildings.push_back(necro);
    city->buildings.push_back(forest);

    return city;
};

void Player::update(float dt)
{
    // printj("   The Player has "<< this->coins << "coins");
};

