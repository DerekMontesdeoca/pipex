# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: dmontesd <dmontesd@student.42madrid.com>   +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/03/27 19:01:30 by dmontesd          #+#    #+#              #
#    Updated: 2025/03/27 19:06:01 by dmontesd         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME := pipex
SRCS := pipex.c
OBJS := $(SRCS:.c=.o)
HEADERS := pipex.h
CFLAGS := 
CPPFLAGS := 
LDFLAGS :=

all:

$(NAME):

clean:

fclean:

re:

.PHONY: all re fclean clean
