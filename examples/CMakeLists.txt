include_directories(${sevent_SOURCE_DIR})

add_executable(SocketExample.cc SocketExample.cc)
target_link_libraries(SocketExample.cc sevent_socket)
target_link_libraries(SocketExample.cc ${Boost_LIBRARIES})

function(add_sevent_executable execname)
    add_executable(${execname} ${execname}.cc)
    target_link_libraries(${execname} sevent)
    target_link_libraries(${execname} ${Boost_LIBRARIES})
endfunction(add_sevent_executable)

add_sevent_executable(EventExample)
add_sevent_executable(Server)
add_sevent_executable(Client)
add_sevent_executable(FancyServer)
add_sevent_executable(FancyClient)
