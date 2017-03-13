#ifndef BUILDINGDETAILSCENE_H
#define BUILDINGDETAILSCENE_H

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


class BuildingDetailScene : public GameLayer
{
    private:
        std::shared_ptr<Building> building;

    public:
        static BuildingDetailScene* create(std::shared_ptr<Building> building);
        static cocos2d::Scene* createScene(std::shared_ptr<Building> building);
        virtual bool init(std::shared_ptr<Building> building);

        virtual void init_children();
        virtual void init_callbacks(); //these should be changed in the child classes
};

///contains building upgrade details like level and salesmen levels etc
class BuildingUpgrade
{
    public:
};

#endif
