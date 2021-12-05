cmake_minimum_required(VERSION 3.15)
project(test_lib VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_library(lib INTERFACE)
target_include_directories(lib 
    INTERFACE 
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/hana/include")

if(LIB_ENABLE_UNITTEST)
enable_testing()
add_subdirectory(test)
endif()