/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   command_child.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dmontesd <dmontesd@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/31 18:05:13 by dmontesd          #+#    #+#             */
/*   Updated: 2025/04/15 12:21:08 by dmontesd         ###   ########.fr       */
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

static bool	try_paths(t_command *command, t_path_iter *path_iter)
{
	int			exit_code;
	extern char	**environ;
	while (path_iter_next(path_iter))
	{
		exit_code = execve(path_iter->path,
				command->args.arg_pointers, environ);
		if (exit_code < 0 && errno != ENOENT && errno != ENOTDIR)
			break ;
	}
	if (exit_code < 0)
		perror(command->args.arg_pointers[0]);
	return (exit_code == 0);
}

bool	child_execvpe(t_command *command)
{
	extern char	**environ;
	t_path_iter	path_iter;
	bool		err;

	if (!path_iter_make(&path_iter, environ, command->args.arg_pointers[0]))
		return (false);
	err = false;
	if (path_iter.env_path.paths_size == 0)
	{
		ft_fprintf(STDERR_FILENO, "%s: No such file or directory\n",
				command->args.arg_pointers[0]);
		err = true;
	}
	else if (ft_strchr(command->args.arg_pointers[0], '/') != NULL)
	{
		err = execve(command->args.arg_pointers[0],
				command->args.arg_pointers, environ) < 0;
		if (err)
			perror(command->args.arg_pointers[0]);
	}
	else
		err = !try_paths(command, &path_iter);
	path_iter_free_contents(&path_iter);
	return (!err);
}

bool	child_setup_pipes(t_command *command)
{
	bool	err;

	fd_close(&command->pip_in[1]);
	fd_close(&command->pip_out[0]);
	err = !(fd_dup2_and_close_old(&command->pip_in[0], STDIN_FILENO,
			command->args.arg_pointers[0])
		&& fd_dup2_and_close_old(&command->pip_out[1], STDOUT_FILENO,
			command->args.arg_pointers[0]));
	return (!err);
}

bool	child_redirect_fds(t_command *command)
{
	int		fd;
	int		flag;
	bool	dup2_err;

	if (command->redirection != NULL && command->redirect_fd == STDIN_FILENO)
		flag = O_RDONLY;
	else if (command->redirection != NULL
		&& command->redirect_fd == STDOUT_FILENO)
		flag = O_WRONLY | O_CREAT | O_TRUNC;
	else
		return (true);
	dup2_err = false;
	fd = open(command->redirection, flag, 0664);
	if (fd < 0)
		ft_fprintf(STDERR_FILENO, "open: %s: %s\n",
				command->redirection, strerror(errno));
	else 
	{
		dup2_err = dup2(fd, command->redirect_fd) < 0;
		if (dup2_err)
			ft_fprintf(STDERR_FILENO, "%s: dup2: %s\n",
					command->args.arg_pointers[0], strerror(errno));
		close(fd);
	}
	return (!(dup2_err || fd < 0));
}
