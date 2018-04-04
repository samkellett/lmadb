add_library(cxx INTERFACE)
add_library(lmadb::cxx ALIAS cxx)

target_include_directories(cxx
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)
