cmake_minimum_required(VERSION 2.8)
project(sevent CXX)
enable_testing()

find_package( Boost 1.46 REQUIRED system serialization thread)
include_directories(${Boost_INCLUDE_DIR})

message("BOOST found at:")
message("   ${Boost_INCLUDE_DIRS}")
message("   ${Boost_LIBRARY_DIRS}")
message("   ${Boost_LIBRARIES}")

add_subdirectory(sevent)
add_subdirectory(test)
add_subdirectory(examples)
