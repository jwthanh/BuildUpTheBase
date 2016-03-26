#include "Worker.h"

Worker::Worker(spBuilding building, std::string name) : Nameable(name) {
    this->building = building;
};
