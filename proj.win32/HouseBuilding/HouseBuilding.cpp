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
#include "../main.h"
#include "Recipe.h"
#include "Worker.h"
#include "../../Classes/RandomWeightMap.h"

const std::map<Ingredient::IngredientType, std::string> Ingredient::type_map = {
    {Ingredient::Grain, "grain"},
    {Ingredient::Seed, "seed"},
    {Ingredient::Wood, "wood"},
    {Ingredient::Iron, "iron"},
    {Ingredient::Copper, "copper"},
    {Ingredient::Wood, "wood"},
    {Ingredient::Fly, "fly"},
    {Ingredient::Flesh, "flesh"},
    {Ingredient::Berry, "berry"}
};

const std::map<Product::ProductType, std::string> Product::type_map = {
    {Product::Veggies, "veggies"},
    {Product::Meat, "meat"},
    {Product::Dairy, "dairy"},
    {Product::Cereals, "cereals"},
    {Product::Sword, "sword"},
    {Product::Shield, "shield"},
    {Product::Meat, "meat"},
};

const std::map<Waste::WasteType, std::string> Waste::type_map = {
    {Waste::Corpse, "corpse"},
    {Waste::Wasted_Iron, "wasted_iron"},
};

Buildup::Buildup()
{
    this->city = NULL;
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

#define type_stuff(ResType) std::string ResType::type_to_string(ResType::##ResType##Type type) \
{ \
    std::string result = "none"; \
    for (auto pair : ResType::type_map) \
        { \
        if (type == pair.first == type) \
                { \
            return pair.second; \
                } \
        } \
 \
    return result; \
}; \
 \
ResType::##ResType##Type ResType::string_to_type(std::string string_type) \
{ \
    ResType::##ResType##Type result = ResType::##ResType##Type::None; \
    std::transform(string_type.begin(), string_type.end(), string_type.begin(), ::tolower); \
    for (auto pair : ResType::type_map) \
        { \
        if (pair.second == string_type) \
                { \
            return pair.first; \
                } \
        } \
    print("type id " << string_type); \
    assert(false && "unknown type"); \
 \
    return result; \
};

type_stuff(Ingredient);
type_stuff(Product);
type_stuff(Waste);

void Battle::do_battle()
{
    if (this->combatants.size() != 0)
    {
        std::cout << "\tA fight between";
        for (auto fighter : this->combatants)
        {
            std::cout << " ..." << fighter->name;
            double& health = fighter->attrs->health->current_val;
            health -= std::rand() % 4 + 6;
            std::cout << " at " << fighter->attrs->health->current_val << " hp;";
        }
        std::cout << "... and that's it!" << std::endl;
    }
    else
    {
        print1("no one to fight");
    }
    return;
}

void farm_task(Building* farm, float dt)
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

    Animal animal = Animal("Horse");
    animal.b2b_transfer(
        farm,
        farm->city->building_by_name("The Workshop"),
        Resource::ResourceType::Ingredient,
        new_products/2
    );
    
};

void workshop_task(Building* workshop, float dt)
{
    if (workshop->ingredients.size() > 0)
    {
        {
            Recipe recipe = Recipe("sword");
            recipe.components[Ingredient::IngredientType::Iron] = 2;

            bool can_make_sword = recipe.is_satisfied(workshop->ingredients);
            if (can_make_sword)
            {
                print1("convert recipe's ingredient to 1 Sword");
                workshop->create_resources(Resource::Product, 1, "Sword");
                print1("creating one ruined iron");
                workshop->create_resources(Resource::Waste, 1, "wasted_iron");
                recipe.consume(workshop->ingredients);
            }
            else 
            {
                print1("can't make sword");
            };
        };
        {

            Recipe shield_rcp = Recipe("shield");
            shield_rcp.components[Ingredient::IngredientType::Wood] = 3;

            bool can_make_sword = shield_rcp.is_satisfied(workshop->ingredients);
            if (can_make_sword)
            {
                print1("convert shield_rcp's ingredient to 1 shield");
                workshop->create_resources(Resource::Product, 1, "Shield");
                shield_rcp.consume(workshop->ingredients);
                print1("creating one ruined iron");
                workshop->create_resources(Resource::Waste, 1, "wasted_iron");
                shield_rcp.consume(workshop->ingredients);
            }
            else 
            {
                print1("can't make shield");
            };
        };
    }

    // if (workshop->products.size() > 0)
    // {
    //     workshop->products.pop_back();
    //     print1("One product wasted away");
    //     workshop->create_resources(Resource::Waste, 1, "Wasted product");
    // }

    VoidFunc pay = [workshop](){
        print1("paying 3 coins for 5 products");
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

void necro_task(Building* necro, float dt)
{
    Village* city = necro->city;
    //looks for waste bodies, converts to skeletons
    Building* grave = necro->city->building_by_name("The Graveyard");

    Recipe skeleton_recipe = Recipe("Skeletons");
    skeleton_recipe.components[Ingredient::IngredientType::Flesh] = 7;

    if (skeleton_recipe.is_satisfied(necro->ingredients))
    {
        print1("creating skeleton!");
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

    // print1("thinking about necro");
    move_if_sized(Resource::Waste,
            2, 2,
            grave, necro,
            NO_CB
        );
};

void grave_task(Building* grave, float dt)
{
    //takes waste bodies from arena and buries them
};

void dump_task(Building* dump, float dt)
{
    std::cout << "\tDoing dump stuff" << std::endl;

    if (dump->wastes.size() > 5)
    {
        print1("more than 5 dump wastes, flies are gathering");
        dump->create_resources(Resource::Ingredient, 2, "fly");
    };
};

void marketplace_task(Building* building, float dt)
{
    std::cout << "\tDoing marketplace stuff" << std::endl;
};

void arena_task(Building* arena, float dt)
{
    std::cout << "\tDoing arena stuff" << std::endl;
    auto battle = std::make_shared<Battle>();
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

    //print1(bodies_to_create << " bodies to create");
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

void mine_task(Building* mine, float dt)
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

void forest_task(Building* forest, float dt)
{
    std::cout << "\tDoing forest stuff" << std::endl;
    forest->create_resources(Resource::Ingredient, 3, "berry");
    forest->create_resources(Resource::Ingredient, 1, "wood");

    if (forest->spawn_clock->passed_threshold())
    {
        print1("creating bunny");
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
    // print("\n\nupdating buildings");
    for (std::shared_ptr<Building> building : this->buildings)
    {
        building->update(dt);
    };

};

Building* Village::building_by_name(std::string name)
{
    for (auto bldg : this->buildings)
    {
        if (bldg->name == name)
            return bldg.get();
    };

    return NULL;
};

template<typename from_V>
void remove_if_sized(from_V& from_vs, unsigned int condition_size, unsigned int remove_count, VoidFunc callback )
{
    if (from_vs.size() > condition_size)
    {

        if (remove_count == 0) { remove_count = condition_size; };
        print1("removing " << remove_count);
        from_vs.erase(from_vs.begin(), from_vs.begin()+remove_count);
        callback();
    };
};

void move_if_sized(Resource::ResourceType res_type,
        unsigned int condition_size, unsigned int move_count,
        Building* from_bldg, Building* to_bldg, VoidFunc callback )
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

void Animal::b2b_transfer(Building* from_bldg, Building* to_bldg, Resource::ResourceType res_type, int quantity)
{
    print1("moving x" << quantity << " " << Resource::type_str(res_type) << " from " << from_bldg->name << " to " << to_bldg->name);
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
}

void Building::update(float dt)
{
    Updateable::update(dt);
    // this->print_inventory();
    if (update_clock->passed_threshold())
    {
        CCLOG("   %s - %.f", this->name.c_str(), this->update_clock->start_time);
        this->spawn_clock->update(dt);
        this->do_task(dt);
        update_clock->reset();
        this->print_inventory();
    }
    else
    {
        // CCLOG("   %s - %.f", this->name.c_str(), this->update_clock->start_time);
        // std::cout << "   \twaiting" << std::endl;
        // print("   \twaiting");
    }
    // std::cout << std::endl;
};

#define PRINT_RESOURCE(Rtype, Rlowertype) \
void Building::print_##Rlowertype##s() \
{ \
    std::stringstream ss; \
 \
    for (auto type_str : Rtype::type_map) \
    { \
        Rtype::Rtype##Type type = type_str.first; \
        std::string str = type_str.second; \
 \
        auto type_matches = [type](sp##Rtype ing){ return ing->Rlowertype##_type == type; }; \
        int count = std::count_if(this->Rlowertype##s.begin(), this->Rlowertype##s.end(), type_matches); \
 \
        if (count != 0) \
        { \
            ss << str << "(x" << count << ") "; \
        } \
    }; \
 \
    if (!ss.str().empty()) \
                {\
        print("   " << #Rtype << ": " << ss.str()); \
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

void Building::print_specifics()
{
    /* go through the vector, count the instances of the type of the element,
      ignoring the ones that have already been counted */
    this->print_ingredients();
    this->print_products();
    this->print_wastes();
    this->print_fighters();

};

void Building::print_inventory()
{
    std::stringstream ss;
    ss << "   ";
    ss << "ING: " << this->ingredients.size();
    ss << " PDT: " << this->products.size();
    ss << " WST: " << this->wastes.size();
    ss << " FIT: " << this->fighters.size();
    // ss << std::endl;

    // std::cout << ss.str();
    print(ss.str());
    this->print_specifics();

};

void Building::do_task(float dt)
{
    if (this->task)
    {
        this->task(this, dt);
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
    auto farm = std::make_shared<Building>(city, "The Test Farm", TaskFunc());
    farm->ingredients = inputs;
    
    ResourceCondition<Ingredient::IngredientType> rc = ResourceCondition<Ingredient::IngredientType>(
            ResourceCondition<Ingredient::IngredientType>::TypeChoices::Ingredient,
            Ingredient::IngredientType::Grain,
            2,
            "test condition");

    assert(rc.is_satisfied(farm) == true);

    farm->ingredients = {
        std::make_shared<Ingredient>("iron")
    };
    assert(rc.is_satisfied(farm) == false);

    rc.choice = Ingredient::Iron;
    assert(rc.is_satisfied(farm) == false);
    rc.quantity = 1;
    assert(rc.is_satisfied(farm) == true);

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

void Buildup::main_loop()
{
    Clock game_clock = Clock(CLOCKS_PER_SEC);
    clock_t start_time = clock() / CLOCKS_PER_SEC;

    print("starting tests...");
    test_recipe();
    test_conditions();
    print("...done tests");
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

    auto farm = std::make_shared<Building>(city, "The Farm", farm_task);
    farm->update_clock->set_threshold(2);
    farm->workers.push_back(std::make_shared<Worker>(farm, "Farmer"));
    auto dump = std::make_shared<Building>(city, "The Dump", dump_task);
    auto workshop = std::make_shared<Building>(city, "The Workshop", workshop_task);
    auto marketplace = std::make_shared<Building>(city, "The Marketplace", marketplace_task);
    auto arena = std::make_shared<Building>(city, "The Arena", arena_task);
    auto mine = std::make_shared<Building>(city, "The Mine", mine_task);
    auto grave = std::make_shared<Building>(city, "The Graveyard", grave_task);
    auto necro = std::make_shared<Building>(city, "The Underscape", necro_task);
    mine->update_clock->set_threshold(3);
    auto forest = std::make_shared<Building>(city, "The Forest", forest_task);

    auto fighter = std::make_shared<Fighter>(arena->city, "Fighter");
    auto brawler = std::make_shared<Fighter>(arena->city, "Brawler");
    brawler->attrs->health->set_vals(20);
    fighter->attrs->health->set_vals(20);
    arena->fighters.push_back(fighter);
    arena->fighters.push_back(brawler);


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
    // print("   The Player has "<< this->coins << "coins");
};

int _tmain(int argc, _TCHAR* argv[])
{
    Buildup* buildup = new Buildup();

    auto player = new Player("Jimothy");
    player->coins = 100;
    buildup->player = player;

    auto animal = std::make_shared<Animal>("Tank");

    buildup->city = Buildup::init_city(buildup);
    buildup->city->update_buildings(0);

    buildup->main_loop();

    std::cout << std::endl << "Done?";
    std::string val;
    std::cin.ignore();
    std::cout << "Peace" << std::endl;



    return 0;
}

std::string Resource::type_str(ResourceType type)
{
    auto mapping = std::map<ResourceType, std::string>();
    mapping[Ingredient] = "Ingredient";
    mapping[ResourceType::Product] = "Product";
    mapping[ResourceType::Waste] = "Waste";

    return mapping[type];

}
