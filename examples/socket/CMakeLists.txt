add_executable(FacadeExample FacadeExample.cc)
target_link_libraries(FacadeExample sevent_socket)
target_link_libraries(FacadeExample ${Boost_LIBRARIES})
