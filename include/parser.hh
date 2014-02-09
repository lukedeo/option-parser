#ifndef PARSER__HH
#define PARSER__HH 

#include <map>
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <algorithm>
#include <stdexcept>
#include <stdlib.h>
#include <unordered_set>


namespace optionparser
{
//----------------------------------------------------------------------------
enum storage_mode { store_true, store_value, store_mult_values };

struct option
{
    option() : value(0) {}
    bool &found() {return m_found;}
    bool m_found = false;

    bool &required() {return m_required;}
    bool m_required = false;

    std::vector<std::string> value;

    std::string &short_flag() {return m_short_flag;}
    std::string &long_flag() {return m_long_flag;}
    std::string m_short_flag = "", m_long_flag = "";

    storage_mode &mode() {return m_mode;};
    storage_mode m_mode = store_true;


    std::string &help() {return m_help;};
    std::string m_help = "";

    std::string &dest() {return m_dest;};
    std::string m_dest = "";
};

inline std::string remove_character(std::string str, char c = ' ');

typedef std::map<std::string, option> Archive;
typedef std::map<std::string, std::string> Dictionary;

class parser
{
public:
    parser() = default;
    ~parser() = default;

    void eat_arguments(int argc, char const *argv[]);

    void add_option(std::string longoption, std::string shortoption, 
        storage_mode mode, bool required = false, std::string help = "", std::string dest = "");


    // void add_option(std::string longoption, 
    //     storage_mode mode, bool required = false, std::string help = "", std::string dest = "");
    // void add_option(option &o);

    // void add_option(const option &o);



    // void add_option(std::string shortoption, storage_mode mode, std::string key = "", bool required = false);

    template <class T = bool>
    T get_value(std::string key);

// private:

    void help();



    // void chomp_short_argument(std::string argument);
    void error(const std::string &e);

    Archive m_options;
    std::string m_prog_name;
    std::unordered_set<std::string> with_val, without_val;
    Dictionary m_opt_map;
    // Dictionary m_long_flags, m_short_flags;
    // SwitchBox m_box;
};
//----------------------------------------------------------------------------
void parser::add_option(std::string longoption, std::string shortoption, 
    storage_mode mode, bool required, std::string help, std::string dest)
{
    if (dest == "")
    {
        dest = remove_character(longoption, '-');
    }
    m_options[dest].short_flag() = shortoption;
    m_options[dest].long_flag() = longoption;
    m_options[dest].help() = help;
    m_options[dest].dest() = dest;
    if (required)
    {
        m_options[dest].required() = true;
    }
    m_options[dest].mode() = mode;
    if (mode != store_true)
    {
        with_val.insert(remove_character(shortoption, '-'));
    }
    else
    {
        without_val.insert(remove_character(shortoption, '-'));
    }
    m_opt_map[remove_character(shortoption, '-')] = dest;
}
// void parser::add_option(option &o)
// {
//     if (o.dest() == "")
//     {
//         o.dest() = remove_character(o.long_flag(), '-');
//     }
//     if (o.mode() != store_true)
//     {
//         with_val.insert(remove_character(o.short_flag(), '-'));
//     }
//     else
//     {
//         without_val.insert(remove_character(o.short_flag(), '-'));
//     }
//     m_options[o.dest()] = o;
//     m_opt_map[remove_character(o.short_flag(), '-')] = o.dest();
// }

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
        // std::cout << "condsidering: " << arguments[arg] << std::endl;
        auto argument = arguments[arg];
        bool match_found = false;

        for (auto &option : m_options) // for each option set
        {
            auto opt = option.second;

            if (argument[0] == '-')
            {
                if (argument.size() == 1)
                {
                    error("A flag needs a letter...");
                }
                if (argument[1] == '-')
                {
                    if (argument.find(opt.long_flag()) != std::string::npos)
                    {
                        if (opt.mode() == store_true)
                        {
                            option.second.found() = true;
                            match_found = true;
                            break;
                        }
                        
                        if(argument.size() > opt.long_flag().size())
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
                                    option.second.value.clear();
                                    option.second.value.push_back(val);
                                }
                                else
                                {
                                    option.second.value.push_back(val);
                                    if (++arg < arguments.size())
                                    {
                                        auto next_arg = arguments[arg];
                                        while (next_arg[0] != '-')
                                        {
                                            option.second.value.push_back(next_arg);
                                            if (++arg >= arguments.size())
                                            {
                                                break;
                                            }
                                            next_arg = arguments[arg];
                                        }
                                        arg--;
                                    }
                                }
                                option.second.found() = true;
                                match_found = true;
                                break;
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
                            option.second.value.clear();
                            option.second.value.push_back(next_arg);
                            option.second.found() = true;
                            match_found = true;
                            break;
                        }
                        if ((opt.mode() == store_mult_values) && (option.second.found() == false))
                        {
                            if (++arg >= arguments.size())
                            {
                                std::string e("error, flag '" + opt.long_flag() + "' requires at least one argument.");
                                error(e);
                            }
                            auto next_arg = arguments[arg];
                            if (next_arg[0] == '-')
                            {
                                std::string e("error, flag '" + opt.long_flag() + "' requires at least one argument.");
                                error(e);
                            }
                            while (next_arg[0] != '-')
                            {

                                option.second.value.push_back(next_arg);
                                if (++arg >= arguments.size())
                                {
                                    break;
                                }
                                next_arg = arguments[arg];
                            }
                            arg--;
                            option.second.found() = true;
                            match_found = true;
                            break;
                        }
                    }
                }
            }
        }

        if (match_found)
        {
            break;
        }

        if ((argument[0] == '-') && !(match_found))
        {
            bool have_value = false;
            for (int i = 1; i < argument.size(); ++i)
            {
                bool valid_flag = false;
                std::string key(1, argument[i]);
                bool value_flag = (with_val.count(key) > 0);
                if (value_flag)
                {
                    auto crit_pos = argument.find(key);
                    if (argument.size() > (crit_pos + 1))
                    {
                        if (m_options[m_opt_map[key]].mode() == store_value)
                        {
                            m_options[m_opt_map[key]].value.clear();
                            m_options[m_opt_map[key]].value.push_back(argument.substr(i + 1));
                            m_options[m_opt_map[key]].found() = true;
                            break;
                        }
                        else
                        {
                            m_options[m_opt_map[key]].value.push_back(argument.substr(i + 1));
                            m_options[m_opt_map[key]].found() = true;
                            if (++arg < arguments.size())
                            {
                                auto next_arg = arguments[arg];
                                while (next_arg[0] != '-')
                                {
                                    m_options[m_opt_map[key]].value.push_back(next_arg);
                                    if (++arg >= arguments.size())
                                    {
                                        break;
                                    }
                                    next_arg = arguments[arg];
                                }
                                arg--;
                            }
                            
                            break;
                        }
                    }
                    else // case where theres a space until the next args.
                    {
                        if (++arg >= arguments.size())
                        {
                            std::string e("error, flag '-" + key + "' requires at least one argument.");
                            error(e);
                        }
                        auto next_arg = arguments[arg];
                        if (next_arg[0] == '-')
                        {
                            std::string e("error, flag '-" + key + "' requires an argument.");
                            error(e);
                        }
                        if (m_options[m_opt_map[key]].mode() == store_value)
                        {
                            m_options[m_opt_map[key]].value.clear();
                            m_options[m_opt_map[key]].value.push_back(next_arg);
                            m_options[m_opt_map[key]].found() = true;
                        }
                        else
                        {
                            while (next_arg[0] != '-')
                            {

                                m_options[m_opt_map[key]].value.push_back(next_arg);
                                if (++arg >= arguments.size())
                                {
                                    break;
                                }
                                next_arg = arguments[arg];
                            }
                            m_options[m_opt_map[key]].found() = true;
                            arg--;
                        }

                    }
                    // if (m_options[m_opt_map[key]].)
                    // {
                    //     /* code */
                    // }

                    // std::cout << "found " << m_opt_map[key] << std::endl;
                    // std::cout << "value " << argument.substr(i + 1) << std::endl;
                    break;
                }

                bool bool_flag = (without_val.count(key) > 0);
                if (bool_flag)
                {
                    m_options[m_opt_map[key]].found() = true;
                }
                else
                {
                    error("Invalid flag '-" + key + "'");
                }
            }
        }
        else
        {
            error("Unrecognized flag/option '" + argument + "'");
        }

    }
}

void parser::error(const std::string &e)
{
    std::cerr << "In excecutable \'" << m_prog_name << "\':\n" << e << std::endl;
    exit(1);
}

void parser::help()
{
    auto split = m_prog_name.find_last_of('/');
    std::string stripped_name = m_prog_name.substr(split + 1);
    std::cout << "usage: " << stripped_name << " [options]\n" << std::endl;
    for (auto &option : m_options)
    {
        auto opt = option.second;
        std::cout << "    " <<std::setw(2)<< opt.long_flag() << ", " << opt.short_flag() << std::left << std::setw(24) << opt.help() << std::endl;
    }
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

template <>
std::vector<std::string> parser::get_value<std::vector<std::string>>(std::string key)
{
    return m_options[key].value;
}










inline std::string remove_character(std::string str, const char c)
{
    std::string::iterator end_pos = std::remove(str.begin(), str.end(), c);
    str.erase(end_pos, str.end());
    return str;
}



}

#endif