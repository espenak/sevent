
include_directories(
	${Boost_INCLUDE_DIRS}
	${sevent_SOURCE_DIR})
link_directories(${Boost_LIBRARY_DIRS})

add_subdirectory(socket)

add_library(sevent STATIC
    EventHandlerMap.cc EventHandlerMap.h
)
target_link_libraries(sevent ${Boost_LIBRARIES})
target_link_libraries(sevent sevent_socket)
