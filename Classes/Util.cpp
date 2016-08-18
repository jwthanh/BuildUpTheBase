#include "Util.h"
#include <iomanip>

#include "ui/CocosGUI.h"

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

std::vector<std::string> suffixes = {
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

std::stringstream _humanize_number_spss = std::stringstream();
std::stringstream _humanize_number_ss = std::stringstream();
std::string _humanize_number(long double& value)
{
    float base = 0;
    std::string suffix = suffixes.at(0);

    if (value >= 1000000)
    {
        value /= 1000;
        while (std::round(value) >= 1000)
        {
            value /= 1000;
            base++;
        }

        if (base >= suffixes.size())
        {
            return "Too Damn High";
        }
        else
        {
            suffix = suffixes.at((unsigned int)base);
        }
    }

    //for 1 567 890, we've got 1.5678 so we go to 1 568 then...
    int embiggened = (int)std::round(value * 1000.0);
    //... we divide by 1000 to get the nice 1.568 number and add the suffix
    // the problem is that if we've got 1.0f, it turns into 1.000 instead of 1, so I need to clear the empty 0s

    _humanize_number_spss.str("");
    _humanize_number_spss << std::fixed << std::setprecision(3) << embiggened / 1000.0;
    std::string str = _humanize_number_spss.str();
    str.erase ( str.find_last_not_of('0') + 1, std::string::npos ); //rstrip zeroes

    auto split_string = split(str, '.');

    //add a comma every 3rd digit (on the left of a period)
    int insertPosition = split_string.at(0).length() - 3;
    while (insertPosition > 0) {
        split_string.at(0).insert(insertPosition, ",");
        insertPosition -= 3;
    }

    _humanize_number_ss.str("");
    _humanize_number_ss << split_string.at(0);
    if (split_string.size() > 1) //a_humanize_number_ssumes only two elems in vector
    {
        _humanize_number_ss << "." << split_string.at(1);
    }

    str = _humanize_number_ss.str();

    //remove trailing period
    if (str.back() == '.')
    {
        str.pop_back();
    }

    return str + suffix;
}

std::stringstream _beautify_double_ss;
std::string beautify_double(long double& value)
{
    bool is_negative = value < 0;
    std::string decimal = "";

    if (value < 1000 && std::floor(value) != value) //check for decimal
    {
         _beautify_double_ss.str(""); 
        _beautify_double_ss << std::fixed << std::setprecision(3) << value;
        std::string fixed_value = _beautify_double_ss.str();
        std::string splitted = split(fixed_value, '.').at(1);
        _beautify_double_ss.str("");
        //splitted.erase(splitted.find_last_not_of('0') + 1, std::string::npos); //rstrip zeroes

        if (splitted.empty() != true)
        {
            _beautify_double_ss << "." << splitted.at(0);
        }
        else
        {
            _beautify_double_ss << ".0";
        }
        decimal = _beautify_double_ss.str();
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

std::string test_double(float input)
{
    long double dbl_input = (long double)input;
    auto s = beautify_double(dbl_input);
    CCLOG("%s\n", s.c_str());

    return s;
}

void test_beautify_double()
{
    CCLOG("first should be 1, 10, 100\n");
    test_double(1.0f);
    test_double(10.0f);
    test_double(100.0f);
    test_double(1000.0f);
    test_double(10000.0f);
    test_double(100000.0f);
    test_double(1000000.0f);

    CCLOG("\nfloats, first should be like 5.55f\n");
    test_double(5.55f);
    test_double(5.777f);
    test_double(1234.5678f);
    CCLOG("\na bunch of millions, first should be 1.222b\n");
    test_double(1222333444.0f);
    test_double(523411111111.0f);
    test_double(12312317653411111111.0f);
    test_double(-123.04f);
}

void try_set_enabled(cocos2d::ui::Widget* widget, bool new_value)
{
    if (widget->isEnabled() != new_value)
    {
        widget->setEnabled(new_value);
    }
};
