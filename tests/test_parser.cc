#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <iterator>

#include "doctest/doctest.h"
#include "include/optionparser.h"

template <class T, size_t N>
constexpr size_t length(T (&)[N]) {
  return N;
}

TEST_CASE("test parser functionality") {
  const char *argv[] = {"tests",
                        "--flag",
                        "--only_large",
                        "-s",
                        "-q",
                        "first_short2",
                        "--in-fil-e",
                        "-dash",
                        "--store_str",
                        "qqqdash",
                        "--store_num",
                        "72",
                        "--store_multy_s",
                        "str1",
                        "str2",
                        "str3",
                        "str4",
                        "str5",
                        "-b",
                        "--pp",
                        "-singledash",
                        "hellooo",
                        "--boolean"};

  int argc = length(argv);

  optionparser::OptionParser p(
      "A test to make sure that this option parser works");

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
  CHECK(check_is_flag_set == true);
  if (p.get_value("only_large")) {
    check_is_flag_set = true;
  }
  CHECK(check_is_flag_set == true);
  if (p.get_value("s_option")) {
    check_is_flag_set = true;
  }
  CHECK(check_is_flag_set == true);
  if (p.get_value("first_short1")) {
    check_is_flag_set = true;
  }
  CHECK(check_is_flag_set == true);
  if (p.get_value("first_short2")) {
    check_is_flag_set = true;
  }
  CHECK(check_is_flag_set == true);
  if (p.get_value("in-fil-e")) {
    check_is_flag_set = true;
  }
  CHECK(check_is_flag_set == true);
  if (p.get_value("dash")) {
    check_is_flag_set = true;
  }
  CHECK(check_is_flag_set == true);

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
    CHECK(p.get_value<bool>("boolean") == true);
  }
}

TEST_CASE("test OO functionality") {
  const char *argv[] = {"tests",    "--flag asdsaflag", "bsadsad",
                        "bqwewqeq", "--boolean",        "--long l1 l2 l3"};

  auto argc = length(argv);

  optionparser::OptionParser p(
      "A test to make sure that this option parser works");

  p.add_option("--flag", "-f")
      .help("just=flag")
      .mode(optionparser::StorageMode::STORE_MULT_VALUES);
  p.add_option("--long", "-l")
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
  if (p.get_value("long")) {
    check_is_flag_set = true;
    auto names = p.get_value<std::vector<std::string>>("long");
    CHECK(names[0] == "l1");
    CHECK(names[1] == "l2");
    CHECK(names[2] == "l3");
  }
}

TEST_CASE("test default argument not passed") {
  const char *argv[] = {"tests", "--flag"};

  auto argc = length(argv);

  optionparser::OptionParser p("");

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

  optionparser::OptionParser p(
      "A test to make sure that this option parser works");

  p.add_option("--flag", "-f")
      .help("just=flag")
      .mode(optionparser::StorageMode::STORE_VALUE);

  p.eat_arguments(argc, argv);

  CHECK(p.get_value("flag"));

  auto value = p.get_value<T>("flag");

  CHECK(typeid(value) == typeid(T));
}

TEST_CASE("test short args") {
  const char *argv[] = {"tests", "-s",   "test_str", "-m",         "str1",
                        "str2",  "str3", "-b",       "-l t1 t2 t3"};

  auto argc = length(argv);

  optionparser::OptionParser p(
      "A test to make sure that this option parser works");

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

TEST_CASE("test positional args") {
  const char *argv[] = {"tests", "pos1", "pos2", "-b", "pos3", "7",
                        "-s",    "pos5", "7",    "5",  "99"};

  auto argc = length(argv);

  optionparser::OptionParser p(
      "A test to make sure that this option parser works");

  p.add_option("pos1")
      .help("just=flag")
      .mode(optionparser::StorageMode::STORE_TRUE);

  p.add_option("-b")
      .help("just=flag")
      .mode(optionparser::StorageMode::STORE_TRUE);

  p.add_option("-s")
      .help("just=flag")
      .mode(optionparser::StorageMode::STORE_TRUE);
  p.add_option("pos2")
      .help("just=flag")
      .mode(optionparser::StorageMode::STORE_TRUE);
  p.add_option("pos3")
      .help("just=flag")
      .mode(optionparser::StorageMode::STORE_VALUE);

  p.add_option("pos4")
      .help("just=flag")
      .mode(optionparser::StorageMode::STORE_VALUE);
  p.add_option("pos5")
      .help("just=flag")
      .mode(optionparser::StorageMode::STORE_MULT_VALUES);

  p.eat_arguments(argc, argv);
  CHECK(p.get_value("pos1"));
  CHECK(p.get_value("pos2"));
  CHECK(p.get_value<double>("pos3") == 7);

  auto names = p.get_value<std::vector<std::string>>("pos5");
  CHECK(names[0] == "7");
  CHECK(names[1] == "5");
  CHECK(names[2] == "99");
}
