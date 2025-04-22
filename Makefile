# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: dmontesd <dmontesd@student.42madrid.com>   +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/03/27 19:01:30 by dmontesd          #+#    #+#              #
#    Updated: 2025/04/22 23:16:05 by dmontesd         ###   ########.fr        #
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
SRCS := spawnp.c spawnp_child.c main.c env.c arg_parser.c parse_arg.c \
		arg_parser_states.c fd.c pipeline.c execvp.c command.c heredoc.c \
		mkstemp.c command_actions.c
OBJS := $(SRCS:.c=.o)

BONUS_SRCS := main_bonus.c spawnp.c spawnp_child.c execvp.c env.c arg_parser.c \
			  parse_arg.c arg_parser_states.c fd.c pipeline.c heredoc.c \
			  mkstemp.c command.c command_actions.c
BONUS_OBJS := $(BONUS_SRCS:.c=.o)

all: $(NAME)

bonus: .bonus_build

.bonus_build: $(BONUS_OBJS) $(LIBFT)
	$(CC) $(LDFLAGS) -o pipex $^
	touch .bonus_build

$(NAME): $(OBJS) $(LIBFT)
	rm -f .bonus_build
	$(CC) $(LDFLAGS) -o $@ $^

%.o: %.c $(HEADERS)
	$(CC) -c $(CFLAGS) $(CPPFLAGS) -o $@ $<

$(LIBFT):
	$(MAKE) -C $(LIBFT_DIR)

clean:
	rm -rf $(OBJS) $(BONUS_OBJS) .bonus_build
	$(MAKE) -C $(LIBFT_DIR) clean

fclean: clean
	rm -rf $(NAME)
	$(MAKE) -C $(LIBFT_DIR) fclean

re: fclean all

.PHONY: all bonus re fclean clean
