/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   command_child.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dmontesd <dmontesd@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/31 18:05:13 by dmontesd          #+#    #+#             */
/*   Updated: 2025/04/04 19:09:50 by dmontesd         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include "pipex.h"
#include "libft.h"

void	child_error(char *str, int exit_status)
{
	perror(str);
	exit(exit_status);
}

static size_t	max_len(char **strs, size_t size)
{
	size_t	i;
	size_t	max;
	size_t	cur;

	i = 0;
	max = 0;
	while (i < size)
	{
		cur = ft_strlen(strs[i]);
		if (cur > max)
			max = cur;
		++i;
	}
	return (max);
}

typedef struct s_path_iter
{
	char		*path;
	size_t		path_size;
	size_t		current;
	t_env_path	env_path;
	char		*program_name;
}	t_path_iter;

bool	path_iter_make(t_path_iter *path_iter, char **environ, char *program_name)
{
	path_iter->program_name = program_name;
	if (!env_path_make(&path_iter->env_path, environ))
		return (false);
	path_iter->path_size = max_len(path_iter->env_path.paths,
			path_iter->env_path.paths_size) + ft_strlen(program_name) + 2;
	path_iter->path = malloc(path_iter->path_size);
	if (path_iter->path == NULL)
		return (free(path_iter->env_path.paths), free(path_iter->env_path.raw_path), false);
	path_iter->current = 0;
	return (true);
}

bool	path_iter_next(t_path_iter *iter)
{
	if (iter->current >= iter->env_path.paths_size)
		return (false);
	path_join(iter->env_path.paths[iter->current], iter->program_name, iter->path, iter->path_size);
	++iter->current;
	return (true);
}

static void try_paths(t_command *command)
{
	int			exit_code;
	extern char	**environ;
	t_path_iter	path_iter;

	if (!path_iter_make(&path_iter, environ, command->args[0]))
		child_error(command->args[0], EXIT_FAILURE);
	while (path_iter_next(&path_iter))
	{
		exit_code = execve(path_iter.path, command->args, environ);
		if (exit_code < 0 && errno != ENOENT && errno != ENOTDIR)
			child_error(command->args[0], EXIT_FAILURE);
	}
	if (exit_code < 0)
		child_error(command->args[0], EXIT_FAILURE);
}

void	child_execvpe(t_command *command)
{
	extern char	**environ;
	if (ft_strchr(command->args[0], '/') != NULL)
	{
		if (execve(command->args[0], command->args, environ) < 0)
			child_error(command->args[0], EXIT_FAILURE);
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
		child_error(command->args[0], EXIT_FAILURE);
	if (dup2(fd, command->redirect_fd) < 0)
		child_error(command->args[0], EXIT_FAILURE);
	close(fd);
}
