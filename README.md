# `option-parser` 💥

[![Build Status](https://travis-ci.org/lukedeo/option-parser.svg?branch=master)](https://travis-ci.org/lukedeo/option-parser)

A lightweight header-only option parser designed for headache-minimization in C++. Just copy-and-paste into your project and go!

## Example Usage

```c++
#include "include/optionparser.h"

...


int main(int argc, char const *argv[]) {
  optionparser::OptionParser p;

  p.add_option("--number", "-n")
      .help("A number to do something with")
      .default_value(42)
      .mode(optionparser::StorageMode::STORE_VALUE);

  p.add_option("--file")
      .help("pass a list of files to load.")
      .mode(optionparser::StorageMode::STORE_MULT_VALUES)
      .required(true);

  p.add_option("--save", "-s")
      .help("Pass a file to save.")
      .dest("")
      .mode(optionparser::StorageMode::STORE_VALUE);

  p.eat_arguments(argc, argv);

  if (p.get_value("number")) {
    auto number_passed = p.get_value<int>("number");
  }

  if (p.get_value("file")) {
    auto filenames = p.get_value<std::vector<std::string>>("file");
  }

  return 0;
}
```

After you `p.add_option("--foo", "-f")`, you can chain additional statements. These include:

* `.default_value(...)`, to set a sensible default.
* `.dest(...)`, to set the metavar (i.e., the key to retrieve the value)
* `.help(...)`, to set a help string for that argument.
* `.mode(...)`, can pass one of `optionparser::StorageMode::STORE_VALUE`, `optionparser::StorageMode::STORE_MULT_VALUES`, or `optionparser::StorageMode::STORE_TRUE`.
* `.required(...)`, which can make a specific command line flag required for valid invocation.

# 🚧 HELP!

Some things I'd love to have but don't have the time to do (in order of priority):

* Documentation! The library has a small surface area, but people shouldn't have to dig through a header file to find out how to do things...
* A proper `CMakeLists.txt` file.
* Subparsers, though nice, might not be possible given the library structure.

