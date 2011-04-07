include_directories(${sevent_SOURCE_DIR})


function(add_sevent_example_executable execname)
    add_executable("Sevent${execname}" ${execname}.cc)
    target_link_libraries("Sevent${execname}" sevent)
    target_link_libraries("Sevent${execname}" ${Boost_LIBRARIES})
endfunction(add_sevent_example_executable)

#add_sevent_example_executable(SocketExample)
#add_sevent_example_executable(EventExample)
add_sevent_example_executable(SimpleServer)
add_sevent_example_executable(SimpleClient)

#add_subdirectory(showcase)
