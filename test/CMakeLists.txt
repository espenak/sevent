include_directories(${sevent_SOURCE_DIR})
add_executable(Example Example.cc)
target_link_libraries(Example sevent_socket)
