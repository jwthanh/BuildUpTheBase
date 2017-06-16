#ifndef SIMULATEMAINLOOP_H
#define SIMULATEMAINLOOP_H

///
// This handles the welcome back loading, running through the game loop
// so that the game acts like it was running when it was closed
class SimulateMainLoop
{
    public:
        //effectively calls BUILDUP->city->update(dt)
        static bool is_simulating;
        static void simulate(float dt);

        static void generate_WIP_welcome_message();
};
#endif
