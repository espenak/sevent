include_directories(${sevent_SOURCE_DIR})
add_subdirectory(socket)

add_executable(EventExample EventExample.cc)
target_link_libraries(EventExample sevent)
target_link_libraries(EventExample ${Boost_LIBRARIES})
