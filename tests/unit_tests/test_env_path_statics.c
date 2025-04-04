#include <stddef.h>
#include <stdarg.h>
#include <setjmp.h>
#include <stdint.h>
#include <cmocka.h>
#include <string.h>
#include "env_path.c"

static void test_count_paths_and_len(
		const char *path, int expected_paths, size_t expected_len
) {
	size_t	n_paths;
	size_t len;
	count_paths_and_len(path, &n_paths, &len);
	assert_int_equal(n_paths, expected_paths);
	assert_uint_equal(expected_len, len);
}

static void test_count_paths_and_len_simple(void **state)
{
	(void) state;
	test_count_paths_and_len("hola", 1, 4);
}

static void test_count_paths_and_len_2(void **state)
{
	(void) state;
	test_count_paths_and_len("hola:chao", 2, 9);
}

static void test_count_paths_and_len_middle_colons(void **state)
{
	(void) state;
	test_count_paths_and_len("hola::chao", 3, 11);
}

static void test_count_paths_and_len_leading_colon(void **state)
{
	(void) state;
	test_count_paths_and_len(":/usr/bin:/usr/local/bin", 3, 25);
}

static void test_count_paths_and_len_trailing_colon(void **state)
{
	(void) state;
	test_count_paths_and_len("/:/:/:/:", 5, 9);
}

static void test_count_paths_and_len_mix_colons(void **state)
{
	(void) state;
	test_count_paths_and_len(":/usr/bin::::/ilik:e::to:", 10, 31);
}

static void test_count_paths_and_len_all_colons(void **state)
{
	(void) state;
	test_count_paths_and_len("::::::", 7, 13);
}

static void test_count_paths_and_len_empty(void **state)
{
	(void) state;
	test_count_paths_and_len("", 1, 1);
}

static void test_copy_and_split(
		const char *raw_path,
		char *expected_raw_path,
		int expected_raw_path_size,
		char **expected_paths,
		int expected_paths_size
) {
	t_env_path env_path = {
		.paths = malloc(sizeof(char *) * expected_paths_size), .paths_size = expected_paths_size,
		.raw_path = malloc(expected_raw_path_size), .raw_path_len = expected_raw_path_size
	};
	assert_non_null(env_path.raw_path);
	assert_non_null(env_path.paths);
	copy_and_split_path(&env_path, raw_path);
	assert_memory_equal(env_path.raw_path, expected_raw_path, expected_raw_path_size);
	for (int i = 0; i < expected_paths_size; ++i)
		assert_string_equal(env_path.paths[i], expected_paths[i]);
	free(env_path.raw_path);
	free(env_path.paths);
}

static void	test_copy_and_split_basic(void **state)
{
	(void) state;
	test_copy_and_split("hola", "hola", 5, (char*[]){"hola"}, 1);
}

static void	test_copy_and_split_2(void **state)
{
	(void) state;
	test_copy_and_split(
				"hola:chao", "hola\0chao", 10, (char*[]){"hola", "chao"}, 2
				);
}

static void	test_copy_and_split_middle_colons(void **state)
{
	(void) state;
	test_copy_and_split(
				"/usr/bin///::chao::dale", 
				"/usr/bin///\0.\0chao\0.\0dale",
				26,
				(char*[]){"/usr/bin///", ".", "chao", ".", "dale"}, 5
				);
}

static void	test_copy_and_split_all_colons(void **state)
{
	(void) state;
	test_copy_and_split(
				"::::::::",
				".\0.\0.\0.\0.\0.\0.\0.\0.", 18,
				(char*[]){".", ".", ".", ".", ".", ".", ".", ".", "." }, 9
				);
}

static void	test_copy_and_split_empty(void **state)
{
	(void) state;
	test_copy_and_split(
				"",
				".", 2,
				(char*[]){"."}, 1
				);
}

static void	test_copy_and_split_trailing(void **state)
{
	(void) state;
	test_copy_and_split(
				"vamosvamos:",
				"vamosvamos\0.", 13,
				(char*[]){"vamosvamos", "."}, 2
				);
}

int	main()
{
	const struct CMUnitTest tests[] = {
		cmocka_unit_test(test_count_paths_and_len_simple),
		cmocka_unit_test(test_count_paths_and_len_2),
		cmocka_unit_test(test_count_paths_and_len_middle_colons),
		cmocka_unit_test(test_count_paths_and_len_leading_colon),
		cmocka_unit_test(test_count_paths_and_len_trailing_colon),
		cmocka_unit_test(test_count_paths_and_len_mix_colons),
		cmocka_unit_test(test_count_paths_and_len_all_colons),
		cmocka_unit_test(test_count_paths_and_len_empty),
		cmocka_unit_test(test_copy_and_split_basic),
		cmocka_unit_test(test_copy_and_split_2),
		cmocka_unit_test(test_copy_and_split_middle_colons),
		cmocka_unit_test(test_copy_and_split_all_colons),
		cmocka_unit_test(test_copy_and_split_empty),
		cmocka_unit_test(test_copy_and_split_trailing),
	};
	return (cmocka_run_group_tests(tests, NULL, NULL));
}
