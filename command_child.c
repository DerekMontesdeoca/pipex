/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   command_child.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dmontesd <dmontesd@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/31 18:05:13 by dmontesd          #+#    #+#             */
/*   Updated: 2025/04/17 18:52:45 by dmontesd         ###   ########.fr       */
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
static bool	child_execvpe(t_command *command);
static bool	child_redirect_fds(t_command *command);
static bool	child_setup_pipes(t_command *command);

bool	command_child_exec(t_command *command)
{
	return (child_redirect_fds(command)
		&& child_setup_pipes(command)
		&& child_execvpe(command));
}

static bool	child_redirect_fds(t_command *command)
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

/**
 * Handles pipe management on the child. After inheriting the pipes, the
 * function closes unused pipe ends, then it dup2s the pipes into the
 * appropriate fd's, and finally closes the old fd's.
 */
static bool	child_setup_pipes(t_command *command)
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

/**
 * To be used on the child process. Execs the executable either by absolute or
 * relative path, or by joining the name with all the path components in the
 * PATH environment variable. This function never returns. On error it exits,
 * and of success it execs.
 */
static bool	child_execvpe(t_command *command)
{
	extern char	**environ;
	t_path_iter	path_iter;
	bool		err;

	if (!path_iter_make(&path_iter, environ, command->args.arg_pointers[0]))
		return (false);
	err = false;
	if (ft_strchr(command->args.arg_pointers[0], '/') != NULL)
	{
		err = execve(command->args.arg_pointers[0],
			command->args.arg_pointers, environ) < 0;
		perror(command->args.arg_pointers[0]);
	}
	else if (path_iter.env_path.paths_size == 0)
	{
		ft_fprintf(STDERR_FILENO, "%s: No such file or directory\n",
				command->args.arg_pointers[0]);
		err = true;
	}
	else
		err = !try_paths(command, &path_iter);
	path_iter_free_contents(&path_iter);
	return (!err);
}

static bool	try_paths(t_command *command, t_path_iter *path_iter)
{
	extern char	**environ;
	bool		got_eaccess;
	int			errnum;

	got_eaccess = false;
	while (path_iter_next(path_iter))
	{
		execve(path_iter->path, command->args.arg_pointers, environ);
		errnum = errno;
		if (errnum == EACCES)
			got_eaccess = true;
		if (errnum != ENOENT && errnum != ENOTDIR && errnum != EACCES)
			break ;
	}
	if (got_eaccess)
		ft_fprintf(STDERR_FILENO, "%s: %s\n", command->args.arg_pointers[0],
				strerror(EACCES));
	else
		ft_fprintf(STDERR_FILENO, "%s: %s\n", command->args.arg_pointers[0],
				strerror(errnum));
	return (false);
}
