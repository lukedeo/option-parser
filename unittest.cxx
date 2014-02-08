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

int main(int argc, char const *argv[])
{
    optionparser::parser p;

    p.add_option("--help", "-h", optionparser::store_true);
    p.add_option("--file", "-f", optionparser::store_value);
    p.add_option("--save", "-s", optionparser::store_value);

    p.eat_arguments(argc, argv);


    return 0;
}