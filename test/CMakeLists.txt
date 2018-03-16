# define unit_test function
function(add_unit_test)
 	cmake_parse_arguments(LMADB_TEST "GTEST" "NAME" "SOURCES;LIBRARIES" ${ARGN})
	if(${LMADB_TEST_GTEST})
		list(APPEND LMADB_TEST_LIBRARIES gmock_main)
	endif()

	add_executable(${LMADB_TEST_NAME} ${LMADB_TEST_SOURCES})
	target_link_libraries(${LMADB_TEST_NAME} ${LMADB_TEST_LIBRARIES})
	add_test(NAME ${LMADB_TEST_NAME} COMMAND ${LMADB_TEST_NAME})
endfunction()
