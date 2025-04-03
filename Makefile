# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: dmontesd <dmontesd@student.42madrid.com>   +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/03/27 19:01:30 by dmontesd          #+#    #+#              #
#    Updated: 2025/04/01 15:41:32 by dmontesd         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

CC := cc
NAME := pipex
SRCS := command_child.c command_fork.c env_path.c main.c path.c \
		pointer_split.c
OBJS := $(SRCS:.c=.o)
HEADERS := pipex.h
LIBFT_DIR := libft
LIBFT := $(LIBFT_DIR)/libft.a
CFLAGS := -g3
CPPFLAGS := -I $(LIBFT_DIR)
LDFLAGS :=

all: $(NAME)

$(NAME): $(OBJS) $(HEADERS) $(LIBFT)
	$(CC) $(LDFLAGS) -o $@ $(filter-out $(HEADERS), $^)

%.o: %.c
	$(CC) -c $(CFLAGS) $(CPPFLAGS) -o $@ $<

$(LIBFT):
	$(MAKE) -C $(LIBFT_DIR)

clean:
	rm -rf $(OBJS)

fclean: clean
	rm -rf $(NAME)

re:

.PHONY: all re fclean clean
