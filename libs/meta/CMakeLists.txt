add_library(meta
	src/reader.cpp
	src/writer.cpp
)

add_library(lmadb::meta ALIAS meta)

target_link_libraries(meta
	PRIVATE
		fmt::fmt
		lmadb::ast
		lmadb::cxx
)

target_include_directories(meta
	PUBLIC
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
		src
)
