cmake_minimum_required(VERSION 3.10.2 FATAL_ERROR)
project(lmadb)

include(CTest)

find_package(Boost 1.66 REQUIRED)

enable_testing()

set(LMADB_STD_CXX -std=c++17)

# in-tree dependencies
add_subdirectory(ext/fmt EXCLUDE_FROM_ALL)
add_subdirectory(ext/googletest EXCLUDE_FROM_ALL)

set(PYBIND11_CPP_STANDARD ${LMADB_STD_CXX})
add_subdirectory(ext/pybind11 EXCLUDE_FROM_ALL)

# Compile time options and features used by all of the targets.
set(LMADB_CXX_OPTIONS ${LMADB_STD_CXX} -stdlib=libc++)
add_compile_options(
	"$<$<COMPILE_LANGUAGE:CXX>:${LMADB_CXX_OPTIONS}>"
	-fcolor-diagnostics
	-Wall
	-Wextra
	-Werror
)

# subprojects
add_subdirectory(libs)
add_subdirectory(test)
add_subdirectory(tools)
