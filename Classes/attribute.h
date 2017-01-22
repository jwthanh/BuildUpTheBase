#ifndef ATTRBUTE_H
#define ATTRBUTE_H

class Actor;
class Person;
class Combat;
class Person;
class Item;
class Inventory;
class Tile;

using res_count_t = long double;

class Attribute
{
    private:
        res_count_t tick;
        bool is_ready;

    public:
        res_count_t current_val;
        res_count_t max_val;

        res_count_t regen_rate; //per turn
        res_count_t regen_interval; //per x turns?

        Attribute();
        Attribute(Attribute& other);



        virtual void update();
        virtual void do_tick();
        virtual bool check_is_ready();

        void set_vals(res_count_t both);
        void set_vals(res_count_t current_val, res_count_t max_val);
        void alter_vals(res_count_t both);
        void alter_vals(res_count_t diff_current, res_count_t diff_max);

        // bool CanRegenerate();
        void regenerate();

        float get_val_percentage();

        virtual void add_to_current_val(res_count_t difference);
        virtual void add_to_max_val(res_count_t difference);
        virtual void add_to_regen_rate(res_count_t difference);
        virtual void add_to_regen_interval(res_count_t difference);

        virtual void remove_from_current_val(res_count_t difference);
        virtual void remove_from_max_val(res_count_t difference);
        virtual void remove_from_regen_rate(res_count_t difference);
        virtual void remove_from_regen_interval(res_count_t difference);

        bool is_empty();
        bool is_full();

};

class HealthAttribute : public Attribute
{
    public:
        HealthAttribute();
	//void regenerate();
};

class ManaAttribute : public Attribute
{
    public:
        ManaAttribute();
};

class ArmorAttribute : public Attribute
{
    public:
        ArmorAttribute();
};

class DamageAttribute : public Attribute
{
    public:
        DamageAttribute();
};

class HungerAttribute : public Attribute
{
    public:
        HungerAttribute();
};

class SpeedAttribute : public Attribute
{
    public:
        SpeedAttribute();
};

#endif
