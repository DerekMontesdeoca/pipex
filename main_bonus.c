/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main_bonus.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dmontesd <dmontesd@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/27 19:08:28 by dmontesd          #+#    #+#             */
/*   Updated: 2025/04/17 23:07:46 by dmontesd         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <assert.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <wait.h>
#include <stdio.h>
#include "args.h"
#include "libft/libft.h"
#include "pipex.h"

static t_execution_result	fork_commands(char **argv, int n_commands);
static int					wait_children(t_execution_result execution_result);
static int					count_commands(int argc, char **argv);

int	main(int argc, char **argv)
{
	t_execution_result	execution_result;
	int					exit_code;
	int					n_commands;

	if (argc < 5)
	{
		ft_fprintf(STDERR_FILENO,
			"Wrong number of arguments: %d, expected: >= 4\n", argc - 1);
		return (EXIT_FAILURE);
	}
	n_commands = count_commands(argc, argv);
	if (n_commands < 0)
		return (EXIT_FAILURE);
	exit_code = EXIT_SUCCESS;
	execution_result = fork_commands(argv, n_commands);
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
	bool				is_first_command;
	bool				is_last_command;

	assert(n_commands > 0);
	command_init(&command);
	result.n_forks = 0;
	++argv;
	while (result.n_forks < n_commands)
	{
		is_first_command = result.n_forks == 0;
		is_last_command = result.n_forks == (n_commands - 1);
		result.last_pid = command_fork(
				&command,
				&argv,
				is_first_command,
				is_last_command);
		if (result.last_pid < 0)
			break ;
		++result.n_forks;
	}
	command_destroy_contents(&command);
	return (result);
}

static int	wait_children(t_execution_result execution_result)
{
	int				i;
	t_wait_status	status;

	i = 0;
	status.exit_code = 0;
	while (i < execution_result.n_forks)
	{
		status.pid = wait(&status.wstatus);
		if (status.pid < 0)
		{
			perror("wait");
			status.exit_code = EXIT_FAILURE;
		}
		if (status.pid > 0 && status.pid == execution_result.last_pid)
			status.last_wstatus = status.wstatus;
		++i;
	}
	if (status.exit_code == EXIT_FAILURE)
		return (status.exit_code);
	else if (WIFEXITED(status.last_wstatus))
		return (WEXITSTATUS(status.last_wstatus));
	else if (WIFSIGNALED(status.last_wstatus))
		return (WTERMSIG(status.last_wstatus));
	else
		return (EXIT_SUCCESS);
}

static int	count_commands(int argc, char **argv)
{
	t_args	args;
	int		n_args;

	if (!parse_args(argv[1], &args))
		return (-1);
	if (ft_strncmp(args.arg_pointers[0], "here_doc", 9) == 0)
		n_args = argc - 4;
	else
		n_args = argc - 3;
	args_free_contents(&args);
	return (n_args);
}
