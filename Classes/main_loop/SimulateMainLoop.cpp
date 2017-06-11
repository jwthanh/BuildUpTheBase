#include "SimulateMainLoop.h"

#include "constants.h"
#include "GameLogic.h"
#include "Buildup.h"
#include "HouseBuilding.h"

bool SimulateMainLoop::is_simulating = false;

void SimulateMainLoop::simulate(float dt)
{
    SimulateMainLoop::is_simulating = true;
    //TODO maybe simulate it for up to an hour, and multiply that output by
    //the hours remaining. Alternatively, rewrite it somehow so that it
    //accurately estimates it from nothing.
    BUILDUP->city->update(dt);

    SimulateMainLoop::is_simulating = false;
};
