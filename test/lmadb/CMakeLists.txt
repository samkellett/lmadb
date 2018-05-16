add_unit_test(NAME test_c_api SOURCES test_c_api.c LIBRARIES lmadb::lmadb)
add_unit_test(GTEST NAME test_parse_statement SOURCES test_parse_statement.cpp LIBRARIES lmadb::lmadb)
