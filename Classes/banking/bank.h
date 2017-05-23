#ifndef BANK_H
#define BANK_H

#include "constants.h"

class BankSerializer;

class Bank
{
    protected:
        static Bank* _instance;

        res_count_t _total_coins_banked;

    public:
        Bank();
        static Bank* getInstance();

        res_count_t get_total_coins_banked() const;
        void add_total_coins_banked(res_count_t added_value);
        void set_total_coins_banked(res_count_t added_value);

        friend class BankSerializer;


};
#endif
