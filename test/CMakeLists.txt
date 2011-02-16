include_directories(${socketevent_SOURCE_DIR}/src)
add_executable(Example Example.cc)
target_link_libraries(Example socketevent)
