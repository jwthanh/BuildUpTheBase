#include "Equipment.h"

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
