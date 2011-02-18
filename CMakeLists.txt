cmake_minimum_required(VERSION 2.8)
project(socketevent CXX)

find_package( Boost REQUIRED system serialization thread regex)

add_subdirectory(src)
add_subdirectory(test)

