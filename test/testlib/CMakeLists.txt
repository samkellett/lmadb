add_library(testlib INTERFACE)
add_library(lmadb::testlib ALIAS testlib)

target_link_libraries(testlib INTERFACE lmadb::cxx)

target_include_directories(testlib
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)
