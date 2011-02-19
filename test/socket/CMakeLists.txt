find_package( Boost COMPONENTS unit_test_framework )
include_directories(${Boost_INCLUDE_DIRS})
link_directories( ${Boost_LIBRARY_DIRS} )

add_boost_test(TestSEventSocket TestSEventSocket.cc)
target_link_libraries(TestSEventSocket sevent_socket)
