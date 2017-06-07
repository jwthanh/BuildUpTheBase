#ifndef MAINLOOP_H
#define MAINLOOP_H

class MainLoop {
    public:
        static MainLoop* getInstance() {
            static MainLoop sharedInstance;
            return &sharedInstance;
        }

        void schedule();
        void unschedule();
        void update(float dt);

};

#endif
