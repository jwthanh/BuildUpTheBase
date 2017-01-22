#include "attribute.h"
Attribute::Attribute()
{
    this->tick = 0;
    this->is_ready = false;
    this->current_val = 999;
    this->max_val = 999;
    this->regen_rate = 1;
    this->regen_interval = 1;

};

Attribute::Attribute(Attribute& other)
{
    this->tick = other.tick;
    this->is_ready = other.is_ready;
    this->current_val = other.current_val;
    this->max_val = other.max_val;
    this->regen_rate = other.regen_rate;
    this->regen_interval = other.regen_interval;

};

float Attribute::get_val_percentage()
{
    float percent = (float)(this->current_val)/ ((float)this->max_val) * 100;
    return percent;

};

void Attribute::update()
{
    this->do_tick();

    if (this->check_is_ready())
    {
        this->regenerate();
        // this->add_to_current_val(this->regen_rate);
    };

};

void Attribute::set_vals(res_count_t both)
{
    this->set_vals(both, both);
};

void Attribute::set_vals(res_count_t current_val, res_count_t max_val)
{
    this->current_val = current_val;
    this->max_val = max_val;
};

void Attribute::alter_vals(res_count_t both)
{
    this->alter_vals(both, both);
};

void Attribute::alter_vals(res_count_t diff_current, res_count_t diff_max)
{
    this->current_val += diff_current;
    this->max_val += diff_max;
};

void Attribute::do_tick()
{

    //if there's no regen rate, freeze and reset regen
    if (this->regen_rate == 0)
    {
        this->is_ready = false;
        this->tick = 1;
        return;
    }

    if (this->tick < this->regen_interval)
    {
        this->tick += 1;
        this->is_ready = false;
    }
    else
    {
        this->tick = 1;
        this->is_ready = true;
    };

};

void Attribute::add_to_current_val(res_count_t difference)
{
    if (difference != 0)
    {
        this->current_val += difference;
        if (this->current_val > this->max_val)
        {
            this->current_val = this->max_val;
        };
    };

};

void Attribute::add_to_max_val(res_count_t difference)
{
    if (difference != 0)
    {
        this->max_val += difference;
    };

};

void Attribute::add_to_regen_rate(res_count_t difference)
{
    if (difference != 0)
    {
        this->regen_rate += difference;
    };

};

void Attribute::add_to_regen_interval(res_count_t difference)
{
    if (difference != 0)
    {
        this->regen_interval += difference;
    };

};

void Attribute::remove_from_current_val(res_count_t difference)
{
    if (difference != 0)
    {
        this->current_val -= difference;
    };

};

void Attribute::remove_from_max_val(res_count_t difference)
{
    if (difference != 0)
    {
        this->max_val -= difference;
    };

};

void Attribute::remove_from_regen_rate(res_count_t difference)
{
    if (difference != 0)
    {
        this->regen_rate -= difference;
    };

};

void Attribute::remove_from_regen_interval(res_count_t difference)
{
    if (difference != 0)
    {
        this->regen_interval -= difference;
    };

}

bool Attribute::is_empty()
{
    return this->current_val <= 0.0f;
}

bool Attribute::is_full()
{
    return this->current_val >= this->max_val;
};

// bool Attribute::CanRegenerate()
// {
//     //TODO add check to see if has enough intervals has been passed
//     return true;
// };

void Attribute::regenerate()
{
    if (this->check_is_ready())
    {
        this->current_val += this->regen_rate;
        if (this->current_val > this->max_val)
        {
            this->current_val = this->max_val;
        }
    };

};

bool Attribute::check_is_ready()
{
    return this->is_ready;
};

//void HealthAttribute::regenerate()
//{
//	Attribute::regenerate();
//
//};

HealthAttribute::HealthAttribute() : Attribute()
{
    this->max_val = 100;
    this->current_val = this->max_val;
    this->regen_interval = 3;

};

ManaAttribute::ManaAttribute() : Attribute()
{
    this->max_val = 25;
    this->current_val = this->max_val;

    this->regen_rate = 1;
    this->regen_interval = 3;

};

ArmorAttribute::ArmorAttribute() : Attribute()
{
    this->max_val = 99999;
    this->current_val = 1;

    this->regen_rate = 0;
    // this->max_val = 1;
    // this->current_val = this->max_val;

    // this->regen_rate = 0;
};

DamageAttribute::DamageAttribute() : Attribute()
{
    this->max_val = 99999;
    this->current_val = 10;

    this->regen_rate = 0;

}

HungerAttribute::HungerAttribute() : Attribute()
{
    this->max_val = 999999;
    this->current_val = 0;

    this->regen_rate = 2;

};

SpeedAttribute::SpeedAttribute() : Attribute()
{
    this->max_val = 1000;
    this->current_val = 1000;

    this->regen_rate = 2;
    this->regen_interval = 3;

};
