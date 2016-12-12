#ifndef EQUIPMENT_H
#define EQUIPMENT_H

#include "constants.h"

enum class ItemSlotType;

class EquipmentSlot
{
    private:
        spItem _item;

    public:
        EquipmentSlot();
        EquipmentSlot(spItem item);

        bool has_item();
        spItem get_item();
        void set_item(spItem item);

};

/// Tracks the equipped items
class Equipment
{
    public:
        std::unique_ptr<EquipmentSlot> combat_slot;
        std::unique_ptr<EquipmentSlot> mining_slot;
        std::unique_ptr<EquipmentSlot> recipe_slot;

        Equipment();

        const std::unique_ptr<EquipmentSlot>& get_slot_by_type(ItemSlotType slot_type);

};
#endif
