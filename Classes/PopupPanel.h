#ifndef POPUPPANEL_H
#define POPUPPANEL_H

#include <functional>
#include <string>

namespace cocos2d
{
    class ParticleSystemQuad;
    class Node;

    namespace ui
    {
        class Layout;
        class Text;
        class TextBMFont;
        class ScrollView;
        class ListView;
    };
};


//only one at a time, used for flavour text along the bottom
class PopupPanel
{
    private:
        cocos2d::ui::Layout* _layout;

        //initial position to animate to, taken from cocos studio
        float initial_x, initial_y;

    public:
        PopupPanel(cocos2d::ui::Layout* panel);
        virtual void init_layout_touch_handler();
        std::function<void()> on_layout_touched;

        void animate_open();
        void animate_close();

        std::string get_string();
        void set_string(std::string message);
        void set_image(std::string path, bool is_plist = false);

        void set_visible(bool is_visible);

        void play_particle(cocos2d::ParticleSystemQuad* parts);
};
#endif
