<<<<<<< HEAD
# `cmd-parser`, a lightweight header-only option parser headache-minimization 💥
=======
# `cmd-parser` 💥
>>>>>>> ef211065f83d93aebb7bbbd6a0b8bf356bb5cfb9

A lightweight header-only option parser designed for headache-minimization.

<<<<<<< HEAD
## Why?
=======
## Example Usage
>>>>>>> ef211065f83d93aebb7bbbd6a0b8bf356bb5cfb9

```c++
#include "include/optionparser.h"

<<<<<<< HEAD
## Usage
=======
...
>>>>>>> ef211065f83d93aebb7bbbd6a0b8bf356bb5cfb9


<<<<<<< HEAD
p.add_option("--number", "-n").help("pass a double")
                              .mode(optionparser::store_value);

p.add_option("-file").help("pass a list of files to load.")
                     .mode(optionparser::store_mult_values)
                     .required(true);

p.add_option("-save", "-s").help("pass a file to save.")
                           .mode(optionparser::store_value);

// directly pass in these in the main invocation
p.eat_arguments(argc, argv);
=======
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
>>>>>>> ef211065f83d93aebb7bbbd6a0b8bf356bb5cfb9

  p.eat_arguments(argc, argv);

<<<<<<< HEAD
if (p.get_value("number")) {
	number_passed = p.get_value<double>("number");
}

if (p.get_value("file")) {
=======
  if (p.get_value("number")) {
    auto number_passed = p.get_value<int>("number");
  }

  if (p.get_value("file")) {
>>>>>>> ef211065f83d93aebb7bbbd6a0b8bf356bb5cfb9
    auto filenames = p.get_value<std::vector<std::string>>("file");
  }

  return 0;
}
```

<<<<<<< HEAD
After you `p.add_option("--foo", "-f")`, you can chain a number of additional statements. These include:

* `.default_value(...)`, to set a sensible default.
* `.dest(...)`, to set the metavar (i.e., the key to retrieve the value)
* `.help(...)`, to set a help string for that argument.
* `.mode(...)`, can pass one of `optionparser::store_value`, `optionparser::store_mult_values`, or `optionparser::store_true`.
* `.required(...)`, which can make a specific command line flag required for valid invocation.

# 🚧 HELP!

Some things I'd love to have but don't have the time to do:

* Unit tests (in [`Catch`](https://github.com/philsquared/Catch) preferably, but any unit testing framework is ok!)
* Positional arguments would be great to have. 
* Simplify all the [STUPID LOGIC](https://github.com/lukedeo/cmd-parser/blob/master/include/parser.hh#L252-L483). *black hole warning* 🕳
=======
After you `p.add_option("--foo", "-f")`, you can chain additional statements. These include:

* `.default_value(...)`, to set a sensible default.
* `.dest(...)`, to set the metavar (i.e., the key to retrieve the value)
* `.help(...)`, to set a help string for that argument.
* `.mode(...)`, can pass one of `optionparser::StorageMode::STORE_VALUE`, `optionparser::StorageMode::STORE_MULT_VALUES`, or `optionparser::StorageMode::STORE_TRUE`.
* `.required(...)`, which can make a specific command line flag required for valid invocation.

# 🚧 HELP!

Some things I'd love to have but don't have the time to do:

* Additional tests
* Positional arguments
>>>>>>> ef211065f83d93aebb7bbbd6a0b8bf356bb5cfb9
* Subparsers, though nice, might not be possible given the library structure.

