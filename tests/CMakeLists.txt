set(TEST_EXECUTABLE test-optionparser)

add_executable(${TEST_EXECUTABLE} test_parser.cc)
target_include_directories(${TEST_EXECUTABLE} PRIVATE include/)
target_compile_features(${TEST_EXECUTABLE} PRIVATE cxx_std_11)
target_link_libraries(${TEST_EXECUTABLE} ${PROJECT_NAME})