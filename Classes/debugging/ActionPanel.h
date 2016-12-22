#ifndef ACTIONPANEL_H
#define ACTIONPANEL_H

#include "ui/CocosGUI.h"

///debugging class for playing with an action, like MoveTo or ScaleBy without
///needing to recompile between each and every test
class ActionPanel : public cocos2d::ui::Layout
{
    CREATE_FUNC(ActionPanel);
};

#endif
