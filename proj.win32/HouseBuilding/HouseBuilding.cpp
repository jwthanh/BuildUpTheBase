// ConsoleApplication1.cpp : Defines the entry point for the console application.
//

#include <memory>
#include <string>
#include <vector>
#include <iostream>
#include <ctime>

#include "HouseBuilding.h"
#include <sstream>
#include "../main.h"
#include "Recipe.h"
#include <algorithm>

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
}

std::string Ingredient::type_to_string(Ingredient::IngredientType type)
{
    std::string result = "none";
    if (type == Ingredient::Grain) return "grain";
    else if (type == Ingredient::Iron) return "iron";

    return result;
};

Ingredient::IngredientType Ingredient::string_to_type(std::string string_type)
{
    Ingredient::IngredientType result = Ingredient::IngredientType::None;
    if (string_type == "grain") return Ingredient::Grain;
    else if (string_type == "iron") return Ingredient::Iron;

    return result;

};

void Battle::do_battle()
{
    if (this->combatants.size() != 0)
    {
        std::cout << "A fight between";
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
        std::cout << "no one to fight" << std::endl;
    }
    return;
}

void farm_task(Building* farm)
{
    int new_products = 10;
    farm->create_resources(Resource::Ingredient, new_products, "grain");

    Animal animal = Animal("Horse");
    animal.b2b_transfer(
        farm,
        farm->city->building_by_name("The Workshop"),
        Resource::ResourceType::Ingredient,
        new_products/2
    );
};

void workshop_task(Building* workshop)
{
    if (workshop->ingredients.size() > 0)
    {
        print("convert one ingredient to 2 Pies");
        workshop->ingredients.pop_back();
        workshop->create_resources(Resource::Product, 2, "Pie");
    }
    if (workshop->products.size() > 0)
    {
        workshop->products.pop_back();
        print("One product wasted away");
        workshop->create_resources(Resource::Waste, 1, "Wasted product");
    }

    VoidFunc pay = [workshop](){
        print("paying 3 coins for 5 products");
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

void necro_task(Building* necro)
{
    //looks for waste bodies, converts to skeletons
    Building* grave = necro->city->building_by_name("The Graveyard");

    // print("thinking about necro");
    move_if_sized(Resource::Waste,
            2, 2,
            grave, necro,
            NO_CB
        );
};

void grave_task(Building* grave)
{
    //takes waste bodies from arena and buries them
};

void dump_task(Building* dump)
{
    std::cout << "\tDoing dump stuff" << std::endl;

    if (dump->wastes.size() > 5)
    {
        print("more than 5 dump wastes, flies are gathering");
        dump->create_resources(Resource::Ingredient, 2, "fly");
    };
};

void marketplace_task(Building* building)
{
    std::cout << "\tDoing marketplace stuff" << std::endl;
};

void arena_task(Building* arena)
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
    auto removed_it = std::remove_if(
        battle->combatants.begin(),
        battle->combatants.end(),
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
    battle->combatants.erase(
            removed_it,
            battle->combatants.end()
            );

    //print(bodies_to_create << " bodies to create");
    for (int i = 0; i < bodies_to_create; i++)
    {
        auto grave = arena->city->building_by_name("The Graveyard");
        grave->create_resources(Resource::Waste, 1, "Corpse");
    }
};

void mine_task(Building* mine)
{
    std::cout << "\tDoing mine stuff" << std::endl;
    mine->create_resources(Resource::Ingredient, 9, "iron");

    move_if_sized(Resource::Ingredient,
        2, 2,
        mine, mine->city->building_by_name("The Workshop"),
        NO_CB);
};

void forest_task(Building* mine)
{
    std::cout << "\tDoing forest stuff" << std::endl;
    mine->create_resources(Resource::Ingredient, 2, "wood");
};

void Village::update(float dt)
{
    Updateable::update(dt);

    this->update_buildings(dt);
};


void Village::update_buildings(float dt)
{
    printf("\n\nupdating buildings\n");
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
        print("removing " << remove_count);
        from_vs.erase(from_vs.begin(), from_vs.begin()+remove_count);
        callback();
    };
};

void move_if_sized(Resource::ResourceType res_type,
        unsigned int condition_size, unsigned int move_count,
        Building* from_bldg, Building* to_bldg, VoidFunc callback )
{
    int from_size;
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
    print("moving x" << quantity << " " << Resource::type_str(res_type) << " from " << from_bldg->name << " to " << to_bldg->name);
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

template<typename T, typename vectorT>
void create(vectorT& vec, int quantity, std::string name)
{
    vectorT created_resources = vectorT();

    for (int i = 0; i < quantity; i++)
    {
        std::shared_ptr<T> p = std::make_shared<T>(name);
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
    printf("   %s - %.f\n", this->name.c_str(), this->update_clock->start_time);
    this->print_inventory();
    if (update_clock->passed_threshold())
    {
        this->do_task();
        update_clock->reset();
    }
    else
    {
        std::cout << "   \twaiting" << std::endl;
    }
    std::cout << std::endl;
};

void Building::print_inventory()
{
    std::stringstream ss;
    ss << "   ";
    ss << "ING: " << this->ingredients.size();
    ss << " PDT: " << this->products.size();
    ss << " WST: " << this->wastes.size();
    ss << std::endl;

    std::cout << ss.str();

};

void Building::do_task()
{
    if (this->task)
    {
        this->task(this);
    };
};

void test_recipe()
{
    
    vsIngredient inputs = {
        std::make_shared<Ingredient>("grain"),
        std::make_shared<Ingredient>("grain"),
        std::make_shared<Ingredient>("iron")
    } ;

    Recipe recipe = Recipe("test recipe");
    bool result = recipe.is_satisfied(inputs);

    std::cout << "is the recipe satisfied? " << std::boolalpha << result << std::endl << std::endl;

}

void Buildup::main_loop()
{
    Clock game_clock = Clock(CLOCKS_PER_SEC);
    clock_t start_time = clock() / CLOCKS_PER_SEC;

    //test_recipe();
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

Village* init_city(Buildup* buildup)
{
    auto city = new Village(buildup, "Burlington");

    auto farm = std::make_shared<Building>(city, "The Farm", farm_task);
    farm->update_clock->set_threshold(2*CLOCKS_PER_SEC);
    auto dump= std::make_shared<Building>(city, "The Dump", dump_task);
    auto workshop = std::make_shared<Building>(city, "The Workshop", workshop_task);
    auto marketplace = std::make_shared<Building>(city, "The Marketplace", marketplace_task);
    auto arena = std::make_shared<Building>(city, "The Arena", arena_task);
    auto mine = std::make_shared<Building>(city, "The Mine", mine_task);
    auto grave = std::make_shared<Building>(city, "The Graveyard", grave_task);
    auto necro = std::make_shared<Building>(city, "The Underscape", necro_task);
    mine->update_clock->set_threshold(3*CLOCKS_PER_SEC);

    auto mitchell = std::make_shared<Fighter>(arena->city, "Mitchell");
    auto barry = std::make_shared<Fighter>(arena->city, "Barry");
    barry->attrs->health->set_vals(20);
    mitchell->attrs->health->set_vals(20);
    arena->fighters.push_back(mitchell);
    arena->fighters.push_back(barry);


    city->buildings.push_back(farm);
    city->buildings.push_back(dump);
    city->buildings.push_back(workshop);
    city->buildings.push_back(marketplace);
    city->buildings.push_back(arena);
    city->buildings.push_back(mine);
    city->buildings.push_back(grave);
    city->buildings.push_back(necro);

    return city;
};

void Player::update(float dt)
{
    printf("   The Player has %i coins\n", this->coins);
};

int _tmain(int argc, _TCHAR* argv[])
{
    Buildup* buildup = new Buildup();

    auto player = new Player("Jimothy");
    player->coins = 100;
    buildup->player = player;

    auto animal = std::make_shared<Animal>("Tank");

    buildup->city = init_city(buildup);
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
