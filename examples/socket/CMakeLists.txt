add_executable(Example Example.cc)
target_link_libraries(Example sevent_socket)
target_link_libraries(Example ${Boost_LIBRARIES})

add_executable(FacadeExample FacadeExample.cc)
target_link_libraries(FacadeExample sevent_socket)
target_link_libraries(FacadeExample ${Boost_LIBRARIES})
