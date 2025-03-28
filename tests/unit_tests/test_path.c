#include <stddef.h>
#include <stdarg.h>
#include <setjmp.h>
#include <stdint.h>
#include <cmocka.h>
#include <string.h>
#include "pipex.h"

static void test_simple_path_join(void **state)
{
	(void) state;
	char *p1 = "/simple/path";
	char *p2 = "ls";
	char result[16];
	path_join(p1, p2, result, 16);
	assert_string_equal(result, "/simple/path/ls");
}

static void test_trailing_slash(void **state)
{
	(void) state;
	char *p1 = "/simple/path////";
	char *p2 = "ls";
	char result[strlen(p1) + 2 + 2];
	path_join(p1, p2, result, strlen(p1) + 2 + 2);
	assert_string_equal(result, "/simple/path/ls");
}

static void test_result_buffer_bigger_than_len(void **state)
{
	(void) state;
	char *p1 = "/simple/path////";
	char *p2 = "ls";
	char result[50];
	path_join(p1, p2, result, strlen(p1) + 2 + 2);
	assert_string_equal(result, "/simple/path/ls");
}

int	main()
{
	const struct CMUnitTest tests[] = {
		cmocka_unit_test(test_simple_path_join),
		cmocka_unit_test(test_trailing_slash),
		cmocka_unit_test(test_result_buffer_bigger_than_len),
	};
	return (cmocka_run_group_tests(tests, NULL, NULL));
}
