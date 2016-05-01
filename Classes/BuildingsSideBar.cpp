#include "BuildingsSideBar.h"

USING_NS_CC;

void temp_func()
{
}

ui::Widget* SideListView::createCloneInstance()
{
    return SideListView::create();
}

void SideListView::copyClonedWidgetChildren(ui::Widget* model)
{
    ListView::copyClonedWidgetChildren(model);
}

void SideListView::copySpecialProperties(Widget *widget)
{
    ListView::copySpecialProperties(widget);
}
