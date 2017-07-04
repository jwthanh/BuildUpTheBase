#ifndef BUILDINGTYPES_H
#define BUILDINGTYPES_H
#include <map>

enum class BuildingTypes
{
    TheFarm = 0,
    TheArena,
    TheUnderscape,
    TheMarketplace,
    TheDump,
    TheWorkshop,
    TheMine,
    TheGraveyard,
    TheForest
};

const std::map<BuildingTypes, std::string> BuildingTypes_to_name = {
    { BuildingTypes::TheFarm, "The Farm" },
    { BuildingTypes::TheArena, "The Arena" },
    { BuildingTypes::TheUnderscape, "The Underscape" },
    { BuildingTypes::TheMarketplace, "The Marketplace" },
    { BuildingTypes::TheDump, "The Dump" },
    { BuildingTypes::TheWorkshop, "The Workshop" },
    { BuildingTypes::TheMine, "The Mine" },
    { BuildingTypes::TheGraveyard, "The Graveyard" },
    { BuildingTypes::TheForest, "The Forest" }
};

const std::map<std::string, BuildingTypes> BuildingTypes_to_string = {
    { "The Farm", BuildingTypes::TheFarm },
    { "The Arena", BuildingTypes::TheArena },
    { "The Underscape", BuildingTypes::TheUnderscape },
    { "The Marketplace", BuildingTypes::TheMarketplace },
    { "The Dump", BuildingTypes::TheDump },
    { "The Workshop", BuildingTypes::TheWorkshop },
    { "The Mine", BuildingTypes::TheMine },
    { "The Graveyard", BuildingTypes::TheGraveyard },
    { "The Forest", BuildingTypes::TheForest }
};

const std::map<BuildingTypes, res_count_t> BuildingTypes_to_base_upgrade_cost{
    { BuildingTypes::TheFarm       , 10.0L },
    { BuildingTypes::TheDump       , 100.0L },
    { BuildingTypes::TheUnderscape , 1000.0L },
    { BuildingTypes::TheArena      , 10000.0L },
    { BuildingTypes::TheWorkshop   , 100000.0L },
    { BuildingTypes::TheMine       , 5000000.0L },
    { BuildingTypes::TheMarketplace, 10000000.0L },
    { BuildingTypes::TheGraveyard  , 25000000.0L },
    { BuildingTypes::TheForest     , 1000000000.0L }
};


#endif
