
#include  <sstream>
#include "attribute_container.h"
#include "attribute.h"
#include "combat.h"


AttributeContainer::AttributeContainer()
{
    this->owner = NULL;

    this->health = new HealthAttribute;
    this->mana = new ManaAttribute;
    this->armor = new ArmorAttribute;
    this->damage = new DamageAttribute;
    this->hunger = new HungerAttribute;
    this->speed = new SpeedAttribute;
};

AttributeContainer::AttributeContainer(AttributeContainer& other)
{
    this->owner = other.owner;

    this->health = new HealthAttribute(*other.health);
    this->mana = new ManaAttribute(*other.mana);
    this->armor = new ArmorAttribute(*other.armor);
    this->damage = new DamageAttribute(*other.damage);
    this->hunger = new HungerAttribute(*other.hunger);
    this->speed = new SpeedAttribute(*other.speed);
};

void AttributeContainer::Update()
{
    this->health->update();
    this->mana->update();
    this->armor->update();
    this->damage->update();
    this->hunger->update();
    this->speed->update();
};

Damage* AttributeContainer::get_effective_damage()
{
    int initial_damage = (int)this->damage->current_val;
	return NULL;
};

void AttributeContainer::RegenerateAll()
{
    this->RegenerateHealth();
    this->RegenerateMana();
    this->RegenerateArmor();
    this->RegenerateDamage();
    this->RegenerateSpeed();
};

void AttributeContainer::RegenerateHealth()
{
    this->health->regenerate();
};

void AttributeContainer::RegenerateMana()
{
    this->mana->regenerate();
};

void AttributeContainer::RegenerateArmor()
{
    this->armor->regenerate();
};

void AttributeContainer::RegenerateDamage()
{
    this->damage->regenerate();
};

void AttributeContainer::RegenerateSpeed()
{
    this->speed->regenerate();
};


template<typename AttrClass>
std::string format_verbose(AttrClass* attr)
{
    std::stringstream ss;
    // ss << attr->current_val << "/" << attr->max_val;
    ss << attr->max_val << "pts";
    std::string temp = ss.str();
    ss.str("");
    ss << colfg(AttrClass::attribute_color, temp) << " regen " << attr->regen_rate << pluralize(attr->regen_rate, "pt") << "/" << attr->regen_interval << "turn" << (attr->regen_interval > 1 ? "s" : "");
    return ss.str();
};


std::vector<std::string> AttributeContainer::PrettyVector()
{
    std::string result = "";
    std::vector<std::string> string_vec;

    string_vec.push_back("HCV: "+std::to_string((long double)(int)this->health->current_val));
    string_vec.push_back("HMV: "+std::to_string((long double)(int)this->health->max_val));
    string_vec.push_back("HRR: "+std::to_string((long double)(int)this->health->regen_rate));
    string_vec.push_back("HRI: "+std::to_string((long double)(int)this->health->regen_interval));

    string_vec.push_back("MCV: "+std::to_string((long double)(int)this->mana->current_val));
    string_vec.push_back("MMV: "+std::to_string((long double)(int)this->mana->max_val));
    string_vec.push_back("MRR: "+std::to_string((long double)(int)this->mana->regen_rate));
    string_vec.push_back("MRI: "+std::to_string((long double)(int)this->mana->regen_interval));

    string_vec.push_back("ACV: "+std::to_string((long double)(int)this->armor->current_val));
    string_vec.push_back("AMV: "+std::to_string((long double)(int)this->armor->max_val));
    string_vec.push_back("ARR: "+std::to_string((long double)(int)this->armor->regen_rate));
    string_vec.push_back("ARI: "+std::to_string((long double)(int)this->armor->regen_interval));

    string_vec.push_back("DCV: "+std::to_string((long double)(int)this->get_effective_damage()));
    string_vec.push_back("DMV: "+std::to_string((long double)(int)this->damage->max_val));
    string_vec.push_back("DRR: "+std::to_string((long double)(int)this->damage->regen_rate));
    string_vec.push_back("DRI: "+std::to_string((long double)(int)this->damage->regen_interval));

    string_vec.push_back("SCV: "+std::to_string((long double)(int)this->speed->current_val));
    string_vec.push_back("SMV: "+std::to_string((long double)(int)this->speed->max_val));
    string_vec.push_back("SRR: "+std::to_string((long double)(int)this->speed->regen_rate));
    string_vec.push_back("SRI: "+std::to_string((long double)(int)this->speed->regen_interval));

    return string_vec;
};

std::string AttributeContainer::PrettyPrint()
{
    return "UNIMPLEMENTED 3";
};
