#ifndef PARSER__HH
#define PARSER__HH 

#include <map>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <stdexcept>
#include <stdlib.h>

namespace optionparser
{

enum storage_mode { store_true, store_value, store_mult_values };

struct option
{
    option()
    : value(3)
    {
    }
    bool &found() {return m_found;}
    bool m_found = false;

    bool &required() {return m_required;}
    bool m_required = false;

    std::vector<std::string> value;

    std::string &short_flag() {return m_short_flag;}
    std::string &long_flag() {return m_long_flag;}
    std::string m_short_flag = "", m_long_flag = "";

    storage_mode &mode() {return m_mode;};
    storage_mode m_mode;
};

inline std::string remove_character(std::string str, char c = ' ');

typedef std::map<std::string, option> Archive;

class parser
{
public:
    parser() = default;
    ~parser() = default;

    void eat_arguments(int argc, char const *argv[]);

    void add_option(std::string longoption, std::string shortoption, storage_mode mode, bool required = false, std::string dest = "");
    // void add_option(std::string shortoption, storage_mode mode, std::string key = "", bool required = false);

    template <class T = bool>
    T get_value(std::string key);

// private:



    // void chomp_short_argument(std::string argument);
    void error(const std::string &e);

    Archive m_options;
    std::string m_prog_name;
    // Dictionary m_long_flags, m_short_flags;
    // SwitchBox m_box;
};

void parser::add_option(std::string longoption, std::string shortoption, 
    storage_mode mode, bool required, std::string dest)
{
    if (dest == "")
    {
        dest = remove_character(longoption, '-');
    }
    m_options[dest].short_flag() = shortoption;
    m_options[dest].long_flag() = longoption;
    if (required)
    {
        m_options[dest].required() = true;
    }
    m_options[dest].mode() = mode;
}

void parser::eat_arguments(int argc, char const *argv[])
{
    m_prog_name = argv[0];
    std::vector<std::string> arguments;
    for (int i = 1; i < argc; ++i)
    {
        arguments.push_back(argv[i]);
    }
    // for each argument cluster
    for (int arg = 0; arg < arguments.size(); ++arg)
    {
        auto argument = arguments[arg];
        bool match_found = false;

        for (auto &option : m_options) // for each option set
        {
            auto opt = option.second;

            if ((argument[0] == '-'))
            {
                if (argument[1] == '-')
                {
                    if (argument.find(opt.long_flag()) != std::string::npos)
                    {
                        if (opt.mode() == store_true)
                        {
                            option.second.found() = true;
                        }
                        if (argument.size() > opt.long_flag().size())
                        {
                            option.second.found() = false;
                            if (opt.mode() != store_true)
                            {
                                auto search_point = argument.find_first_of('=');
                                if (search_point == std::string::npos)
                                {
                                    std::string e = "Error, long options (";
                                    e += opt.long_flag();
                                    e += ") require a '=' before a value.";
                                    error(e);
                                }
                                std::string val = argument.substr(search_point + 1);
                                if (val == "")
                                {
                                    std::string e = "Error, argument needed after '='.";
                                    error(e);
                                }
                                if (opt.mode() == store_value)
                                {
                                    option.second.value[0] = val;
                                }
                                else
                                {
                                    option.second.value.push_back(val);
                                }
                                option.second.found() = true;
                                std::cout << "value = " << option.second.value[0] << std::endl;
                            }
                            else
                            {
                                std::string e = "Error, value passed to flag '";
                                e += opt.long_flag();
                                error(e + "', which takes no values.");
                            }              
                        }

                        if ((opt.mode() == store_value) && (option.second.found() == false))
                        {
                            if (++arg >= arguments.size())
                            {
                                std::string e("error, flag '" + opt.long_flag() + "' requires an argument.");
                                error(e);
                            }
                            auto next_arg = arguments[arg];
                            if (next_arg[0] == '-')
                            {
                                std::string e("error, flag '" + opt.long_flag() + "' requires an argument.");
                                error(e);
                            }

                            option.second.value[0] = next_arg;
                            std::cout << "value = " << option.second.value[0] << std::endl;
                        }
                    }
                }

            }
        }
    }
}


void parser::error(const std::string &e)
{
    std::cerr << "In excecutable \'" << m_prog_name << "\':\n" << e << std::endl;
    exit(1);
}

// void parser::chomp_short_argument(std::string argument)
// {

// }

template <class T>
T parser::get_value(std::string key)
{
    return m_options[key].found();
}


template <>
std::string parser::get_value<std::string>(std::string key)
{
    return m_options[key].value[0];
}

template <>
double parser::get_value<double>(std::string key)
{
    return std::stod(m_options[key].value[0]);
}

template <>
float parser::get_value<float>(std::string key)
{
    return std::stof(m_options[key].value[0]);
}

template <>
int parser::get_value<int>(std::string key)
{
    return std::stoi(m_options[key].value[0]);
}

template <>
unsigned int parser::get_value<unsigned int>(std::string key)
{
    return std::stoul(m_options[key].value[0]);
}











inline std::string remove_character(std::string str, const char c)
{
    std::string::iterator end_pos = std::remove(str.begin(), str.end(), c);
    str.erase(end_pos, str.end());
    return str;
}



}

#endif