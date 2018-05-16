pybind11_add_module(pylmadb THIN_LTO
	src/pylmadb.cpp
)

add_library(lmadb::pylmadb ALIAS pylmadb)

target_link_libraries(pylmadb PRIVATE lmadb::lmadb)

configure_file(src/setup.py ${CMAKE_CURRENT_BINARY_DIR}/setup.py COPYONLY)
