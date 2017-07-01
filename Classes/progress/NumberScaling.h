#ifndef NUMBERSCALING_H
#define NUMBERSCALING_H

#include "constants.h"

enum class BuildingTypes;
res_count_t get_building_upgrade_cost(BuildingTypes building_type, res_count_t building_level);

#endif
