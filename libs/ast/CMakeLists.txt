add_library(ast
	src/parse_statement.cpp
)

add_library(lmadb::ast ALIAS ast)

target_link_libraries(ast
	PUBLIC
		lmadb::cxx
	PRIVATE
		lmadb::pdk
)

target_include_directories(ast
	PUBLIC
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
		src
)
