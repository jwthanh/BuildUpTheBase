#ifndef ACTIONPANEL_H
#define ACTIONPANEL_H

#include "ui/CocosGUI.h"

///debugging class for playing with an action, like MoveTo or ScaleBy without
///needing to recompile between each and every test
class ActionPanel : public cocos2d::ui::Layout
{
    private:
        cocos2d::Node* _target;
        cocos2d::ui::Layout* _panel;

        float _default_x;
        float _default_y;

        // float _duration;
        // float _target_x;
        // float _target_y;
        // float _rate;

    public:
        CREATE_FUNC(ActionPanel);

        virtual bool init();

        //sets the target node to run the action on
        void set_target(cocos2d::Node* target);
        void reset_target();

        void init_action_buttons();

        float get_duration();
        cocos2d::Vec2 get_target_pos();
        float get_target_pos_x();
        float get_target_pos_y();
        float get_rate();
};

#endif
