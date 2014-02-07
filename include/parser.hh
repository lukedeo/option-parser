#ifndef PARSER__HH
#define PARSER__HH 

#include <map>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

namespace optionparser
{

inline std::string remove_character(std::string str, char c = ' ');

typedef std::map<std::string, std::map<std::string, std::vector<std::string>>> Archive;

typedef std::map<std::string, std::map<std::string, std::string>> Dictionary;



enum storage_mode { store_true, store_value, store_mult_values };



class parser
{
public:
	parser() = default;
	~parser()= default;

	// void eat_arguments(int argc, char const *argv[]);

	void add_option(std::string longoption, std::string shortoption, storage_mode mode, bool required = false, std::string key = "");
	// void add_option(std::string shortoption, storage_mode mode, std::string key = "", bool required = false);

	// template <class T>
	// T get_value(std::string key);

// private:
	Archive m_options;
	Dictionary m_long_flags, m_short_flags;
};

void parser::add_option(std::string longoption, std::string shortoption, 
	storage_mode mode, bool required, std::string key)
{
	if (key == "")
	{
		key = remove_character(longoption, '-');
	}
	std::string req_string = "optional";
	if (required)
	{
		req_string = "required";
	}
	m_long_flags[req_string][key] = longoption;
	m_short_flags[req_string][key] = shortoption;
}











inline std::string remove_character(std::string str, const char c)
{
    std::string::iterator end_pos = std::remove(str.begin(), str.end(), c);
    str.erase(end_pos, str.end());
    return str;
}


}

#endif