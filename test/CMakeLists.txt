find_package( Boost COMPONENTS unit_test_framework )
include_directories(${Boost_INCLUDE_DIRS})
link_directories( ${Boost_LIBRARY_DIRS} )

include_directories(${sevent_SOURCE_DIR})

function(add_boost_test testname)
	add_executable(${testname} ${ARGV1})
	target_link_libraries(${testname} ${Boost_LIBRARIES})
	add_test(${testname} ${testname})
endfunction(add_boost_test)

add_subdirectory(socket)
