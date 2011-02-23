cmake_minimum_required(VERSION 2.8)
project(sevent CXX)
enable_testing()

find_package( Boost REQUIRED system serialization thread)

add_subdirectory(sevent)
add_subdirectory(test)
add_subdirectory(examples)
