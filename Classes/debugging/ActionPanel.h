#ifndef ACTIONPANEL_H
#define ACTIONPANEL_H

#include "ui/CocosGUI.h"

namespace cocos2d
{
    class ActionInterval;
}

///debugging class for playing with an action, like MoveTo or ScaleBy without
///needing to recompile between each and every test
class ActionPanel : public cocos2d::ui::Layout
{
    private:
        cocos2d::Node* _target;
        cocos2d::ui::Layout* _panel;

        float _default_pos_x;
        float _default_pos_y;
        float _default_scale_x;
        float _default_scale_y;

    public:
        CREATE_FUNC(ActionPanel);

        virtual bool init();

        //sets the target node to run the action on
        void set_target(cocos2d::Node* target);
        void reset_target();

        std::function<cocos2d::ActionInterval*()> current_action;
        void init_action_buttons();
        void init_modifier_buttons();

        float get_duration();
        cocos2d::Vec2 get_target_pos();
        float get_target_pos_x();
        float get_target_pos_y();
        float get_rate();

        //outputs the params to screen to allow for 'saving' //TODO save to file
        void output_parameters(std::string text);
};

#endif
