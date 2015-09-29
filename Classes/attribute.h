#ifndef ATTRBUTE_H
#define ATTRBUTE_H

class Actor;
class Person;
class Combat;
class Person;
class Item;
class Inventory;
class Tile;

class Attribute 
{
    private:
        int tick;
        bool is_ready;

    public:
        double current_val;
        double max_val;

        double regen_rate; //per turn
        double regen_interval; //per x turns? 

        Attribute();
        Attribute(Attribute& other);

        static const TCODColor attribute_color;


        virtual void update();
        virtual void do_tick();
        virtual bool check_is_ready();

        void set_vals(int both);
        void set_vals(int current_val, int max_val);
        void alter_vals(int both);
        void alter_vals(int diff_current, int diff_max);

        // bool CanRegenerate();
        void regenerate();
        
        float get_val_percentage();

        virtual void add_to_current_val(int difference);
        virtual void add_to_max_val(int difference);
        virtual void add_to_regen_rate(int difference);
        virtual void add_to_regen_interval(int difference);

        virtual void remove_from_current_val(int difference);
        virtual void remove_from_max_val(int difference);
        virtual void remove_from_regen_rate(int difference);
        virtual void remove_from_regen_interval(int difference);

};

class HealthAttribute : public Attribute
{
    public:
        static const TCODColor attribute_color;
        HealthAttribute();
	//void regenerate();
};

class ManaAttribute : public Attribute
{
    public:
        static const TCODColor attribute_color;
        ManaAttribute();
};

class ArmorAttribute : public Attribute
{
    public:
        static const TCODColor attribute_color;
        ArmorAttribute();
};

class DamageAttribute : public Attribute
{
    public:
        static const TCODColor attribute_color;
        DamageAttribute();
};

class HungerAttribute : public Attribute
{
    public:
        static const TCODColor attribute_color;
        HungerAttribute();
};

class SpeedAttribute : public Attribute
{
    public:
        static const TCODColor attribute_color;
        SpeedAttribute();
};

#endif
