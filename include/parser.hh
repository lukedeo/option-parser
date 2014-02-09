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

struct dict_entry
{
    unsigned int pos;
    std::string name;
};

struct option
{
    option(){}
    bool &found() {return m_found;}
    bool m_found = false;


    option &required(bool req) 
    {
        m_required = req;
        return *this;
    }
    bool &required() {return m_required;}

    bool m_required = false;

    // std::vector<std::string> value;

    std::string &short_flag() {return m_short_flag;}
    
    // option &short_flag(const std::string &short_flag) 
    // {
    //     m_short_flag = short_flag;
    //     return *this;
    // }

    std::string &long_flag() {return m_long_flag;}
    
    // std::string &long_flag(const std::string &long_flag)
    // {
    //     m_long_flag = long_flag;
    //     return *this;
    // }
    std::string m_short_flag = "", m_long_flag = "";

    storage_mode &mode() {return m_mode;}
    
    option &mode(const storage_mode &mode)
    {
        m_mode = mode;
        return *this;
    }

    storage_mode m_mode = store_true;


    std::string &help() {return m_help;}
    
    option &help(const std::string &help)
    {
        m_help = help;
        return *this;
    }
    std::string m_help = "";

    std::string &dest() {return m_dest;}
    
    option &dest(const std::string &dest)
    {
        m_dest = dest;
        return *this;
    }
    std::string m_dest = "";
};

inline std::string remove_character(std::string str, char c = ' ');

typedef std::map<std::string, std::vector<std::string>> Archive;
typedef std::map<std::string, dict_entry> Dictionary;

class parser
{
public:
    parser(): m_options(0) {}
    ~parser() = default;

    void eat_arguments(int argc, char const *argv[]);

    void add_option(std::string longoption, std::string shortoption, 
        storage_mode mode, bool required = false, std::string help = "", std::string dest = "");

    option &add_option(std::string longoption, std::string shortoption);


    option &add_option_internal(std::string longoption, std::string shortoption);


    // void add_option(std::string longoption, 
    //     storage_mode mode, bool required = false, std::string help = "", std::string dest = "");
    // void add_option(option &o);

    // void add_option(const option &o);



    // void add_option(std::string shortoption, storage_mode mode, std::string key = "", bool required = false);

    template <class T = bool>
    T get_value(std::string key);

// private:

    // void help();



    // void chomp_short_argument(std::string argument);
    void error(const std::string &e);

    Archive m_values;

    std::vector<option> m_options;

    std::string m_prog_name;
    std::unordered_set<std::string> with_val, without_val;
    Dictionary m_opt_map;

    std::map<std::string, unsigned int> idx;
    // Dictionary m_long_flags, m_short_flags;
    // SwitchBox m_box;
};
//----------------------------------------------------------------------------
option &parser::add_option(std::string longoption, std::string shortoption)
{
    return add_option_internal(longoption, shortoption);
}
//----------------------------------------------------------------------------
option &parser::add_option_internal(std::string longoption, std::string shortoption)
{
    m_options.resize(m_options.size() + 1);

    option &opt = m_options.back();

    if (opt.m_dest == "")
    {
        opt.m_dest = remove_character(longoption, '-');   
    }
    opt.long_flag() = longoption;
    opt.short_flag() = shortoption;
    auto k = remove_character(shortoption, '-');
    m_opt_map[k].pos = m_options.size() - 1;
    m_opt_map[k].name = opt.m_dest;
    return opt;
}
//----------------------------------------------------------------------------


void parser::eat_arguments(int argc, char const *argv[])
{
    unsigned int idx_ctr = 0;
    for (auto &opt : m_options)
    {
        idx[opt.m_dest] = idx_ctr;
        if (opt.short_flag() != "")
        { 
            if (opt.mode() != store_true)
            {
                with_val.insert(remove_character(opt.short_flag(), '-'));
            }
            else
            {
                without_val.insert(remove_character(opt.short_flag(), '-'));
            }
            m_opt_map[remove_character(opt.short_flag(), '-')].name = opt.dest();
        }
        ++idx_ctr;
    }
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
            auto opt = option;

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
                            option.found() = true;
                            match_found = true;
                            break;
                        }
                        
                        if(argument.size() > opt.long_flag().size())
                        {
                            option.found() = false;
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
                                    m_values[opt.dest()].clear();
                                    m_values[opt.dest()].push_back(val);
                                }
                                else
                                {
                                    m_values[opt.dest()].push_back(val);
                                    if (++arg < arguments.size())
                                    {
                                        auto next_arg = arguments[arg];
                                        while (next_arg[0] != '-')
                                        {
                                            m_values[opt.dest()].push_back(next_arg);
                                            if (++arg >= arguments.size())
                                            {
                                                break;
                                            }
                                            next_arg = arguments[arg];
                                        }
                                        arg--;
                                    }
                                }
                                option.found() = true;
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

                        if ((opt.mode() == store_value) && (option.found() == false))
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
                            m_values[opt.dest()].clear();
                            m_values[opt.dest()].push_back(next_arg);
                            option.found() = true;
                            match_found = true;
                            break;
                        }
                        if ((opt.mode() == store_mult_values) && (option.found() == false))
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

                                m_values[opt.dest()].push_back(next_arg);
                                if (++arg >= arguments.size())
                                {
                                    break;
                                }
                                next_arg = arguments[arg];
                            }
                            arg--;
                            option.found() = true;
                            match_found = true;
                            break;
                        }
                    }
                }
            }
            // if (match_found)
            // {
            //     break;
            // }
        }
        if ((argument[0] == '-') && 
            !(match_found) &&
            (argument[1] != '-'))
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
                        if (m_options[m_opt_map[key].pos].mode() == store_value)
                        {
                            m_values[m_opt_map[key].name].clear();
                            m_values[m_opt_map[key].name].push_back(argument.substr(i + 1));
                            m_options[m_opt_map[key].pos].found() = true;
                            break;
                        }
                        else
                        {
                            m_values[m_opt_map[key].name].push_back(argument.substr(i + 1));
                            m_options[m_opt_map[key].pos].found() = true;
                            if (++arg < arguments.size())
                            {
                                auto next_arg = arguments[arg];
                                while (next_arg[0] != '-')
                                {
                                    m_values[m_opt_map[key].name].push_back(next_arg);
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
                        if (m_options[m_opt_map[key].pos].mode() == store_value)
                        {
                            m_values[m_opt_map[key].name].clear();
                            m_values[m_opt_map[key].name].push_back(next_arg);
                            m_options[m_opt_map[key].pos].found() = true;
                        }
                        else
                        {
                            while (next_arg[0] != '-')
                            {

                                m_values[m_opt_map[key].name].push_back(next_arg);
                                if (++arg >= arguments.size())
                                {
                                    break;
                                }
                                next_arg = arguments[arg];
                            }
                            m_options[m_opt_map[key].pos].found() = true;
                            arg--;
                        }

                    }
                    // if (m_options[m_opt_map[key].pos].)
                    // {
                    //     /* code */
                    // }

                    // std::cout << "value " << argument.substr(i + 1) << std::endl;
                    break;
                }

                bool bool_flag = (without_val.count(key) > 0);
                if (bool_flag)
                {
                    m_options[m_opt_map[key].pos].found() = true;
                }
                else
                {
                    error("Invalid flag '-" + key + "'");
                }
            }
            match_found = true;
        }
        if(!match_found)
        {
            error("Unrecognized flag/option '" + argument + "'");
        }

    }
    std::vector<std::string> missing;
    for (auto &opt : m_options)
    {
        
        if ((opt.required()) && (!opt.found()))
        {
            missing.push_back(opt.dest());
        }
    }
    if (missing.size() > 0)
    {
        std::string e = "Missing required flags: ";
        e += missing.at(0);
        for (int i = 1; i < missing.size(); ++i)
        {
            e += ", ";
            e += missing.at(i);
        }
        error(e + ".");
    }
}

void parser::error(const std::string &e)
{
    std::cerr << "In excecutable \'" << m_prog_name << "\':\n" << e << std::endl;
    exit(1);
}

// void parser::help()
// {
//     auto split = m_prog_name.find_last_of('/');
//     std::string stripped_name = m_prog_name.substr(split + 1);
//     std::cout << "usage: " << stripped_name << " [options]\n" << std::endl;
//     for (auto &option : m_options)
//     {
//         auto opt = option;
//         std::cout << "    " <<std::setw(2)<< opt.long_flag() << ", " << opt.short_flag() << std::left << std::setw(24) << opt.help() << std::endl;
//     }
// }

// void parser::chomp_short_argument(std::string argument)
// {

// }

template <class T>
T parser::get_value(std::string key)
{
    return m_options[idx[key]].found();
}


template <>
std::string parser::get_value<std::string>(std::string key)
{
    return m_values[key][0];
}

template <>
double parser::get_value<double>(std::string key)
{
    return std::stod(m_values[key][0]);
}

template <>
float parser::get_value<float>(std::string key)
{
    return std::stof(m_values[key][0]);
}

template <>
int parser::get_value<int>(std::string key)
{
    return std::stoi(m_values[key][0]);
}

template <>
unsigned int parser::get_value<unsigned int>(std::string key)
{
    return std::stoul(m_values[key][0]);
}

template <>
std::vector<std::string> parser::get_value<std::vector<std::string>>(std::string key)
{
    return m_values[key];
}










inline std::string remove_character(std::string str, const char c)
{
    std::string::iterator end_pos = std::remove(str.begin(), str.end(), c);
    str.erase(end_pos, str.end());
    return str;
}



}

#endif