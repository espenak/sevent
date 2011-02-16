include_directories(
	${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})

add_library(socketevent STATIC
    Session.cc Session.h)
target_link_libraries(socketevent ${Boost_LIBRARIES})
