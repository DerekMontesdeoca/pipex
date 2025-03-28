/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dmontesd <dmontesd@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/27 19:08:28 by dmontesd          #+#    #+#             */
/*   Updated: 2025/03/28 05:00:55 by dmontesd         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <wait.h>
#include <stdio.h>
#include "libft.h"

typedef struct s_env_path
{
	char	**paths;
	int		size;
}	t_env_path;

static int count_paths(const char *str)
{
	const char	*begin;
	const char	*end;
	int			count;

	begin = str;
	end = str;
	count = 0;
	while (*end != '\0')
	{
		if (*end == ':')
		{
			if (end != begin)
				++count;
			begin = end + 1;
		}
		++end;
	}
	if (begin != end)
		++count;
	return (count);
}

static int	find_path_variable(char **envp)
{
	size_t	i;

	i = 0;
	while (envp[i] != NULL)
	{
		if (ft_strncmp(envp[i], "PATH=", 5) == 0)
			return (i);
		++i;
	}
	return (-1);
}

void	env_path_split(t_env_path *env_path, char *raw_path)
{
	char	*begin;
	char	*end;
	size_t	i;
	
	begin = raw_path;
	end = raw_path;
	while (*end != '\0')
	{
		if (*end == ':')
		{
			*end = '\0';
			if (end != begin)
				env_path->paths[i++] = begin;
			begin = end + 1;
		}
		++end;
	}
	if (begin != end)
		env_path->paths[i++] = begin;
}

int	env_path_make(t_env_path *env_path, char **envp)
{
	int		path_index;
	char	*raw_path;

	env_path->paths = NULL;
	env_path->size = 0;
	path_index = find_path_variable(envp);
	if (path_index < 0)
		return (0);
	raw_path = envp[path_index] + 5;
	if (raw_path[0] == '\0')
		return (0);
	env_path->size = count_paths(raw_path);
	env_path->paths = malloc(env_path->size);
	if (env_path->paths == NULL)
		return (-1);
	env_path_split(env_path, raw_path);
	return (0);
}

// Todo: Im thinking of allocating a single string and using it to try all
// paths. Its a good idea. I need to calculate the length of the max path. It 
// can be pre-cached at read time. Maybe I need to expand my path struct.
// path_join()


int	ft_execvpe(
		t_env_path *env_path, char *path, char **const argv, char **const envp
) {
	int	i;
	char *final_path;
	
	i = 0;
	while (i < env_path->size)
	{
		final_path = path_join(env_path->paths[i], path);
		execve(final_path, argv, envp);
	}
}


int	pipe_child_setup(int *pip_out, int *pip_in)
{
	if (pip_in[1] != -1)
	{
		close(pip_in[1]);
		pip_in[1] = -1;
	}
	if (pip_out[0] != -1)
	{
		close(pip_out[0]);
		pip_out[0] = -1;
	}
	if (pip_in[0] != -1)
		if (dup2(pip_in[0], STDIN_FILENO) < 0)
			return (-1);
	if (pip_out[1] != -1)
		if (dup2(pip_out[1], STDOUT_FILENO) < 0)
			return (-1);
}

int	fork_command(
		t_env_path *env_path, char **argv, int *pip_in, int *pip_out
) {
	pid_t	pid;

	pid = fork();
	if (pid == 0)
	{
		if (pipe_child_setup(pip_out, pip_in) < 0)
			return (-1);
		// TODO: Don't replace envp path delims with nulls.
		ft_execvpe(env_path, command, command_args, envp);
	}
}

void pipes_init(int *pip_1, int *pip_2)
{
	pip_1[0] = -1;
	pip_1[1] = -1;
	pip_2[0] = -1;
	pip_2[1] = -1;
}

void pipes_swap(int *pip_out, int *pip_in)
{
	close(pip_in[0]);
	close(pip_in[1]);
	ft_memcpy(pip_out, pip_in, sizeof(int) * 2);
	pip_out[0] = -1;
	pip_out[1] = -1;
}

int	fork_commands(t_env_path *env_path, char **argv, int n_commands)
{
	int		i;
	int		pip_in[2];
	int		pip_out[2];
	int		n_forks;

	pipes_init(pip_in, pip_out);
	n_forks = 0;
	i = 0;
	while (i < n_commands)
	{
		if (i < (n_commands - 1))
		{
			if (pipe(pip_out) < 0)
				return (n_forks);
			fork_command(env_path, argv, pip_in, pip_out);
		}
		pipes_swap(pip_out, pip_in);
		++i;
	}
}

int wait_children(int size)
{
	int		i;
	pid_t	pid;

	i = 0;
	while (i < size)
	{
		pid = wait(NULL);
		if (pid < 0)
			fprintf(stderr, "wait pid %d: %s", pid, strerror(pid));
		++i;
	}
	if (pid < 0)
		return (pid);
	return (0);
}

int	main(int argc, char **argv, char **envp)
{
	t_env_path	env_path;
	int			n_commands;
	int			n_forks;

	if (argc != 4)
		return (1);
	env_path_make(&env_path, envp);
	n_forks = fork_commands(&env_path, argv, 2);
	return (wait_children(n_forks));
}
