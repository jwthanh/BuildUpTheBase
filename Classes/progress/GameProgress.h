#ifndef GAMEPROGRESS_H
#define GAMEPROGRESS_H

class GameProgress
{
    private:
        static GameProgress* _instance;

    public:
        static GameProgress* getInstance();

};
#endif
