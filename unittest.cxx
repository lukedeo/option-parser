#include "include/parser.hh"
#include <iostream>

// class unittest
// {
// public:
//  unittest();

//  ~unittest();
// };

#include <fstream>


template <class T, class U>
std::ostream& operator << ( std::ostream& o, const std::map<T, U> &m)
{
    for (auto &e : m)
    {
        o << e.first << ":\n" << e.second << std::endl;
    }
    return o;
}

template <class T>
std::ostream& operator << (std::ostream& o, const std::vector<T> &v)
{
    for (auto &entry : v)
    {
        o << entry << "   ";
    }
    return o;
}

int main(int argc, char const *argv[])
{
    optionparser::parser p;

    // p.add_option("", "-h", optionparser::store_true, false, "Display this message then exit.", "help");
    // p.add_option("--file", "-f", optionparser::store_mult_values, false, "pass a list of files to load.");
    // p.add_option("--save", "-s", optionparser::store_value, false, "do the save thing");
    // p.add_option("--print", "-p", optionparser::store_value, false, "do the print thing");
    // p.add_option("--nom", "-n", optionparser::store_true, false, "do the nom thing");

    p.add_option("--help", "-h") .help("Display this message then exit.");

    p.add_option("--file", "-f") .help("pass a list of files to load.")
                                 .mode(optionparser::store_mult_values)
                                 .required(false);

    p.eat_arguments(argc, argv);
    // p.help();

    for (auto &entry : p.m_options)
    {
        std::cout << "name: " << entry.dest() << (entry.found() ? " found!": " not found!") << std::endl;
    }

    if(p.get_value("help"))
    {
        std::cout << "help passed!" << std::endl;
    }


    // double lrn = p.get_value<double>("save");

    // std::cout << "retrieved value for 2 * save is " << 2*lrn << std::endl;

    if (p.get_value("file"))
    {
        std::cout << "hi!" << std::endl;
        auto names = p.get_value<std::vector<std::string>>("file");

        std::cout << "Filenames" << std::endl;

        for (int i = 0; i < names.size(); ++i)
        {
            std::cout << "element " << i << ": " << names[i] << std::endl;
        }

        // std::cout << names << std::endl;
    }

    // if (p.get_value("save"))
    // {
    //     auto names = p.get_value<std::vector<std::string>>("save");

    //     std::cout << "Savenames" << std::endl;

    //     for (int i = 0; i < names.size(); ++i)
    //     {
    //         std::cout << "element " << i << ": " << names[i] << std::endl;
    //     }
    // }
    


    return 0;
}