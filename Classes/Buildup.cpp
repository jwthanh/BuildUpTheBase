#include "Buildup.h"
#include "Fighter.h"
#include "combat.h"
#include "attribute_container.h"
#include "attribute.h"

Buildup::Buildup()
{
    this->city = NULL;
    this->set_target_building(NULL);

    this->fighter = std::make_shared<Fighter>("PlayerAvatar");
	auto fighter_combat = std::make_shared<Combat>("playeravatar combat", this->fighter);
	this->fighter->combat = fighter_combat;
    this->fighter->attrs->health->set_vals(100);

    this->player = NULL;
};

///for building in buildings, sum up their ingredients and return the map
mistIngredient& Buildup::get_all_ingredients()
{
    return this->_all_ingredients;
}

//remove ing type count from list, making sure it doesnt result in less than 0
void Buildup::remove_shared_ingredients_from_all(Ingredient::SubType ing_type, res_count_t removals)
{
    mistIngredient& all_ingredients = this->get_all_ingredients();
    all_ingredients[ing_type] -= removals;

	//make sure there's never less than 0
    if (all_ingredients[ing_type] < 0.0)
    {
        all_ingredients[ing_type] = 0.0;
    };
};

