add_library(pdk INTERFACE)
add_library(lmadb::pdk ALIAS pdk)

target_compile_options(pdk INTERFACE -Wno-gnu-string-literal-operator-template)
target_link_libraries(pdk INTERFACE lmadb::cxx)

target_include_directories(pdk
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)
