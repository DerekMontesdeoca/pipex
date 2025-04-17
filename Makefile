# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: dmontesd <dmontesd@student.42madrid.com>   +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/03/27 19:01:30 by dmontesd          #+#    #+#              #
#    Updated: 2025/04/17 22:50:17 by dmontesd         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

CC := cc

LIBFT_DIR := libft
LIBFT := $(LIBFT_DIR)/libft.a
CFLAGS := -Wall -Wextra -Werror
ifdef DEBUG
	CFLAGS += -g3
endif
CPPFLAGS := -I $(LIBFT_DIR)/include
LDFLAGS :=
HEADERS := pipex.h args.h arg_parser.h

NAME := pipex
SRCS := command_child.c command_fork.c env_path.c main.c path.c \
		path_iter.c arg_parser.c parse_arg.c arg_parser_states.c fd.c
OBJS := $(SRCS:.c=.o)

BONUS_SRCS := main_bonus.c command_child.c command_fork.c env_path.c path.c \
		path_iter.c arg_parser.c parse_arg.c arg_parser_states.c fd.c
BONUS_OBJS := $(BONUS_SRCS:.c=.o)

all: $(NAME)

bonus: .bonus_build

.bonus_build: $(BONUS_OBJS) $(LIBFT)
	touch .bonus_build
	$(CC) $(LDFLAGS) -o pipex $^

$(NAME): $(OBJS) $(LIBFT)
	rm -f .bonus_build
	$(CC) $(LDFLAGS) -o $@ $^

%.o: %.c $(HEADERS)
	$(CC) -c $(CFLAGS) $(CPPFLAGS) -o $@ $<

$(LIBFT):
	$(MAKE) -C $(LIBFT_DIR)

clean:
	rm -rf $(OBJS) $(BONUS_OBJS) .bonus_build

fclean: clean
	rm -rf $(NAME)

re: fclean all

.PHONY: all bonus re fclean clean
