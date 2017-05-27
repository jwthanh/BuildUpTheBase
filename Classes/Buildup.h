#ifndef BUILDUP_H
#define BUILDUP_H

#include "Clock.h"
#include "constants.h"
#include "Ingredients.h"

class Village;
class Player;

#include "SubTypes.h"

class Buildup
{
    private:
        spBuilding _target_building;

    public:
        Player* player;
        spFighter fighter;
        spFighter brawler;

        spBuilding get_target_building();
        void set_target_building(spBuilding building);

        Village* city;

        static Village* init_city(Buildup*);
        res_count_t count_ingredients();
        res_count_t count_ingredients(IngredientSubType ing_type);

        Clock server_clock;

    private:
        mistIngredient _all_ingredients;
    public:
        mistIngredient& get_all_ingredients();
        void remove_shared_ingredients_from_all(Ingredient::SubType ing_type, res_count_t count);

        Buildup();
        void update(float dt);

        void post_update();
};

#endif
