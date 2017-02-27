#include "Util.h"

#include <iomanip>
#include <sstream>

#include "base/CCDirector.h"
#include "ui/UIWidget.h"

#include "utilities/vibration.h"
#include "SoundEngine.h"

float sx (float x, bool do_scale)
{
    if (!do_scale)
    {
        return x;
    }
    auto frs = cocos2d::Director::getInstance()->getOpenGLView()->getFrameSize();
    return (float)(x * (frs.width / 960.0));
};

float sy (float y, bool do_scale)
{
    if (!do_scale)
    {
        return y;
    }
    auto frs = cocos2d::Director::getInstance()->getOpenGLView()->getFrameSize();
    return (float)(y * (float)(frs.height / 640.0));
};

cocos2d::Vec2 sv(cocos2d::Vec2 input)
{
    return cocos2d::Vec2(sx(input.x, true), sy(input.y, true));
}

cocos2d::Vec2 get_relative(cocos2d::Size input, cocos2d::Vec2 relative_pos)
{
    return cocos2d::Vec2(
        input.width * relative_pos.x,
        input.height * relative_pos.y
    );
};

cocos2d::Vec2 get_relative_middle(cocos2d::Size input)
{
    return cocos2d::Vec2(
        input.width * 0.5f,
        input.height * 0.5f
    );
};

cocos2d::Vec2 get_center_pos(float offset_x, float offset_y)
{
    cocos2d::Size visibleSize = cocos2d::Director::getInstance()->getVisibleSize();
    cocos2d::Vec2 origin = cocos2d::Director::getInstance()->getVisibleOrigin();
    cocos2d::Vec2 center_pos = cocos2d::Vec2(
        origin.x + visibleSize.width / 2.0f,
        origin.y + visibleSize.height / 2.0f
    );

    center_pos.x += offset_x;
    center_pos.y += offset_y;

    return center_pos;
}


void log_vector(cocos2d::Vec2 vector, std::string message)
{
    std::stringstream ss;
    ss << message << ": " << vector.x << " " << vector.y;
    CCLOG("%s", ss.str().c_str());
};

std::vector<std::string>& split(const std::string &s, char delim, std::vector<std::string> &elems) {
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}


std::vector<std::string> split(const std::string &s, char delim) {
    std::vector<std::string> elems;
    split(s, delim, elems);
    return elems;

}

const std::vector<const std::string> suffixes = {
    "",
    "M",
    "B",
    "T",
    "Qa",
    "Qi",
    "Sx",
    "Sp",
    "Oc",
    "No",
    "Dc",
    "UnD",
    "DoD",
    "TrD",
    "QaD",
    "QiD",
    "QuD",
    "SxD",
    "SpD",
    "OcD",
    "NoD",
    "Vin"
};
unsigned int suffixes_size = suffixes.size();

std::stringstream _humanize_number_spss = std::stringstream();
std::stringstream _humanize_number_ss = std::stringstream();
std::string _humanize_number(long double& value)
{
    unsigned int base = 0;
    std::string suffix = suffixes[base];

    std::string out_str;
    out_str.reserve(16); //dont know if this helps

    if (value >= 1000000)
    {
        value /= 1000;
        while (std::round(value) >= 1000)
        {
            value /= 1000;
            base++;
        }

        if (base >= suffixes_size)
        {
            return "Too Damn High";
        }
    }

    suffix = suffixes[base];

    //for 1 567 890, we've got 1.5678 so we go to 1 568 then...
    int embiggened = (int)std::round(value * 1000.0);

    //... we divide by 1000 to get the nice 1.568 number and add the suffix ...
    _humanize_number_spss.str("");
    _humanize_number_spss << std::fixed << std::setprecision(2) << embiggened / 1000.0;

    //... if we've got 1.0f, it turns into 1.000 instead of 1, so clear the empty 0s
    out_str.assign(_humanize_number_spss.str() );
    out_str.erase( out_str.find_last_not_of('0') + 1, std::string::npos ); //rstrip zeroes

    std::string split_string = out_str;
    std::string::size_type end = split_string.find('.');
    std::string post_period = "";
    if (end != std::string::npos)
    {
        post_period = split_string.substr(end, std::string::npos);
        split_string = split_string.substr(0, end);
    }

    //add a comma every 3rd digit (on the left of a period)
    int insertPosition = split_string.length() - 3;
    while (insertPosition > 0) {
        split_string.insert(insertPosition, ",");
        insertPosition -= 3;
    }

    _humanize_number_ss.str("");
    _humanize_number_ss << split_string;
    _humanize_number_ss << post_period;

    out_str.assign(_humanize_number_ss.str());

    //remove trailing period
    if (out_str.back() == '.')
    {
        out_str.pop_back();
    }

    return out_str.append(suffix);
}

std::stringstream _beautify_double_ss;
std::string beautify_double(long double& value)
{
    bool is_negative = value < 0;
    std::string decimal = "";

    //special case for below 1000
    if (value < 1000 && std::floor(value) != value) //check for decimal
    {
        _beautify_double_ss.str("");
        _beautify_double_ss << std::fixed << std::setprecision(2) << value;
        std::string fixed_value = _beautify_double_ss.str();
        fixed_value.erase(fixed_value.find_last_not_of('0') + 1, std::string::npos);

        auto period_pos = fixed_value.find('.');
        std::string post_period = ".0";
        if (period_pos != std::string::npos)
        {
            post_period = fixed_value.substr(period_pos, std::string::npos);
            fixed_value = fixed_value.substr(0, period_pos);
        }

        if (post_period != ".") //dont include only a period for a decimal
        {
            decimal = post_period;
        }

    }

    //get rid of decimal bits of the number, already take care of
    long double plain_value = std::floor(std::abs(value));

    std::string output = _humanize_number(plain_value);

    output = output + decimal;


    if (is_negative)
    {
        output = "-" + output;
    }

    return output;
}

std::string test_double(float input, std::string expected)
{
    long double dbl_input = (long double)input;
    std::string output = beautify_double(dbl_input);
    if (output != expected){
        CCLOG("FAILURE\ninput:\n%f\noutput:\n%s\nexpected:\n%s", input, output.c_str(), expected.c_str());
    }

    return output;
}

void test_beautify_double()
{
    CCLOG("starting test_beautify_double");

    //first should be 1, 10, 100
    test_double(1.0f, "1");
    test_double(10.0f, "10");
    test_double(100.0f, "100");
    test_double(1000.0f, "1,000");
    test_double(10000.0f, "10,000");
    test_double(100000.0f, "100,000");
    test_double(1000000.0f, "1M");

    //floats, first should be like 5.55f
    test_double(5.55f, "5.55");
    test_double(5.777f, "5.78");
    test_double(1234.5678f, "1,234");

    //a bunch of millions, first should be 1.222b
    test_double(1222333444.0f, "1.22B");
    test_double(523411111111.0f, "523.41B");
    test_double(12312317653411111111.0f, "12.31Qi");
    test_double(-123.04f, "-123.04");

    CCLOG("completed test_beautify_double");
}

void try_set_enabled(cocos2d::ui::Widget* widget, bool new_value)
{
    if (widget->isEnabled() != new_value)
    {
        widget->setEnabled(new_value);
    }
};

void try_set_visible(cocos2d::Node* node, bool new_value)
{
    if (node->isVisible() != new_value)
    {
        node->setVisible(new_value);
    }
};

void bind_touch_ended(cocos2d::ui::Widget* widget, std::function<void(void)> callback)
{
    auto touch_handler = [callback](cocos2d::Ref*, cocos2d::ui::Widget::TouchEventType evt)
    {
        if (evt == cocos2d::ui::Widget::TouchEventType::ENDED)
        {
            do_vibrate(5);
            callback();
            SoundLibrary::play_general_widget_touched();
        }
    };
    widget->addTouchEventListener(touch_handler);
};
