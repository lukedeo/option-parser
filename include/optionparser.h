//-----------------------------------------------------------------------------
//  optionparser.h -- A Header-Only commandline argument parser
//  Author: Luke de Oliveira <lukedeo@ldo.io>
//  License: MIT
//-----------------------------------------------------------------------------

#ifndef OPTIONPARSER_H_
#define OPTIONPARSER_H_

#include <iostream>
#include <map>
#include <numeric>
#include <sstream>
#include <utility>
#include <vector>

namespace optionparser {

// The utils::* namespace contains general utilities not necessarily useful
// outside the main scope of the library
namespace utils {

std::vector<std::string> split_str(std::string s,
                                   const std::string &delimiter = " ") {
  size_t pos = 0;
  size_t delimiter_length = delimiter.length();
  std::vector<std::string> vals;
  while ((pos = s.find(delimiter)) != std::string::npos) {
    vals.push_back(s.substr(0, pos));
    s.erase(0, pos + delimiter_length);
  }
  vals.push_back(s);
  return vals;
}

std::string stitch_str(const std::vector<std::string> &text,
                       unsigned max_per_line = 80,
                       const std::string &leading_str = "") {
  std::vector<std::string> result;

  std::string line_value;
  for (const auto &token : text) {
    if (line_value.empty()) {
      line_value = (leading_str + token);
      continue;
    }

    auto hypothetical_line = line_value;
    hypothetical_line.append(" " + token);

    if (hypothetical_line.size() > max_per_line) {
      // In this case, we were better off before
      result.emplace_back(line_value);
      line_value = (leading_str + token);
    } else {
      line_value = hypothetical_line;
    }
  }
  // Collect the last line since we don't track indices in the loop proper.
  result.emplace_back(line_value);
  return std::accumulate(
      result.begin() + 1, result.end(), result.at(0),
      [](std::string &s, const std::string &piece) -> std::string {
        return s + "\n" + piece;
      });
}

} // end namespace utils

// Define a thin error for any sort of parser error that arises
class ParserError : public std::runtime_error {
  using std::runtime_error::runtime_error;
};

// Enums for Option config
enum StorageMode { STORE_TRUE = 0, STORE_VALUE, STORE_MULT_VALUES };
enum OptionType { LONG_OPT = 0, SHORT_OPT, POSITIONAL_OPT, EMPTY_OPT };

// Option class definition
class Option {
public:
  Option() = default;

  std::string help_doc();

  std::string &short_flag() { return m_short_flag; }
  std::string &long_flag() { return m_long_flag; }
  std::string &pos_flag() { return m_pos_flag; }

  bool found() { return m_found; }
  Option &found(bool found) {
    m_found = found;
    return *this;
  }

  StorageMode mode() { return m_mode; }
  Option &mode(const StorageMode &mode) {
    m_mode = mode;
    return *this;
  }

  bool required() { return m_required; }
  Option &required(bool req) {
    m_required = req;
    return *this;
  }

  std::string metavar() {
    std::string formatted_metavar;
    if (!m_metavar.empty()) {
      if (m_mode == STORE_TRUE) {
        return "";
      }
      formatted_metavar = m_metavar;
    } else {
      for (const auto &cand :
           {m_dest, m_pos_flag, m_long_flag, std::string("ARG")}) {
        if (!cand.empty()) {
          formatted_metavar = cand.substr(cand.find_first_not_of('-'));
          std::transform(formatted_metavar.begin(), formatted_metavar.end(),
                         formatted_metavar.begin(), ::toupper);
          break;
        }
      }
    }
    if (m_mode == STORE_MULT_VALUES) {
      formatted_metavar = (formatted_metavar + "1 [" + formatted_metavar +
                           "2, " + formatted_metavar + "3, ...]");
    }
    return formatted_metavar;
  }

  Option &metavar(const std::string &mvar) {
    m_metavar = mvar;
    return *this;
  }

  std::string help() { return m_help; }
  Option &help(const std::string &help) {
    m_help = help;
    return *this;
  }

  std::string dest() { return m_dest; }
  Option &dest(const std::string &dest) {
    m_dest = dest;
    return *this;
  }

  std::string default_value() { return m_default_value; }

  Option &default_value(const std::string &default_value) {
    m_default_value = default_value;
    return *this;
  }

  Option &default_value(const char *default_value) {
    m_default_value = std::string(default_value);
    return *this;
  }

  template <typename T> Option &default_value(const T &default_value) {
    m_default_value = std::to_string(default_value);
    return *this;
  }

  static OptionType get_type(std::string opt);
  static std::string get_destination(const std::string &first_option,
                                     const std::string &second_option);
  static void validate_option_types(const OptionType &first_option_type,
                                    const OptionType &second_option_type);

private:
  bool m_found = false;
  bool m_required = false;
  StorageMode m_mode = STORE_TRUE;
  std::string m_help = "";
  std::string m_dest = "";
  std::string m_default_value = "";
  std::string m_metavar = "";

  std::string m_short_flag = "";
  std::string m_long_flag = "";
  std::string m_pos_flag = "";
};

// Non-inline definitions for Option methods
std::string Option::help_doc() {
  std::string h = "    ";
  if (!m_long_flag.empty()) {
    h += m_long_flag;
    if (!m_short_flag.empty()) {
      h += ", ";
    }
  }
  if (!m_short_flag.empty()) {
    h += m_short_flag;
  }
  if (!m_pos_flag.empty()) {
    h += m_pos_flag;
  }

  auto arg_buf = std::max(h.length() + 1, static_cast<unsigned long>(25));
  auto help_str = utils::stitch_str(utils::split_str(m_help), arg_buf + 50,
                                    std::string(arg_buf, ' '));
  char char_buf[h.length() + help_str.length() + 100];
  sprintf(char_buf, ("%-" + std::to_string(arg_buf) + "s%s\n").c_str(),
          h.c_str(), help_str.substr(arg_buf).c_str());
  return std::string(char_buf);
}

OptionType Option::get_type(std::string opt) {
  if (opt.empty()) {
    return OptionType::EMPTY_OPT;
  }
  if (opt.size() == 2) {
    if (opt[0] == '-') {
      return OptionType::SHORT_OPT;
    }
  }

  if (opt.size() > 2) {
    if (opt[0] == '-' && opt[1] == '-') {
      return OptionType::LONG_OPT;
    }
  }

  return OptionType::POSITIONAL_OPT;
}

void Option::validate_option_types(const OptionType &first_option_type,
                                   const OptionType &second_option_type) {

  auto err = [](const std::string &msg) {
    throw std::runtime_error("Parser inconsistency: " + msg);
  };
  if (first_option_type == OptionType::EMPTY_OPT) {
    err("Cannot have first option be empty.");
  }
  if (first_option_type == OptionType::POSITIONAL_OPT &&
      second_option_type != OptionType::EMPTY_OPT) {
    err("Positional arguments can only have one option, found non-empty second "
        "option.");
  }
  if (second_option_type == OptionType::POSITIONAL_OPT) {
    err("Cannot have second option be a positional option.");
  }
}

std::string Option::get_destination(const std::string &first_option,
                                    const std::string &second_option) {
  std::string dest;

  auto first_opt_type = Option::get_type(first_option);
  auto second_opt_type = Option::get_type(second_option);

  validate_option_types(first_opt_type, second_opt_type);

  if (first_opt_type == OptionType::LONG_OPT) {
    dest = first_option.substr(2);
  } else if (second_opt_type == OptionType::LONG_OPT) {
    dest = second_option.substr(2);
  } else {
    if (first_opt_type == OptionType::SHORT_OPT) {
      dest = first_option.substr(1) + "_option";
    } else if (second_opt_type == OptionType::SHORT_OPT) {
      dest = second_option.substr(1) + "_option";
    } else {
      if (first_opt_type == OptionType::POSITIONAL_OPT &&
          second_opt_type == OptionType::EMPTY_OPT) {
        dest = first_option;
      } else {
        std::string msg = "Parser inconsistency error.";
        throw std::runtime_error(msg);
      }
    }
  }

  return dest;
}

// OptionParser class definition
class OptionParser {
public:
  explicit OptionParser(std::string description = "", bool create_help = true)
      : m_options(0), m_description(std::move(description)),
        m_pos_args_count(1), m_exit_on_failure(true) {
    if (create_help) {
      add_option("--help", "-h").help("Display this help message and exit.");
    }
  }

  ~OptionParser() = default;

  void eat_arguments(unsigned int argc, char const *argv[]);

  Option &add_option(const std::string &first_option,
                     const std::string &second_option = "");

  // We template-specialize these later
  template <class T = bool> T get_value(const std::string &key);

  void help();

  OptionParser &exit_on_failure(bool exit = true);

  OptionParser &throw_on_failure(bool throw_ = true);

private:
  Option &add_option_internal(const std::string &first_option,
                              const std::string &second_option);

  void try_to_exit_with_message(const std::string &e);

  ParserError fail_for_missing_key(const std::string &key);

  ParserError fail_unrecognized_argument(const std::string &arg);

  ParserError
  fail_for_missing_arguments(const std::vector<std::string> &missing_flags);

  bool get_value_arg(std::vector<std::string> &arguments, unsigned int &arg,
                     Option &opt, std::string &flag);

  bool try_to_get_opt(std::vector<std::string> &arguments, unsigned int &arg,
                      Option &option, std::string &flag);

  void check_for_missing_args();

  std::map<std::string, std::vector<std::string>> m_values;
  int m_pos_args_count;
  std::vector<Option> m_options;
  std::string m_prog_name, m_description;
  std::vector<std::string> m_positional_options_names;
  std::map<std::string, unsigned int> m_option_idx;
  bool m_exit_on_failure;
};

// Define methods non-inline
Option &OptionParser::add_option(const std::string &first_option,
                                 const std::string &second_option) {
  return add_option_internal(first_option, second_option);
}

Option &OptionParser::add_option_internal(const std::string &first_option,
                                          const std::string &second_option) {
  m_options.resize(m_options.size() + 1);
  Option &opt = m_options.back();
  OptionType first_option_type = Option::get_type(first_option);
  OptionType second_option_type = Option::get_type(second_option);

  try {
    opt.dest(Option::get_destination(first_option, second_option));
  } catch (const std::runtime_error &err) {
    try_to_exit_with_message(err.what());
    throw err;
  }

  if (first_option_type == OptionType::LONG_OPT) {
    opt.long_flag() = first_option;
  } else if (second_option_type == OptionType::LONG_OPT) {
    opt.long_flag() = second_option;
  }

  if (first_option_type == OptionType::SHORT_OPT) {
    opt.short_flag() = first_option;
  } else if (second_option_type == OptionType::SHORT_OPT) {
    opt.short_flag() = second_option;
  }
  if (first_option_type == OptionType::POSITIONAL_OPT) {
    opt.pos_flag() = first_option;
    m_pos_args_count += 1;
    m_positional_options_names.push_back(first_option);
  }
  return opt;
}

bool OptionParser::get_value_arg(std::vector<std::string> &arguments,
                                 unsigned int &arg, Option &opt,
                                 std::string &flag) {
  std::string val;
  m_values[opt.dest()].clear();

  if (arguments[arg].size() > flag.size()) {
    auto search_pt = arguments[arg].find_first_of('=');

    if (search_pt == std::string::npos) {
      search_pt = arguments[arg].find_first_of(' ');

      if (search_pt == std::string::npos) {
        try_to_exit_with_message("Error, long options (" + flag +
                                 ") require a '=' or space before a value.");
        return false;
      }
      auto vals = utils::split_str(arguments[arg].substr(search_pt + 1));
      for (const auto &v : vals) {
        m_values[opt.dest()].push_back(v);
      }
    }
  } else {
    if (arg + 1 >= arguments.size()) {
      if (opt.default_value().empty()) {
        try_to_exit_with_message("error, flag '" + flag +
                                 "' requires an argument.");
        return false;
      }
      if (m_values[opt.dest()].empty()) {
        val = opt.default_value();
      }
    } else {
      if (arguments[arg + 1][0] == '-') {
        if (opt.default_value().empty()) {
          try_to_exit_with_message("error, flag '" + flag +
                                   "' requires an argument.");
          return false;
        }
        if (m_values[opt.dest()].empty()) {
          val = opt.default_value();
        }
      }
    }
  }

  if (!val.empty()) {
    m_values[opt.dest()].push_back(val);
    return true;
  }
  int arg_distance = 0;
  while (arguments[arg + 1][0] != '-') {
    arg++;
    if (arg_distance && (opt.mode() != StorageMode::STORE_MULT_VALUES)) {
      break;
    }
    arg_distance++;
    m_values[opt.dest()].push_back(arguments[arg]);
    if (arg + 1 >= arguments.size()) {
      break;
    }
  }

  return true;
}

bool OptionParser::try_to_get_opt(std::vector<std::string> &arguments,
                                  unsigned int &arg, Option &option,
                                  std::string &flag) {
  if (flag.empty()) {
    return false;
  }

  if (arguments[arg] != flag) {
    return false;
  }

  if (!option.pos_flag().empty()) {
    m_values[option.dest()].push_back(option.pos_flag());
    option.found(true);
    return true;
  }

  if (option.mode() == STORE_TRUE) {
    option.found(true);
    return true;
  }

  if (((option.mode() == STORE_VALUE) ||
       (option.mode() == STORE_MULT_VALUES)) &&
      !option.found()) {
    if (get_value_arg(arguments, arg, option, flag)) {
      option.found(true);
      return true;
    }
  }

  return false;
}

void OptionParser::check_for_missing_args() {
  std::vector<std::string> missing;
  for (auto &opt : m_options) {
    if ((opt.required()) && (!opt.found())) {
      missing.push_back(opt.dest());
    } else if ((!opt.default_value().empty()) && (!opt.found())) {
      m_values[opt.dest()].push_back(opt.default_value());
      opt.found(true);
    }
  }
  if (!missing.empty()) {
    throw fail_for_missing_arguments(missing);
  }
}

void OptionParser::eat_arguments(unsigned int argc, char const *argv[]) {
  unsigned int idx_ctr = 0;
  for (auto &opt : m_options) {
    m_option_idx[opt.dest()] = idx_ctr;
    idx_ctr++;
  }

  const std::string args_end = "- ";
  m_prog_name = argv[0];
  std::vector<std::string> arguments(argv + 1, argv + argc);

  // dummy way to solve problem with last arg of
  arguments.emplace_back(args_end);

  // for each argument cluster
  int pos_args = 1;
  for (unsigned int arg = 0; arg < arguments.size(); ++arg) {
    bool match_found = false;
    // for each option sets
    for (auto &option : m_options) {
      match_found = try_to_get_opt(arguments, arg, option, option.long_flag());
      if (match_found) {
        break;
      }

      match_found = try_to_get_opt(arguments, arg, option, option.short_flag());
      if (match_found) {
        break;
      }
    }

    if (!match_found) {
      if (arguments[arg] != args_end) {
        if (m_pos_args_count > pos_args) {
          m_options[m_option_idx.at(m_positional_options_names[pos_args - 1])]
              .found(true);
          m_values[m_positional_options_names[pos_args - 1]].push_back(
              arguments[arg]);
          pos_args++;
        } else
          throw fail_unrecognized_argument(arguments[arg]);
      }
    }
  }

  if (get_value("help")) {
    help();
  }
  check_for_missing_args();
}

void OptionParser::try_to_exit_with_message(const std::string &e) {
  if (m_exit_on_failure) {
    std::cerr << "In excecutable \'";
    std::cerr << m_prog_name << "\':\n" << e << std::endl;
    exit(1);
  }
}

ParserError OptionParser::fail_for_missing_key(const std::string &key) {
  auto msg = "Tried to access value for field '" + key +
             "' which is not a valid field.";
  try_to_exit_with_message(msg);
  return ParserError(msg);
}

ParserError OptionParser::fail_unrecognized_argument(const std::string &arg) {
  auto msg = "Unrecognized flag/option '" + arg + "'";
  try_to_exit_with_message(msg);
  return ParserError(msg);
}

ParserError OptionParser::fail_for_missing_arguments(
    const std::vector<std::string> &missing_flags) {
  auto msg =
      "Missing required flags: " +
      std::accumulate(
          missing_flags.begin() + 1, missing_flags.end(), missing_flags.at(0),
          [](std::string &s, const std::string &piece) -> std::string {
            return s + ", " + piece;
          }) +
      ".";
  try_to_exit_with_message(msg);
  return ParserError(msg);
}

void OptionParser::help() {
  auto split = m_prog_name.find_last_of('/');
  std::stringstream leading;

  leading << "usage: " << m_prog_name.substr(split + 1) << " ";
  std::string usage_str = leading.str();
  std::cout << usage_str;

  std::vector<std::string> option_usage;
  for (auto &option : m_options) {
    std::stringstream optss;
    optss << (option.required() ? "" : "[");
    if (!option.short_flag().empty()) {
      optss << option.short_flag();
    } else if (!option.long_flag().empty()) {
      optss << option.long_flag();
    }
    if (option.mode() != StorageMode::STORE_TRUE) {
      optss << (option.pos_flag().empty() ? " " : "") << option.metavar();
    }
    optss << (option.required() ? " " : "] ");
    option_usage.emplace_back(optss.str());
  }
  std::cout << utils::stitch_str(option_usage, 80,
                                 std::string(usage_str.size(), ' '))
                   .substr(usage_str.size())
            << std::endl;

  if (!m_description.empty()) {
    std::cout << "\n" << m_description << "\n" << std::endl;
  }

  std::vector<Option> pos_opts;
  std::vector<Option> reg_opts;
  std::copy_if(m_options.begin(), m_options.end(), std::back_inserter(pos_opts),
               [](Option &o) { return !o.pos_flag().empty(); });
  std::copy_if(m_options.begin(), m_options.end(), std::back_inserter(reg_opts),
               [](Option &o) { return o.pos_flag().empty(); });

  if (!pos_opts.empty()) {
    std::cout << "\nPositional Arguments:" << std::endl;
    std::for_each(pos_opts.begin(), pos_opts.end(),
                  [](Option &o) { std::cout << o.help_doc(); });
  }

  std::cout << "\nOptions:" << std::endl;
  std::for_each(reg_opts.begin(), reg_opts.end(),
                [](Option &o) { std::cout << o.help_doc(); });
  exit(0);
}

OptionParser &OptionParser::exit_on_failure(bool exit) {
  m_exit_on_failure = exit;
  return *this;
}

OptionParser &OptionParser::throw_on_failure(bool throw_) {
  m_exit_on_failure = !throw_;
  return *this;
}

// Provide all template specializations for get_value<T>(keyName)
template <class T> T OptionParser::get_value(const std::string &key) {
  try {
    return m_options[m_option_idx.at(key)].found();
  } catch (std::out_of_range &err) {
    throw fail_for_missing_key(key);
  }
}

template <>
std::string OptionParser::get_value<std::string>(const std::string &key) {
  try {
    return m_values.at(key).at(0);
  } catch (std::out_of_range &err) {
    throw fail_for_missing_key(key);
  }
}

template <> double OptionParser::get_value<double>(const std::string &key) {
  try {
    return std::stod(m_values.at(key).at(0));
  } catch (std::out_of_range &err) {
    throw fail_for_missing_key(key);
  }
}

template <> float OptionParser::get_value<float>(const std::string &key) {
  try {
    return std::stof(m_values.at(key).at(0));
  } catch (std::out_of_range &err) {
    throw fail_for_missing_key(key);
  }
}

template <> int OptionParser::get_value<int>(const std::string &key) {
  try {
    return std::stoi(m_values.at(key).at(0));
  } catch (std::out_of_range &err) {
    throw fail_for_missing_key(key);
  }
}

template <>
unsigned int OptionParser::get_value<unsigned int>(const std::string &key) {
  try {
    return std::stoul(m_values.at(key).at(0));
  } catch (std::out_of_range &err) {
    throw fail_for_missing_key(key);
  }
}

template <>
std::vector<std::string>
OptionParser::get_value<std::vector<std::string>>(const std::string &key) {
  try {
    return m_values.at(key);
  } catch (std::out_of_range &err) {
    throw fail_for_missing_key(key);
  }
}

template <>
std::vector<int>
OptionParser::get_value<std::vector<int>>(const std::string &key) {
  try {
    std::vector<int> v;
    for (auto &entry : m_values.at(key)) {
      v.push_back(std::stoi(entry));
    }
    return std::move(v);
  } catch (std::out_of_range &err) {
    throw fail_for_missing_key(key);
  }
}

} // end namespace optionparser

#endif
