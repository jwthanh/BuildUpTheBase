#include "Fighter.h"
#include "attribute_container.h"
#include "combat.h"

Fighter::Fighter(Village* city, std::string name) : Nameable(name), Updateable(), city(city) {
    this->attrs = new AttributeContainer();
    this->xp = new Experience();
    this->combat = new Combat("unnamed combat", this);
    this->team = TeamTwo; //default 2 for enemy
};
