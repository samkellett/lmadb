# define unit_test function
function(add_unit_test)
	cmake_parse_arguments(UNIT_TEST "GTEST" "NAME" "SOURCES;LIBRARIES" ${ARGN})
	if(${UNIT_TEST_GTEST})
		list(APPEND UNIT_TEST_LIBRARIES gmock_main)
	endif()

	add_executable(${UNIT_TEST_NAME} ${UNIT_TEST_SOURCES})
	target_link_libraries(${UNIT_TEST_NAME} ${UNIT_TEST_LIBRARIES})
	add_test(NAME ${UNIT_TEST_NAME} COMMAND ${UNIT_TEST_NAME})
endfunction()

# define fcheck_test function
function(add_fcheck_test)
	cmake_parse_arguments(FCHECK_TEST "" "NAME" "" ${ARGN})
	set(FCHECK_TOOL ${CMAKE_SOURCE_DIR}/tools/fcheck)

	add_test(
		NAME ${FCHECK_TEST_NAME}
		COMMAND ${FCHECK_TOOL} ${FCHECK_TEST_NAME}
		WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

	# expose the tools to the tests
	# TODO: come up with a better way of doing this.
	set_tests_properties(${FCHECK_TEST_NAME}
		PROPERTIES ENVIRONMENT "PATH=${CMAKE_SOURCE_DIR}/tools:$ENV{PATH}")
endfunction()

add_subdirectory(ast)
add_subdirectory(cxx)
add_subdirectory(lmadb)
add_subdirectory(pdk)
add_subdirectory(storage)
add_subdirectory(tools)
