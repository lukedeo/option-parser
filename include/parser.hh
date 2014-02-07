#ifndef PARSER__HH
#define PARSER__HH 

#include <map>
#include <iostream>
#include <string>
#include <vector>

namespace optionparser
{

inline std::string remove_character(std::string str, char c = ' ');

typedef std::map<std::string, std::map<std::string, std::vector<std::string>>> Archive;

typedef std::map<std::string, std::map<std::string, std::string>> Dictionary;



enum storage_mode { store_true, store_value, store_mult_values };



class parser
{
public:
	parser();
	~parser();

	// void eat_arguments(int argc, char const *argv[]);

	void add_option(std::string longoption, std::string shortoption, storage_mode mode, std::string key = "", bool required = false);
	void add_option(std::string shortoption, storage_mode mode, std::string key = "", bool required = false);

	// template <class T>
	// T get_value(std::string key);

private:
	Archive m_options;
	Dictionary m_long_flags, m_short_flags;
};

void parser::add_option(std::string longoption, std::string shortoption, 
	storage_mode mode, std::string key = "", bool required = false)
{
	if (key == "")
	{
		key = remove_character(longoption, '-');
	}
	if (required)
	{
		std::string req_string = "required";
	}
	else
	{
		std::string req_string = "optional";
	}
	m_long_flags[req_string][key] = shortoption;
	m_short_flags[req_string][key] = longoption;
}











inline std::string remove_character(std::string str, char c)
{
    std::string::iterator end_pos = std::remove(str.begin(), str.end(), c);
    str.erase(end_pos, str.end());
    return str;
}


}

#endif