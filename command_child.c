/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   command_child.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dmontesd <dmontesd@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/31 18:05:13 by dmontesd          #+#    #+#             */
/*   Updated: 2025/04/01 05:10:19 by dmontesd         ###   ########.fr       */
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

static void try_paths(t_command *command)
{
	char	*path;
	size_t	size;
	size_t	i;
	int		exit_code;

	size = max_len(command->env_path->paths, command->env_path->paths_size)
		+ ft_strlen(command->args[0]) + 2;
	path = malloc(size);
	i = 0;
	while (i < (size_t) command->env_path->paths_size)
	{
		path_join(command->env_path->paths[i], command->args[0], path, size);
		exit_code = execve(path, command->args, command->envp);
		if (exit_code < 0 && errno != ENOENT && errno != ENOTDIR)
			child_error(command->args[0], EXIT_FAILURE);
		++i;
	}
	if (exit_code < 0)
		child_error(command->args[0], EXIT_FAILURE);
}

void	child_execvpe(t_command *command)
{
	if (ft_strchr(command->args[0], '/') != NULL)
	{
		if (execve(command->args[0], command->args, command->envp) < 0)
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
