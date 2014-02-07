#ifndef PARSER__HH
#define PARSER__HH 

#include <map>
#include <iostream>
#include <string>
#include <vector>

namespace optionparser
{

typedef std::map<std::string, std::map<std::string, std::vector<std::string>>> Dictionary;

enum storage_mode { store_true, store_value, store_mult_values };



class parser
{
public:
	parser();
	~parser();

	void eat_arguments(int argc, char const *argv[]);

	void add_option(std::string longoption, std::string shortoption, storage_mode mode, std::string key = "", bool required = false);
	void add_option(std::string shortoption, storage_mode mode, std::string key = "", bool required = false);

	template <class T>
	T get_value(std::string key);

private:
	Dictionary options;

	
};





}

#endif