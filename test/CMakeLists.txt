include_directories(${socketevent_SOURCE_DIR}/src/socket)
add_executable(Example Example.cc)
target_link_libraries(Example sevent_socket)
