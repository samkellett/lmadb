add_library(lmadb
	src/connection.cpp
	src/lmadb.cpp
	src/statement.cpp
)

add_library(lmadb::lmadb ALIAS lmadb)

target_link_libraries(lmadb
	PRIVATE
		lmadb::ast
		lmadb::cxx
		lmadb::meta
		fmt::fmt
)

target_include_directories(lmadb
	PUBLIC
		${CMAKE_SOURCE_DIR}/include/lmadb
		src
)
