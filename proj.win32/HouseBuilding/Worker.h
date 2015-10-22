#pragma once
#ifndef WORKER_H
#define WORKER_H


//
//attaches to a building and builds recipes once certain conditions are hit
//
class Worker : public Nameable
{
    public:
        spBuilding building;

        Worker(spBuilding building, std::string name) : Nameable(name) {
            this->building = building;

        };
};

#endif
