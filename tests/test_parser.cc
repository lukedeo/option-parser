#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include <iterator>

#include "doctest.h"

// Makes sure we don't exit on failures.
#include "optionparser.h"

template <class T, size_t N> constexpr size_t length(T (&)[N]) { return N; }

optionparser::OptionParser parser() {
  optionparser::OptionParser p(
      "A test to make sure that this option parser works");
  p.throw_on_failure();
  return p;
}

TEST_CASE("test substring names") {
  SUBCASE("test boolean arg with long arg") {
    const char *argv[] = {"tests", "--boolean"};
    int argc = length(argv);
    auto p = parser();
    p.add_option("--boolean").help(" boolean value");

    p.eat_arguments(argc, argv);
    CHECK(p.get_value("boolean"));
  }

  SUBCASE("test boolean arg with short arg") {
    const char *argv[] = {"tests", "-b"};
    int argc = length(argv);
    auto p = parser();
    p.add_option("-b").help(" boolean value");
    p.eat_arguments(argc, argv);
    CHECK(p.get_value("b_option"));
  }

  SUBCASE("test boolean arg with both short  and long arg pass long") {
    const char *argv[] = {"tests", "--boolean"};
    int argc = length(argv);
    auto p = parser();
    p.add_option("--boolean", "-b").help(" boolean value");
    p.eat_arguments(argc, argv);
    CHECK(p.get_value("boolean"));
  }

  SUBCASE("test boolean arg with both short  and long arg pass short") {
    const char *argv[] = {"tests", "-b"};
    int argc = length(argv);
    auto p = parser();
    p.add_option("--boolean", "-b").help(" boolean value");
    p.eat_arguments(argc, argv);
    CHECK(p.get_value("boolean"));
  }

  SUBCASE("test boolean positional arg ") {
    const char *argv[] = {"tests", "passed_bool_value"};
    int argc = length(argv);
    auto p = parser();
    p.add_option("pass").help(" positional boolean value");
    p.eat_arguments(argc, argv);
    CHECK(p.get_value("pass"));
  }

  SUBCASE("test boolean positional arg which is not passed") {
    const char *argv[] = {"tests"};
    int argc = length(argv);
    auto p = parser();
    p.add_option("pass").help(" positional boolean value");
    p.eat_arguments(argc, argv);
    CHECK(!p.get_value("pass"));
  }

  SUBCASE("test boolean positional arg which is not passed but is required") {
    const char *argv[] = {"tests"};
    int argc = length(argv);
    auto p = parser();
    p.add_option("pass").help(" positional boolean value").required(true);
    CHECK_THROWS(p.eat_arguments(argc, argv));
  }

  SUBCASE("test many boolean  arg which ") {
    const char *argv[] = {
        "tests",
        "--second",
        "-f",
    };
    int argc = length(argv);
    auto p = parser();
    p.add_option("--first", "-f").help(" first boolean value");
    p.add_option("-s", "--second").help(" second boolean value");
    p.add_option("--third").help(" third boolean value");
    p.add_option("-l").help(" last boolean value");

    p.eat_arguments(argc, argv);
    CHECK(p.get_value("first"));
    CHECK(p.get_value("second"));
    CHECK(!p.get_value("third"));
    CHECK(!p.get_value("l_option"));
  }

  SUBCASE("test many boolean arg  with positional args ") {
    const char *argv[] = {"tests", "--second", "-f",         "first_pos",
                          "72",    "--third",  "test_string"};
    int argc = length(argv);
    auto p = parser();
    p.add_option("--first", "-f").help(" first boolean value");
    p.add_option("-s", "--second").help(" second boolean value");
    p.add_option("--third").help(" third boolean value");
    p.add_option("-l").help(" last boolean value");
    p.add_option("first_pos").help(" first boolean positional value");
    p.add_option("second_pos").help(" second boolean positionalboolean value");
    p.add_option("last_pos").help(" last boolean value");

    p.eat_arguments(argc, argv);
    CHECK(p.get_value("first"));
    CHECK(p.get_value("second"));
    CHECK(p.get_value("third"));
    CHECK(!p.get_value("l_option"));

    CHECK(p.get_value("first_pos"));
    CHECK(p.get_value<int>("second_pos") == 72);
    CHECK(p.get_value<std::string>("last_pos") == "test_string");
  }
}

TEST_CASE("test substring names") {
  const char *argv[] = {
      "tests",
      "flag1",
      "--flag",
  };
  int argc = length(argv);

  auto p = parser();

  p.add_option("--flag", "-f").help("just flag");
  p.add_option("flag1").help("just flag with substring for name");
  p.eat_arguments(argc, argv);
  bool check_is_flag_set = false;
  if (p.get_value("flag1")) {
    check_is_flag_set = true;
  }

  CHECK(check_is_flag_set);

  check_is_flag_set = false;
  if (p.get_value("flag")) {
    check_is_flag_set = true;
  }

  CHECK(check_is_flag_set);
}

TEST_CASE("test parser functionality") {
  const char *argv[] = {"tests", "--flag", "--only_large", "-s", "-q",
                        "first_short2", "--in-fil-e", "-dash", "--store_str",
                        "qqqdash", "--store_num", "72", "--store_multy_s",
                        "str1", "str2", "str3", "str4", "str5", "-b", "--pp",
                        "-singledash",
                        // "hellooo",
                        "--boolean"};

  int argc = length(argv);

  auto p = parser();

  p.add_option("--flag", "-f").help("just flag");
  p.add_option("--boolean", "-b")
      .help("boolean")
      .mode(optionparser::StorageMode::STORE_TRUE);

  p.add_option("--only_large").help("set only large opt");
  p.add_option("-singledash")
      .help("singledash")
      .mode(optionparser::StorageMode::STORE_VALUE);
  p.add_option("-s").help("set only small opt");
  p.add_option("-q", "--first_short1")
      .help("first_short1")
      .mode(optionparser::StorageMode::STORE_VALUE);
  p.add_option("-w", "--first_short2").help("first_short2");
  p.add_option("--in-fil-e", "-i").help("dashes in name");
  p.add_option("-dash").help("only one dash for large opt");
  p.add_option("--store_str", "-v")
      .help("store single str val")
      .mode(optionparser::StorageMode::STORE_VALUE)
      .default_value("stor str def1");
  p.add_option("--store_num", "-n")
      .help("store single num val")
      .mode(optionparser::StorageMode::STORE_VALUE);
  p.add_option("--store_multy_s", "-m")
      .help("store multy str val")
      .mode(optionparser::StorageMode::STORE_MULT_VALUES)
      .default_value("def2");
  p.add_option("--pp", "-p")
      .help("store single str val")
      .mode(optionparser::StorageMode::STORE_VALUE)
      .default_value("p1");

  p.eat_arguments(argc, argv);

  bool check_is_flag_set = false;
  if (p.get_value("flag")) {
    check_is_flag_set = true;
  }
  CHECK(check_is_flag_set);
  if (p.get_value("only_large")) {
    check_is_flag_set = true;
  }
  CHECK(check_is_flag_set);
  if (p.get_value("s_option")) {
    check_is_flag_set = true;
  }
  CHECK(check_is_flag_set);
  if (p.get_value("first_short1")) {
    check_is_flag_set = true;
  }
  CHECK(check_is_flag_set);
  if (p.get_value("first_short2")) {
    check_is_flag_set = true;
  }
  CHECK(check_is_flag_set);
  if (p.get_value("in-fil-e")) {
    check_is_flag_set = true;
  }
  CHECK(check_is_flag_set);
  // if (p.get_value("option_-dash")) {
  //   check_is_flag_set = true;
  // }
  // CHECK(check_is_flag_set == true);

  if (p.get_value("store_str")) {
    auto names = p.get_value<std::vector<std::string>>("store_str");
    CHECK(names[0] == argv[9]);
  }

  if (p.get_value("store_num")) {
    CHECK(p.get_value<int>("store_num") == 72);
  }

  if (p.get_value("store_multy_s")) {
    auto names = p.get_value<std::vector<std::string>>("store_multy_s");
    CHECK(names[0] == argv[13]);
    CHECK(names[1] == argv[14]);
    CHECK(names[2] == argv[15]);
    CHECK(names[3] == argv[16]);
    CHECK(names[4] == argv[17]);
  }

  if (p.get_value("boolean")) {
    CHECK(p.get_value<bool>("boolean"));
  }

  SUBCASE("fail on invalid arg") {
    CHECK_THROWS(p.get_value("this-isnt-in-parser"));
  }
}

TEST_CASE("test OO functionality") {
  const char *argv[] = {"tests",   "--flag",   "asdsaflag",
                        "bsadsad", "bqwewqeq", "--boolean"};

  auto argc = length(argv);

  auto p = parser();

  p.add_option("--flag", "-f")
      .help("just=flag")
      .mode(optionparser::StorageMode::STORE_MULT_VALUES);

  p.add_option("--boolean", "-b")
      .help("boolean")
      .mode(optionparser::StorageMode::STORE_TRUE);
  p.eat_arguments(argc, argv);

  bool check_is_flag_set = false;
  if (p.get_value("flag")) {
    check_is_flag_set = true;
    auto names = p.get_value<std::vector<std::string>>("flag");
    CHECK(names[0] == "asdsaflag");
    CHECK(names[1] == "bsadsad");
    CHECK(names[2] == "bqwewqeq");
  }
  CHECK(check_is_flag_set);
}

TEST_CASE("test default argument not passed") {
  const char *argv[] = {"tests", "--flag"};

  auto argc = length(argv);

  optionparser::OptionParser p("fsafasf");

  p.add_option("--flag", "-f")
      .help("just=flag")
      .mode(optionparser::StorageMode::STORE_MULT_VALUES)
      .default_value("foo");
  p.eat_arguments(argc, argv);

  CHECK(p.get_value("flag"));
  CHECK(p.get_value<std::string>("flag") == "foo");

  auto v = p.get_value<std::vector<std::string>>("flag");

  CHECK(v.size() == 1);
  CHECK(v[0] == "foo");
}

TEST_CASE_TEMPLATE("test typecasting functionality", T, int, double,
                   std::string) {
  const char *argv[] = {"tests", "--flag", "2.0"};

  auto argc = length(argv);

  auto p = parser();

  p.add_option("--flag", "-f")
      .help("just=flag")
      .mode(optionparser::StorageMode::STORE_VALUE);

  p.eat_arguments(argc, argv);

  CHECK(p.get_value("flag"));

  auto value = p.get_value<T>("flag");

  CHECK(typeid(value) == typeid(T));
}

TEST_CASE("test short args") {
  const char *argv[] = {"tests", "-s", "test_str", "-m", "str1", "str2",
                        "str3",  "-b", "-l",       "t1", "t2",   "t3"};

  auto argc = length(argv);

  auto p = parser();

  p.add_option("-b")
      .help("just=flag")
      .mode(optionparser::StorageMode::STORE_TRUE);
  p.add_option("-s")
      .help("just=flag")
      .mode(optionparser::StorageMode::STORE_VALUE);
  p.add_option("-m")
      .help("just=flag")
      .mode(optionparser::StorageMode::STORE_MULT_VALUES);
  p.add_option("-l")
      .help("just=flag")
      .mode(optionparser::StorageMode::STORE_MULT_VALUES);

  p.eat_arguments(argc, argv);
  CHECK(p.get_value("b_option"));

  CHECK(p.get_value<std::string>("s_option") == "test_str");

  auto names = p.get_value<std::vector<std::string>>("m_option");
  CHECK(names[0] == "str1");
  CHECK(names[1] == "str2");
  CHECK(names[2] == "str3");

  auto qq = p.get_value<std::vector<std::string>>("l_option");
  CHECK(qq[0] == "t1");
  CHECK(qq[1] == "t2");
  CHECK(qq[2] == "t3");
}

TEST_CASE("test OO functionality") {
  const char *argv[] = {"tests", "asd1", "wqe", "--file", "72",
                        "-l",    "t1",   "t2",  "t3"};

  auto argc = length(argv);

  auto p = parser();

  p.add_option("pos").help("just=flag");

  p.add_option("pos1").help("just=flag");

  p.add_option("--file").help("just=flag");
  p.add_option("pos3").help("just=flag");

  p.add_option("-l")
      .help("just=flag")
      .mode(optionparser::StorageMode::STORE_MULT_VALUES);
  p.eat_arguments(argc, argv);

  CHECK(p.get_value<std::string>("pos") == "asd1");
  CHECK(p.get_value<std::string>("pos1") == "wqe");

  CHECK(p.get_value<int>("pos3") == 72);
  CHECK(p.get_value("file"));
  auto qq = p.get_value<std::vector<std::string>>("l_option");
  CHECK(qq[0] == "t1");
  CHECK(qq[1] == "t2");
  CHECK(qq[2] == "t3");
}
