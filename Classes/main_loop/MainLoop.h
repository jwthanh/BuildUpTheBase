#ifndef MAINLOOP_H
#define MAINLOOP_H

using res_count_t = long double;

class MainLoop {
    public:
        static MainLoop* getInstance() {
            static MainLoop sharedInstance;
            return &sharedInstance;
        }

        res_count_t seconds_played;

        MainLoop();
        void schedule();
        void unschedule();
        void update(float dt);

};

#endif
