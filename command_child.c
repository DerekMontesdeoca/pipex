/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   command_child.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dmontesd <dmontesd@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/31 18:05:13 by dmontesd          #+#    #+#             */
/*   Updated: 2025/04/10 03:20:58 by dmontesd         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include "libft/libft.h"
#include "pipex.h"

void	child_error(char *str, int exit_status)
{
	perror(str);
	exit(exit_status);
}

static void	try_paths(t_command *command)
{
	int			exit_code;
	extern char	**environ;
	t_path_iter	path_iter;

	if (!path_iter_make(&path_iter, environ, command->args.split_args[0]))
		child_error(command->args.split_args[0], EXIT_FAILURE);
	while (path_iter_next(&path_iter))
	{
		exit_code = execve(path_iter.path, command->args.split_args, environ);
		if (exit_code < 0 && errno != ENOENT && errno != ENOTDIR)
			child_error(command->args.split_args[0], EXIT_FAILURE);
	}
	if (exit_code < 0)
		child_error(command->args.split_args[0], EXIT_FAILURE);
}

void	child_execvpe(t_command *command)
{
	extern char	**environ;

	if (ft_strchr(command->args.split_args[0], '/') != NULL)
	{
		if (execve(command->args.split_args[0],
				command->args.split_args, environ) < 0)
			child_error(command->args.split_args[0], EXIT_FAILURE);
	}
	else
		try_paths(command);
}

void	child_setup_pipes(t_command *command)
{
	if (command->pip_in[1] != -1)
	{
		close(command->pip_in[1]);
		command->pip_in[1] = -1;
	}
	if (command->pip_out[0] != -1)
	{
		close(command->pip_out[0]);
		command->pip_out[0] = -1;
	}
	if (command->pip_in[0] != -1)
	{
		if (dup2(command->pip_in[0], STDIN_FILENO) < 0)
			child_error("dup2", EXIT_FAILURE);
		close(command->pip_in[0]);
	}
	if (command->pip_out[1] != -1)
	{
		if (dup2(command->pip_out[1], STDOUT_FILENO) < 0)
			child_error("dup2", EXIT_FAILURE);
		close(command->pip_out[1]);
	}
}

void	child_redirect_fds(t_command *command)
{
	int	fd;
	int	flag;

	if (command->redirection != NULL && command->redirect_fd == STDIN_FILENO)
		flag = O_RDONLY;
	else if (command->redirection != NULL
		&& command->redirect_fd == STDOUT_FILENO)
		flag = O_WRONLY | O_CREAT | O_TRUNC;
	else
		return ;
	fd = open(command->redirection, flag, 0664);
	if (fd < 0)
		child_error(command->args.split_args[0], EXIT_FAILURE);
	if (dup2(fd, command->redirect_fd) < 0)
		child_error(command->args.split_args[0], EXIT_FAILURE);
	close(fd);
}
