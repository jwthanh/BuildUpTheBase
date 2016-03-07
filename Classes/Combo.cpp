#include "Combo.h"

#include "Beatup.h"
#include "ProgressBar.h"
#include "Clock.h"
#include "Util.h"
#include "SoundEngine.h"

USING_NS_CC;


Combo::Combo(Beatup* beatup, std::string id_key, std::string name) : Buyable(id_key)
{
    this->beatup = beatup;

    this->activate_count = 0;

    this->name = name;
    this->id_key = id_key;

    this->_shop_cost = 50;

    this->shake_time = 1.5;

    this->order_is_shown = false;

    this->stamina_regen = 6.66f; //unset symbol

    this->can_activate = true;

    this->is_combo_met = false;
    this->last_matched_punch_idx = -1;
    this->hand_order = new std::vector<FistHands>;

    this->defaults.cooldown = 5.0f;
    this->cooldown = this->defaults.cooldown;
    this->cooldown_clock = new Clock(this->cooldown);

    this->cd_bar = new ProgressBar(beatup, "combo_bar.png", "combo_mid.png");
    this->cd_bar->update_string(name); //TODO renable once bars work
    this->cd_bar->setScale(sx(5));

    this->cd_bar->front_timer->setAnchorPoint(Vec2(0.5f, 0.5f));
    this->cd_bar->back_timer->setAnchorPoint(Vec2(0.5f, 0.5f));

    this->cd_bar->front_timer->setPosition(0, 0);
    this->cd_bar->back_timer->setPosition(0, 0);

    this->order_lbl = NULL;
};

bool Combo::can_punch()
{
    return this->can_activate;
};

void Combo::update(float dt)
{
    if (this->beatup->get_level_over())
    {
        if (this->order_lbl != NULL)
        {
            this->order_lbl->setVisible(false);
        };

        this->cd_bar->setVisible(false);
    };
 
    //FIXME actuall remove this
    if (false && this->get_been_bought() && !this->beatup->get_level_over())
    {
        this->cd_bar->front_timer->setVisible(true);
        this->cd_bar->back_timer->setVisible(false); //FIXME hack to make the back timer always invisible
        this->cd_bar->background->setVisible(true);
    }
    else
    {
        this->cd_bar->front_timer->setVisible(false);
        this->cd_bar->back_timer->setVisible(false);
        this->cd_bar->background->setVisible(false);
    };

    if (this->cooldown_clock->is_active())
    {
        this->cooldown_clock->update(dt);
        float percentage = this->cooldown_clock->get_percentage();
        if (percentage > 1.0f) { percentage = 1.0f; };
        this->cd_bar->scroll_to_percentage(percentage);

        if (this->order_lbl != NULL)
        {
            this->order_lbl->setColor(Color3B::GRAY);
        };

        if (this->cooldown_clock->passed_threshold())
        {
            this->can_activate = true;
            this->cd_bar->front_timer->setPercentage(100);

            if (this->order_lbl != NULL) 
            {
                this->order_lbl->setColor(Color3B::WHITE); 
            };
        };
    }
};

void Combo::hide_order()
{
    if (this->order_lbl != NULL)
    {
        this->order_lbl->setVisible(false);
    };
    this->order_is_shown = false;
};

std::string Combo::order_to_string(std::vector<FistHands>* hand_order)
{
    auto string_map = std::map<FistHands, std::string>();
    string_map[FistHands::Left] = "L";
    string_map[FistHands::Right] = "R";
    string_map[FistHands::RCharge] = "cR";
    string_map[FistHands::LCharge] = "cL";

    std::stringstream ss;
    for (auto h : *hand_order)
    {
        ss << string_map[h] << ",";
    }

    return ss.str();
};
void Combo::color_combo_chars()
{
    //count the characters, accounting for commas and two-character strings
    //like lC and rC
    int total_steps = this->last_matched_punch_idx;
    if (this->last_matched_punch_idx != -1)
    {
        total_steps *= 2; //each element has a comma

        //find all the LCharge and Rcharge instances in the combo, appending
        //thme to the total steps
        int lc_count = std::count(
            this->hand_order->begin(),
            this->hand_order->begin()+this->last_matched_punch_idx+1,
            FistHands::LCharge
        );
        total_steps += lc_count;

        int rc_count = std::count(
            this->hand_order->begin(),
            this->hand_order->begin()+this->last_matched_punch_idx+1,
            FistHands::RCharge
        );
        total_steps += rc_count;

        for (int i = 0; i <= total_steps; i++)
        {
            const char chr = this->order_lbl->getString().at(i);

            //if this is a comma, skip it... for some reason
            bool comma = std::string(&chr) == ",";
            if (comma) { i++; }

            auto chr_sprite = this->order_lbl->getLetter(i);
            if (chr_sprite != NULL)
            {
                chr_sprite->setColor(Color3B::GREEN);

            }

        };

    };
};

void Combo::activate()
{
    this->is_combo_met = true;
    this->last_matched_punch_idx = -1;

    this->activate_count += 1;

    auto lbl = this->order_lbl;
    if (lbl != NULL)
    {
        lbl->setColor(Color3B(100, 100, 100));
    }

    SoundEngine::play_sound("sounds\\new\\real_punch\\C_combo_1.mp3");

    this->beatup->add_to_stamina(this->stamina_regen);
};

bool Combo::handle_hand(FistHands hand)
{
    if (this->hand_order->at(this->last_matched_punch_idx + 1) == hand)
    {
        this->last_matched_punch_idx++;

        int size = this->hand_order->size();
        if (this->last_matched_punch_idx + 1 >= size)
        {
            this->activate();
            return true;
        };

        auto lbl = this->order_lbl;
        if (lbl != NULL)
        {
            this->color_combo_chars();
        }
        return false;
    }
    else
    {
        this->last_matched_punch_idx = -1;
        this->is_combo_met = false;

        auto lbl = this->order_lbl;
        if (lbl != NULL)
        {
            lbl->setColor(Color3B::WHITE);
        }
        return false;
    };
};

