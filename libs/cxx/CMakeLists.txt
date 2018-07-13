add_library(cxx INTERFACE)
add_library(lmadb::cxx ALIAS cxx)

target_link_libraries(cxx
	INTERFACE
		fmt::fmt
		Boost::filesystem)

target_include_directories(cxx
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)
