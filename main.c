/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dmontesd <dmontesd@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/27 19:08:28 by dmontesd          #+#    #+#             */
/*   Updated: 2025/04/14 21:26:35 by dmontesd         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <wait.h>
#include <stdio.h>
#include "args.h"
#include "libft/libft.h"
#include "pipex.h"

static void	count_commands(char **argv);

typedef struct s_pipes
{
	int	pip_in[2];
	int	pip_out[2];
}	t_pipes;

bool	pipes_rotate(t_pipes *pipes, bool first_command, bool last_command)
{
	if (!first_command)
	{
		close(pipes->pip_in[0]);
		close(pipes->pip_in[1]);
		ft_memcpy(pipes->pip_in, pipes->pip_out, sizeof(int[2]));
		pipes->pip_out[0] = -1;
		pipes->pip_out[1] = -1;
	}
	if (!last_command)
	{
		if (pipe(pipes->pip_out) < 0)
			return (false);
	}
	return (true);
}

void	pipes_close(t_pipes *pipes)
{
	int	i;
	int j;
	int	**all;

	all = (int *[2]){pipes->pip_in, pipes->pip_out};
	i = 0;
	while (i < 2)
	{
		j = 0;
		while (j < 2)
		{
			if (all[i][j] != -1)
				close(all[i][j]);
			all[i][j] = -1;
			++j;
		}
		++i;
	}
}

t_execution_result	fork_commands(
		char **argv,
		int n_commands
) {
	t_execution_result	result;
	t_pipes				pipes;

	result.n_forks = 0;
	++argv;
	while (result.n_forks < n_commands)
	{
		pipes_rotate(
				&pipes,
				result.n_forks == 0,
				result.n_forks == (n_commands - 1)
				);
		result.last_pid = fork_process(&pipes, &argv);
		if (result.last_pid < 0)
			return (result);
		++result.n_forks;
	}
	pipes_close(&pipes);
	return (result);
}

int	wait_children(t_execution_result execution_result)
{
	int		i;
	pid_t	pid;
	int		wstatus;
	int		last_wstatus;

	i = 0;
	pid = 0;
	while (i < execution_result.n_forks)
	{
		pid = wait(&wstatus);
		if (pid < 0)
			perror("wait");
		if (pid == execution_result.last_pid)
			last_wstatus = wstatus;
		++i;
	}
	if (WIFEXITED(last_wstatus))
		return (WEXITSTATUS(last_wstatus));
	if (WIFSIGNALED(last_wstatus))
		return (WTERMSIG(last_wstatus));
	return (EXIT_SUCCESS);
}

int	main(int argc, char **argv)
{
	t_execution_result	execution_result;
	int					exit_code;

	exit_code = EXIT_SUCCESS;
	if (argc != 5)
	{
		ft_fprintf(STDERR_FILENO,
			"Wrong number of arguments: %d, expected: 4\n", argc - 1);
		return (EXIT_FAILURE);
	}
	execution_result = fork_commands(argv, 2);
	if (execution_result.n_forks > 0)
		exit_code = wait_children(execution_result);
	return (exit_code);
}
