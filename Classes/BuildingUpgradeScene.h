#ifndef BUILDINGUPGRADESCENE_H
#define BUILDINGUPGRADESCENE_H

#include "GameLayer.h"

namespace cocos2d
{
    namespace ui
    {
        class Layout;
        class Text;
        class ListView;
        class Button;
    }
}

class Building;


class BuildingUpgradeScene : public GameLayer
{
    private:
        std::shared_ptr<Building> building;

    public:
        static BuildingUpgradeScene* create(std::shared_ptr<Building> building);
        static cocos2d::Scene* createScene(std::shared_ptr<Building> building);
        virtual bool init(std::shared_ptr<Building> building);
};
#endif
