cmake_minimum_required(VERSION 3.12)

project("optionparser"
        VERSION 0.2.0
        DESCRIPTION "A lightweight header-only option parser designed for headache-minimization in C++."
        HOMEPAGE_URL "https://github.com/lukedeo/option-parser")

include(CTest)
include(GNUInstallDirs)

add_library(${PROJECT_NAME} INTERFACE)
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

target_include_directories(
        ${PROJECT_NAME}
        INTERFACE
        include/
)

target_compile_features(${PROJECT_NAME} INTERFACE cxx_std_11)

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    add_subdirectory(tests)
    add_test(NAME test-optionparser COMMAND tests/test-optionparser)
endif()