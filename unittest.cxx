#include "include/parser.hh"
#include <iostream>

// class unittest
// {
// public:
// 	unittest();

// 	~unittest();
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
	p.add_option("--file", "-f", optionparser::store_true, 1);
	p.add_option("--save", "-s", optionparser::store_true);

	std::cout << p.m_long_flags << std::endl;
	std::cout << p.m_short_flags << std::endl;

	p.eat_arguments(argc, argv);


	return 0;
}