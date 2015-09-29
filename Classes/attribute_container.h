#pragma once
#ifndef ATTRIBUTE_CONTAINER_H
#define ATTRIBUTE_CONTAINER_H

#include <string>
#include <vector>

class HealthAttribute;
class ManaAttribute;
class ArmorAttribute;
class DamageAttribute;
class HungerAttribute;
class SpeedAttribute;

class Actor;

class Damage;
class AttributeContainer
{
    public:
        Actor* owner;

        HealthAttribute* health;
        ManaAttribute* mana;
        ArmorAttribute* armor;
        DamageAttribute* damage;
        HungerAttribute* hunger;
        SpeedAttribute* speed;

        Damage* get_effective_damage(); //include native damage + weapons

        AttributeContainer();
        AttributeContainer(AttributeContainer& other);

        void RegenerateAll();
        void RegenerateHealth();
        void RegenerateMana();
        void RegenerateArmor();
        void RegenerateDamage();
        void RegenerateSpeed();

        void Update();
        std::string PrettyPrint();
        std::vector<std::string> PrettyVector();
        //std::vector<std::string> PrettyVectorColored();
        std::vector<std::string> VerboseVector();

};

#endif
