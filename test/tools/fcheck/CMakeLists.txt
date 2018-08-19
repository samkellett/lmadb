add_fcheck_test(NAME fcheck_success.sh)

add_fcheck_test(NAME fcheck_command_fails.sh)
set_tests_properties(fcheck_command_fails.sh PROPERTIES WILL_FAIL true)

add_fcheck_test(NAME fcheck_check_not_found.sh)
set_tests_properties(fcheck_check_not_found.sh PROPERTIES WILL_FAIL true)

add_fcheck_test(NAME fcheck_check_next_not_next.sh)
set_tests_properties(fcheck_check_next_not_next.sh PROPERTIES WILL_FAIL true)
