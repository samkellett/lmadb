add_library(lmadb
	src/connection.cpp
	src/lmadb.cpp
	src/parse_statement.cpp
)

add_library(lmadb::lmadb ALIAS lmadb)

target_link_libraries(lmadb PRIVATE lmadb::pdk)

target_include_directories(lmadb
	PUBLIC
		${CMAKE_SOURCE_DIR}/include/lmadb
		src
)
