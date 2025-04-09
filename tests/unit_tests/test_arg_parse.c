#include <stddef.h>
#include <stdarg.h>
#include <setjmp.h>
#include <stdint.h>
#include <cmocka.h>
#include <string.h>
#include "pipex.h"


int	main()
{
	const struct CMUnitTest tests[] = {
	};
	return (cmocka_run_group_tests(tests, NULL, NULL));
}
