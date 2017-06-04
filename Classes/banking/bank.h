#ifndef BANK_H
#define BANK_H

#include "constants.h"

class BankSerializer;

class Bank
{
    protected:
        static Bank* _instance;

        //should never be less than 0
        res_count_t _total_coins_banked;

    public:
        Bank();
        static Bank* getInstance();

        //takes a number of coins, puts as much as it can into BEATUP->total coins
        // then puts the rest into the bank
        void pocket_or_bank_coins(res_count_t added_coins);

        res_count_t get_total_coins_banked() const;
        void add_total_coins_banked(res_count_t added_value);
        void set_total_coins_banked(res_count_t new_total);

        //takes from total coins and puts it into bank, if it can.
        // must be a positive number
        void transfer_to_bank(res_count_t to_bank);
        void transfer_from_bank(res_count_t from_bank);

        friend class BankSerializer;
};
#endif
