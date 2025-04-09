#include <stddef.h>
#include <stdarg.h>
#include <setjmp.h>
#include <stdint.h>
#include <cmocka.h>
#include <string.h>
#include "arg_parse.c"

static void test_arg_parse(const char *raw_args, t_args *expected_args)
{
	t_args args_out;
	assert_true(parse_command_args(raw_args, &args_out));
	assert_uint_equal(args_out.n_args, expected_args->n_args);
	assert_uint_equal(args_out.len, expected_args->len);
	size_t i = 0;
	for (i = 0; args_out.split_args[i] != NULL; ++i);
	assert_uint_equal(args_out.n_args, i);
	assert_memory_equal(args_out.args, expected_args->args, expected_args->len + 1);
	for (i = 0; i < expected_args->n_args; ++i)
		assert_string_equal(args_out.split_args[i], expected_args->split_args[i]);
	assert_null(args_out.split_args[i]);
	assert_null(expected_args->split_args[i]);
	args_free_contents(&args_out);
}

static void test_basic(void **state)
{
	(void) state;
	const char *raw_args = "grep -q hola";
	char *split_args[] = {"grep", "-q", "hola", NULL};
	t_args expected = {
		.split_args = split_args,
		.n_args = 3,
		.len = strlen(raw_args),
		.args = "grep\0-q\0hola",
	};
	test_arg_parse(raw_args, &expected);
}

static void test_empty(void **state)
{
	(void) state;
	const char *raw_args = "";
	char *split_args[] = {"", NULL};
	t_args expected = {
		.split_args = split_args,
		.n_args = 1,
		.len = strlen(raw_args),
		.args = "",
	};
	test_arg_parse(raw_args, &expected);
}

static void test_null(void **state)
{
	(void) state;
	assert_false(parse_command_args(NULL , NULL));
}

static void test_quotes(void **state)
{
	(void) state;
	const char *raw_args = "sed -E 's/hola/chao/g'  ";
	char *split_args[] = {"sed", "-E", "s/hola/chao/g", NULL};
	t_args expected = {
		.split_args = split_args,
		.n_args = 3,
		.len = 20,
		.args = "sed\0-E\0s/hola/chao/g",
	};
	test_arg_parse(raw_args, &expected);
}

static void test_double_quotes(void **state)
{
	(void) state;
	const char *raw_args = "sed -E \"s/hola/chao/g\"  h";
	char *split_args[] = {"sed", "-E", "s/hola/chao/g", "h", NULL};
	t_args expected = {
		.split_args = split_args,
		.n_args = 4,
		.len = 22,
		.args = "sed\0-E\0s/hola/chao/g\0h",
	};
	test_arg_parse(raw_args, &expected);
}

static void test_escape_backslash(void **state)
{
	(void) state;
	const char *raw_args = "so why 'am i ' this\\ bs \\\\ \"\\\"\"     ";
	char *split_args[] = {"so", "why", "am i ", "this bs", "\\", "\"", NULL};
	t_args expected = {
		.split_args = split_args,
		.n_args = 6,
		.len = 24,
		.args = "so\0why\0am i \0this bs\0\\\0\"",
	};
	test_arg_parse(raw_args, &expected);
}

int	main()
{
	const struct CMUnitTest tests[] = {
		cmocka_unit_test(test_basic),
		cmocka_unit_test(test_empty),
		cmocka_unit_test(test_null),
		cmocka_unit_test(test_quotes),
		cmocka_unit_test(test_double_quotes),
		cmocka_unit_test(test_escape_backslash),
	};
	return (cmocka_run_group_tests(tests, NULL, NULL));
}
