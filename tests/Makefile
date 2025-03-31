SHELL := /bin/bash
CC := cc
UNIT_TESTS := test_path test_env_path_statics
UNIT_TESTS := $(UNIT_TESTS:%=unit_tests/%)
TEST := test.sh
BONUS_TEST := test_bonus.sh
BATS := local/bin/bats
CMOCKA := local/lib/libcmocka.so
LIBFT := ../libft/libft.a
override CFLAGS += -Wall -Wextra -Werror -g3
override CPPFLAGS += -I.. -Ilocal/include -I../libft
override LDFLAGS += -Llocal/lib -lcmocka -Wl,-rpath=$(shell realpath ./local/lib)

all:
	$(MAKE) test
	$(MAKE) test
	$(MAKE) -C .. fclean
	$(MAKE) test_bonus
	$(MAKE) -C .. fclean

test: $(BATS)
	@$(MAKE) -C ..
	./$(NAME)

test_bonus: $(BATS)
	@$(MAKE) -C .. bonus
	./$(BONUS_NAME)  

unit_test: $(UNIT_TESTS)
	for t in $(UNIT_TESTS); do \
		./$$t; \
	done

unit_tests/test_path: unit_tests/test_path.o ../path.o $(LIBFT)
	$(CC) $(LDFLAGS) -o $@ $^

unit_tests/test_env_path_statics: unit_tests/test_env_path_statics.o ../path.o $(LIBFT)
	$(CC) $(LDFLAGS) -o $@ $^

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
	rm -rf bats-core cmocka $(UNIT_TESTS:%=%.o)
	$(MAKE) -C .. clean

fclean: clean
	rm -rf $(UNIT_TESTS)

.PHONY: all test test_bonus clean unit_test fclean
