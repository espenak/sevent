include_directories(${sevent_SOURCE_DIR})

add_executable(EventExample EventExample.cc)
target_link_libraries(EventExample sevent)
target_link_libraries(EventExample ${Boost_LIBRARIES})

add_executable(SocketExample.cc SocketExample.cc)
target_link_libraries(SocketExample.cc sevent_socket)
target_link_libraries(SocketExample.cc ${Boost_LIBRARIES})
