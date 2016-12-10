#ifndef EQUIPMENT_H
#define EQUIPMENT_H

#include "constants.h"

class EquipmentSlot
{
    private:
        spItem _item;

    public:
        EquipmentSlot();
        EquipmentSlot(spItem item);

        spItem get_item();
        void set_item(spItem item);

};

/// Tracks the equipped items
class Equipment
{
    private:
        std::unique_ptr<EquipmentSlot> combat_slot;
        std::unique_ptr<EquipmentSlot> mining_slot;
        std::unique_ptr<EquipmentSlot> recipe_slot;

    public:
        Equipment();

};
#endif
