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

std::string Ingredient::type_to_string(Ingredient::Type type)
{
    std::string result = "none";
    if (type == Ingredient::Grain) return "grain";
    else if (type == Ingredient::Iron) return "iron";

    return result;
};

Ingredient::Type Ingredient::string_to_type(std::string string_type)
{
    Ingredient::Type result = Ingredient::Type::None;
    if (string_type == "grain") return Ingredient::Grain;
    else if (string_type == "iron") return Ingredient::Iron;

    return result;

};

void farm_task(Building* farm)
{
    std::cout << "\tDoing farm stuff";

    int new_products = 10;
    farm->create_resources(Resource::Ingredient, new_products, "grain");

    std::cout << ", aka generating grain ingredients" << std::endl;

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
        workshop->ingredients.pop_back();
        workshop->create_resources(Resource::Product, 2, "Pie");
    }
    if (workshop->products.size() > 0)
    {
        workshop->products.pop_back();
        workshop->create_resources(Resource::Waste, 1, "dead product");
    }

    VoidFunc pay = [workshop](){
        workshop->city->buildup->player->coins += 3;
    };
    remove_if_sized(workshop->products, 5, 5, pay);

    std::cout << "\tDoing workshop stuff" << std::endl;

    if (workshop->wastes.size() > 2)
    {
        Animal animal = Animal("Horse");
        animal.b2b_transfer(
            workshop,
            workshop->city->building_by_name("The Dump"),
            Resource::ResourceType::Waste,
            1
            );
    };
};

void dump_task(Building* building)
{
    std::cout << "\tDoing dump stuff" << std::endl;
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
        battle->combatants.push_back(fighter);
    };

    battle->do_battle();
};

void mine_task(Building* mine)
{
    std::cout << "\tDoing mine stuff" << std::endl;
    mine->create_resources(Resource::Ingredient, 9, "iron");
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
        from_vs.erase(from_vs.begin(), from_vs.begin()+remove_count);
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
        std::move(from_vs.begin(), std::next(from_vs.begin(), quantity), std::back_inserter(to_vs));
    }
};

void Animal::b2b_transfer(Building* from_bldg, Building* to_bldg, Resource::ResourceType res_type, int quantity)
{
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
        std::shared_ptr<T> p = std::make_shared<T>("grain");
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
    printf("   %s - time at %.f\n", this->name.c_str(), this->update_clock->start_time);
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
    ss << "I: " << this->ingredients.size();
    ss << " P: " << this->products.size();
    ss << " W: " << this->wastes.size();
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

void Buildup::main_loop()
{
    Clock game_clock = Clock(CLOCKS_PER_SEC);
    clock_t start_time = clock() / CLOCKS_PER_SEC;

    int current_ticks = 0;
    while (true)
    {
        game_clock.update((float)current_ticks);

        current_ticks = clock() / CLOCKS_PER_SEC - start_time;
        if (game_clock.passed_threshold())
        {
            this->city->update(game_clock.start_time);
            this->player->update(game_clock.start_time);

            game_clock.reset();

            current_ticks = 0;
            start_time = clock() / CLOCKS_PER_SEC;
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
    mine->update_clock->set_threshold(3*CLOCKS_PER_SEC);

    arena->fighters.push_back(std::make_shared<Fighter>(arena->city, "Mitchell"));
    arena->fighters.push_back(std::make_shared<Fighter>(arena->city, "Barry"));


    city->buildings.push_back(farm);
    city->buildings.push_back(dump);
    city->buildings.push_back(workshop);
    city->buildings.push_back(marketplace);
    city->buildings.push_back(arena);
    city->buildings.push_back(mine);

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

