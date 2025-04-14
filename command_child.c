/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   command_child.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dmontesd <dmontesd@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/31 18:05:13 by dmontesd          #+#    #+#             */
/*   Updated: 2025/04/14 22:49:56 by dmontesd         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include "libft/libft.h"
#include "pipex.h"

static bool	try_paths(t_command *command, t_path_iter *path_iter);

void run_child(t_pipes *pipes, char **argv)
{
	t_command	command;

	parse_command(&command);
	command_redirect_fds(&command);
	pipes_setup_io(pipes);
	command_execvpe(&command);
}

inline void	child_error(char *str, int exit_status)
{
	perror(str);
	exit(exit_status);
}

void	command_execvpe(t_command *command)
{
	extern char	**environ;
	t_path_iter	path_iter;

	if (!path_iter_make(&path_iter, environ, command->args.arg_pointers[0]))
		child_error(command->args.arg_pointers[0], EXIT_FAILURE);
	if (path_iter.env_path.paths_size == 0)
	{
		ft_fprintf(2, "%s: No such file or directory\n",
				command->args.arg_pointers[0]);
		exit(EXIT_FAILURE);
	}
	if (ft_strchr(command->args.arg_pointers[0], '/') != NULL)
	{
		if (execve(command->args.arg_pointers[0],
				command->args.arg_pointers, environ) < 0)
		{
			path_iter_free_contents(&path_iter);
			child_error(command->args.arg_pointers[0], EXIT_FAILURE);
		}
	}
	else if (!try_paths(command, &path_iter))
	{
		path_iter_free_contents(&path_iter);
		child_error(command->args.arg_pointers[0], EXIT_FAILURE);
	}
}

static void	command_redirect_fds(t_command *command)
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
		child_error(command->redirection, EXIT_FAILURE);
	if (dup2(fd, command->redirect_fd) < 0)
	{
		ft_fprintf(STDERR_FILENO, "%s: dup2: %s\n",
				command->args.arg_pointers[0], strerror(errno));
		exit(EXIT_FAILURE);
	}
	close(fd);
}

static bool	try_paths(t_command *command, t_path_iter *path_iter)
{
	int			exit_code;
	extern char	**environ;
	while (path_iter_next(path_iter))
	{
		exit_code = execve(path_iter->path, command->args.arg_pointers, environ);
		if (exit_code < 0 && errno != ENOENT && errno != ENOTDIR)
			return (false);
	}
	return (exit_code == 0);
}

