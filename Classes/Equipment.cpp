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
    this->inventory = {};

    this->combat_slot = std::shared_ptr<EquipmentSlot>(new EquipmentSlot());
    this->mining_slot = std::shared_ptr<EquipmentSlot>(new EquipmentSlot());
    this->recipe_slot = std::shared_ptr<EquipmentSlot>(new EquipmentSlot());
};

void Equipment::remove_item_from_inventory(spItem item)
{
    this->inventory.erase(
        std::remove(this->inventory.begin(), this->inventory.end(), item),
        this->inventory.end()
    );

    if (this->combat_slot->get_item() == item)
    {
        this->combat_slot->set_item(NULL);
    }
    else if (this->mining_slot->get_item() == item)
    {
        this->mining_slot->set_item(NULL);
    }
    else if (this->recipe_slot->get_item() == item)
    {
        this->recipe_slot->set_item(NULL);
    }

};

std::vector<const std::shared_ptr<EquipmentSlot>> Equipment::get_slots()
{
    std::vector<const std::shared_ptr<EquipmentSlot>> slots;
    slots.push_back(this->combat_slot);
    //const std::vector<std::shared_ptr<EquipmentSlot>&> slots =  { &this->combat_slot, this->mining_slot, this->recipe_slot };
    return slots;
};

const std::shared_ptr<EquipmentSlot>& Equipment::get_slot_by_type(ItemSlotType slot_type)
{
    if (slot_type == ItemSlotType::Combat) return this->combat_slot;
    else if (slot_type == ItemSlotType::Mining) return this->mining_slot;
    else if (slot_type == ItemSlotType::Recipe) return this->recipe_slot;

    assert(false && "slot type unrecognized");
    return this->combat_slot;
};
