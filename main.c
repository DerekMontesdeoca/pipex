/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dmontesd <dmontesd@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/27 19:08:28 by dmontesd          #+#    #+#             */
/*   Updated: 2025/04/15 12:00:25 by dmontesd         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <wait.h>
#include <stdio.h>
#include "libft/libft.h"
#include "pipex.h"

static t_execution_result	fork_commands(char **argv, int n_commands);
static int	wait_children(t_execution_result execution_result);

int	main(int argc, char **argv)
{
	t_execution_result	execution_result;
	int					exit_code;

	if (argc != 5)
	{
		ft_fprintf(STDERR_FILENO,
			"Wrong number of arguments: %d, expected: 4\n", argc - 1);
		return (EXIT_FAILURE);
	}
	exit_code = EXIT_SUCCESS;
	execution_result = fork_commands(argv, 2);
	if (execution_result.n_forks > 0)
		exit_code = wait_children(execution_result);
	if (execution_result.last_pid < 0)
		exit_code = EXIT_FAILURE;
	return (exit_code);
}

static t_execution_result	fork_commands(
		char **argv,
		int n_commands
) {
	t_execution_result	result;
	t_command			command;

	command_init(&command);
	result.n_forks = 0;
	++argv;
	while (result.n_forks < n_commands)
	{
		result.last_pid = command_fork(
				&command,
				&argv,
				result.n_forks == 0,
				result.n_forks == (n_commands - 1)
				);
		if (result.last_pid < 0)
			break ;
		++result.n_forks;
	}
	command_destroy_contents(&command);
	return (result);
}

static int	wait_children(t_execution_result execution_result)
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
		{
			perror("wait");
			if (i == execution_result.n_forks - 1)
				return (EXIT_FAILURE);
		}
		if (pid == execution_result.last_pid)
			last_wstatus = wstatus;
		++i;
	}
	if (WIFEXITED(last_wstatus))
		return (WEXITSTATUS(last_wstatus));
	if (WIFSIGNALED(last_wstatus))
		return (WTERMSIG(last_wstatus));
	return (0);
}
