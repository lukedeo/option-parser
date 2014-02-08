#ifndef PARSER__HH
#define PARSER__HH 

#include <map>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

namespace optionparser
{

enum storage_mode { store_true, store_value, store_mult_values };

struct option
{
    bool found = false;
    bool required = false;
    std::vector<std::string> value;
    std::string short_flag = "", long_flag = "";
    storage_mode mode;
};

inline std::string remove_character(std::string str, char c = ' ');

typedef std::map<std::string, std::map<std::string, std::vector<std::string>>> Archive;

typedef std::map<std::string, bool> SwitchBox;

typedef std::map<std::string, std::map<std::string, std::string>> Dictionary;

typedef int MyCustomType;






class parser
{
public:
    parser() = default;
    ~parser() = default;

    void eat_arguments(int argc, char const *argv[]);

    void add_option(std::string longoption, std::string shortoption, storage_mode mode, bool required = false, std::string dest = "");
    // void add_option(std::string shortoption, storage_mode mode, std::string key = "", bool required = false);

    // template <class T>
    // T get_value(std::string key);

// private:

    // void chomp_short_argument(std::string argument);

    Archive m_options;
    Dictionary m_long_flags, m_short_flags;
    SwitchBox m_box;
};

void parser::add_option(std::string longoption, std::string shortoption, 
    storage_mode mode, bool required, std::string dest)
{
    if (dest == "")
    {
        dest = remove_character(longoption, '-');
    }
    std::string req_string = "optional";
    if (required)
    {
        req_string = "required";
    }
    m_long_flags[req_string][dest] = longoption;
    m_short_flags[req_string][dest] = shortoption;
}

void parser::eat_arguments(int argc, char const *argv[])
{
    std::vector<std::string> arguments;
    for (int i = 1; i < argc; ++i)
    {
        arguments.push_back(argv[i]);
    }

    for (int arg = 0; arg < arguments.size(); ++arg)
    {
        auto argument = arguments[arg];
        bool match_found = false;

        for (auto &flag : m_long_flags["optional"])
        {
            if (argument.find(flag.second) != std::string::npos)
            {
                std::cout << "!!Found flag " << flag.second << " with key " << flag.first << std::endl;

            }
        }
    }

    // for (auto &entry : m_long_flags["optional"])
    // {
    //     for (auto &arg : arguments)
    //     {
    //         if (arg == entry.second)
    //         {
                
    //         }
    //     }
    // }
}

// void parser::chomp_short_argument(std::string argument)
// {

// }











inline std::string remove_character(std::string str, const char c)
{
    std::string::iterator end_pos = std::remove(str.begin(), str.end(), c);
    str.erase(end_pos, str.end());
    return str;
}


}

#endif