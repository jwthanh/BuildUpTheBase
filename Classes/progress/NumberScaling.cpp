#include "NumberScaling.h"
#include "HouseBuilding.h"
#include "Util.h"

res_count_t get_building_upgrade_cost(BuildingTypes building_type, res_count_t building_level)
{
    res_count_t base_shop_cost = BuildingTypes_to_base_upgrade_cost.at(building_type);
    res_count_t shop_cost = scale_number(base_shop_cost, static_cast<res_count_t>(building_level)-1.0L, 3.5L);
    return shop_cost;
};
