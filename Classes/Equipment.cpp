#include "Equipment.h"
#include <assert.h>

#include "Item.h"

EquipmentSlot::EquipmentSlot()
{
    this->_item = NULL;
};

EquipmentSlot::EquipmentSlot(spItem item)
{
    EquipmentSlot();
    this->set_item(item);
};

bool EquipmentSlot::has_item()
{
    return this->_item != NULL;
};

spItem EquipmentSlot::get_item()
{
    return this->_item;
};

void EquipmentSlot::set_item(spItem item)
{
    this->_item = item;
};

Equipment::Equipment()
{
    this->combat_slot = std::unique_ptr<EquipmentSlot>(new EquipmentSlot());
    this->mining_slot = std::unique_ptr<EquipmentSlot>(new EquipmentSlot());
    this->recipe_slot = std::unique_ptr<EquipmentSlot>(new EquipmentSlot());
};

const std::unique_ptr<EquipmentSlot>& Equipment::get_slot_by_type(ItemSlotType slot_type)
{
    if (slot_type == ItemSlotType::Combat) return this->combat_slot;
    else if (slot_type == ItemSlotType::Mining) return this->mining_slot;
    else if (slot_type == ItemSlotType::Recipe) return this->recipe_slot;

    assert(false && "slot type unrecognized");
    return this->combat_slot;
};
