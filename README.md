# `cmd-parser`, a lightweight header-only option parser headache-minimization 💥


## Why?

Because why not? I figured this was a good exercise in edge cases.

## Usage

```c++
optionparser::parser p;

p.add_option("--number", "-n").help("pass a double")
                              .mode(optionparser::store_value);

p.add_option("-file").help("pass a list of files to load.")
                     .mode(optionparser::store_mult_values)
                     .required(true);

p.add_option("-save", "-s").help("pass a file to save.")
                           .mode(optionparser::store_value);

// directly pass in these in the main invocation
p.eat_arguments(argc, argv);

double number_passed;

if (p.get_value("number")) {
	number_passed = p.get_value<double>("number");
}

if (p.get_value("file")) {
    auto filenames = p.get_value<std::vector<std::string>>("file");
}
```

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
* Subparsers, though nice, might not be possible given the library structure.

