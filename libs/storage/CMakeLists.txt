add_library(storage
	src/insert.cpp
	src/validate.cpp
)

add_library(lmadb::storage ALIAS storage)

target_link_libraries(storage
	PUBLIC
		lmadb::ast
		lmadb::meta
)

target_include_directories(storage
	PUBLIC
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
		src
)
