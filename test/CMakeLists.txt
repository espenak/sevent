include_directories(${sevent_SOURCE_DIR})
function(add_boost_test testname)
	add_executable(${testname} ${ARGV})
	target_link_libraries(${testname} ${Boost_LIBRARIES})
	add_test(${testname} ${testname})
endfunction(add_boost_test)

add_subdirectory(socket)
