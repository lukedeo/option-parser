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

    p.add_option("--help", "-h", optionparser::store_true);
    p.add_option("--file", "-f", optionparser::store_mult_values);
    p.add_option("--save", "-s", optionparser::store_value);

    p.eat_arguments(argc, argv);

    if(p.get_value("help"))
    {
        std::cout << "help passed!" << std::endl;
    }


    // double lrn = p.get_value<double>("save");

    // std::cout << "retrieved value for 2 * save is " << 2*lrn << std::endl;

    if (p.get_value("file"))
    {
        auto names = p.get_value<std::vector<std::string>>("file");

        for (int i = 0; i < names.size(); ++i)
        {
            std::cout << "element " << i << ": " << names[i] << std::endl;
        }

        // std::cout << names << std::endl;

    }
    


    return 0;
}