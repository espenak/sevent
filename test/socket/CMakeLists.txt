find_package( Boost COMPONENTS unit_test_framework )
include_directories(${Boost_INCLUDE_DIRS})
link_directories( ${Boost_LIBRARY_DIRS} )

add_boost_test(TestSEventSocketFacade TestSEventSocketFacade.cc)
target_link_libraries(TestSEventSocketFacade sevent_socket)
