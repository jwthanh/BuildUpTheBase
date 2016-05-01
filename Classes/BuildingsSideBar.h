#pragma once
#ifndef BUILDINGSSIDEBAR_H
#define BUILDINGSSIDEBAR_H

#include "cocostudio/ActionTimeline/CSLoader.h"
#include "ui/CocosGUI.h"

///Base ListView for sidebar
class SideListView : public cocos2d::ui::ListView
{
    public:
        CREATE_FUNC(SideListView);
        virtual Widget* createCloneInstance() override;
        virtual void copySpecialProperties(Widget* model) override;
        virtual void copyClonedWidgetChildren(Widget* model) override;
};

#endif
