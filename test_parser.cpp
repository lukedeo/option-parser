#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest/doctest.h"
#include "include/parser.hh"


TEST_CASE("testing current parser functionality") 
{
    int argc = 18;
    const char * argv[18];
    argv[0] = "tests";
    argv[1] = "--flag";
    argv[2] = "--only_large";
    argv[3] = "-s";
    argv[4] = "-q";
    argv[5] = "--first_short2";
    argv[6] = "--in-fil-e";
    argv[7] = "-dash";
    argv[8] = "--store_str";
    argv[9] = "test_str";
    argv[10] = "--store_num";
    argv[11] = "72";
    argv[12] = "-m";
    argv[13] = "str1";
    argv[14] = "str2";
    argv[15] = "str3";
    argv[16] = "str4";
    argv[17] = "str5";
 

    optionparser::parser p("A test to make sure that this option parser works");


    p.add_option("--flag", "-f") .help("just flag");
    p.add_option("--only_large") .help("set only large opt");
    p.add_option("-s") .help("set only small opt");
    p.add_option("-q", "--first_short1") .help("first_short1");
    p.add_option("-w", "--first_short2") .help("first_short2");
    p.add_option("--in-fil-e", "-i").help("dashes in name");
    p.add_option("-dash") .help("only one dash for large opt");
    p.add_option("--store_str", "-v") .help("store single str val").mode(optionparser::store_value);
    p.add_option("--store_num", "-n") .help("store single num val").mode(optionparser::store_value);
    p.add_option("--store_multy_s", "-m") .help("store multy str val").mode(optionparser::store_mult_values);
  
    p.eat_arguments(argc, argv);

    bool chech_is_flag_set = false; 
    if (p.get_value("flag"))
    {
      chech_is_flag_set = true;
    }
    CHECK(chech_is_flag_set == true);
    if (p.get_value("only_large"))
    {
      chech_is_flag_set = true;
    }
    CHECK(chech_is_flag_set == true);
    if (p.get_value("s_option"))
    {
      chech_is_flag_set = true;
    }
    CHECK(chech_is_flag_set == true);
     if (p.get_value("first_short1"))
    {
      chech_is_flag_set = true;
    }
    CHECK(chech_is_flag_set == true);
     if (p.get_value("first_short2"))
    {
      chech_is_flag_set = true;
    }
    CHECK(chech_is_flag_set == true);
     if (p.get_value("in-fil-e"))
    {
      chech_is_flag_set = true;
    }
    CHECK(chech_is_flag_set == true);
    if (p.get_value("dash"))
    {
      chech_is_flag_set = true;
    }
    CHECK(chech_is_flag_set == true);

    if (p.get_value("store_str"))
    {
        auto names = p.get_value<std::vector<std::string>>("store_str");
        CHECK(names[0] == argv[9]);
    }
    
    if (p.get_value("store_num"))
    {
        CHECK(p.get_value<int>("store_num") == 72);
    }

    if (p.get_value("store_multy_s"))
    {
        auto names = p.get_value<std::vector<std::string>>("store_multy_s");
        CHECK(names[0] == argv[13]);
        CHECK(names[1] == argv[14]);
        CHECK(names[2] == argv[15]);
        CHECK(names[3] == argv[16]);
        CHECK(names[4] == argv[17]);
    }


}