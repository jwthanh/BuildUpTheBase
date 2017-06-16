#ifndef SIMULATEMAINLOOP_H
#define SIMULATEMAINLOOP_H
#include "constants.h"
#include "SubTypes.h"

///
// This handles the welcome back loading, running through the game loop
// so that the game acts like it was running when it was closed
class SimulateMainLoop
{
    public:
        //effectively calls BUILDUP->city->update(dt)
        static bool is_simulating;
        static mistIngredient simulate(float dt);

        static void generate_WIP_welcome_message();
};
#endif
