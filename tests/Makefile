SHELL := /bin/bash
CC := cc
UNIT_TESTS := test_path test_env_path_statics test_arg_parse
UNIT_TEST_SRCS := $(UNIT_TESTS:%=unit_tests/%.c)
UNIT_TEST_OBJS := $(UNIT_TEST_SRCS:%.c=%.o)
UNIT_TEST_EXES := $(UNIT_TESTS:%=unit_tests/bin/%)
TEST := test.sh
BONUS_TEST := test_bonus.sh
BATS := local/bin/bats
CMOCKA := local/lib/libcmocka.so
LIBFT := ../libft/libft.a
override CFLAGS += -Wall -Wextra -Werror -g3
override CPPFLAGS += -I.. -Ilocal/include -I../libft/include
override LDFLAGS += -Llocal/lib -lcmocka -Wl,-rpath=$(shell realpath ./local/lib)

all:
	$(MAKE) test
	$(MAKE) -C .. fclean
	$(MAKE) test_bonus
	$(MAKE) -C .. fclean

test: $(BATS) $(TEST) 
	$(MAKE) -C ..
	$^

test_bonus: $(BATS)
	./$(BONUS_NAME)  

unit_test: $(UNIT_TEST_EXES)
	for t in $(UNIT_TEST_EXES); do \
		./$$t; \
	done

unit_tests/bin/test_path: unit_tests/test_path.o ../path.o $(LIBFT) $(CMOCKA)
	$(CC) $(LDFLAGS) -o $@ unit_tests/test_path.o ../path.o $(LIBFT)

unit_tests/bin/test_env_path_statics: unit_tests/test_env_path_statics.o \
	../path.o $(LIBFT)
	$(CC) $(LDFLAGS) -o $@ unit_tests/test_env_path_statics.o \
		../path.o $(LIBFT)

unit_tests/bin/test_arg_parse: unit_tests/test_arg_parse.o $(LIBFT) $(CMOCKA) \
	../arg_parser.o ../parse_arg.o ../arg_parser_states.o
	$(CC) $(LDFLAGS) -o $@ unit_tests/test_arg_parse.o $(LIBFT) ../arg_parser.o \
		../parse_arg.o ../arg_parser_states.o

%.o: %.c
	$(CC) $(CFLAGS) $(CPPFLAGS) -c -o $@ $<

$(LIBFT):
	$(MAKE) -C ../libft

$(BATS):
	mkdir -p "local"
	git clone "https://github.com/bats-core/bats-core"
	./bats-core/install.sh "local"

$(CMOCKA):
	mkdir -p "local"
	git clone "https://github.com/clibs/cmocka"
	mkdir -p cmocka/build
	cmake -S cmocka -B cmocka/build \
		-DCMAKE_INSTALL_PREFIX=./local -DCMAKE_BUILD_TYPE=Debug
	make -C cmocka/build
	make -C cmocka/build install

clean:
	rm -rf bats-core cmocka $(UNIT_TEST_OBJS)
	$(MAKE) -C .. clean

fclean: clean
	rm -rf $(UNIT_TEST_EXES)

.PHONY: all test test_bonus clean unit_test fclean
