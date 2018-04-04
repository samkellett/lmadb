add_library(lmadb
	src/parse_statement.cpp
)

add_library(lmadb::lmadb ALIAS lmadb)

target_link_libraries(lmadb PRIVATE lmadb::pdk)

target_include_directories(lmadb
	PUBLIC
		${CMAKE_CURRENT_SOURCE_DIR}/src
)
