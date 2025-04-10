/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   command_fork.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dmontesd <dmontesd@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/31 14:02:14 by dmontesd          #+#    #+#             */
/*   Updated: 2025/04/10 02:35:27 by dmontesd         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include "args.h"
#include "pipex.h"
#include "libft/libft.h"

static bool	command_make( t_command *command, char ***argv,
		bool first_command, bool last_command);
static void	parse_cli( t_command *command, char ***argv,
		bool is_first_command, bool is_last_command);

void	command_init(t_command *command)
{
	command->pip_out[0] = -1;
	command->pip_out[1] = -1;
	command->pip_in[0] = -1;
	command->pip_in[1] = -1;
	args_init(&command->args);
	command->redirection = NULL;
	command->heredoc_delim = NULL;
}

int	command_fork(
		t_command *command,
		char ***argv,
		bool is_first_command,
		bool is_last_command
) {
	pid_t	pid;

	if (!command_make(command, argv, is_first_command, is_last_command))
		return (-1);
	pid = fork();
	if (pid < 0)
		return (perror(command->args.split_args[0]),
			args_free_contents(&command->args), -1);
	if (pid == 0)
	{
		child_redirect_fds(command);
		child_setup_pipes(command);
		child_execvpe(command);
	}
	args_free_contents(&command->args);
	return (pid);
}

void	command_cleanup(t_command *command)
{
	if (command->pip_in[0] != -1)
		close(command->pip_in[0]);
	if (command->pip_in[1] != -1)
		close(command->pip_in[1]);
	if (command->pip_out[0] != -1)
		close(command->pip_out[0]);
	if (command->pip_out[1] != -1)
		close(command->pip_out[1]);
}

/**
 * Since the command struct is reused for all children, this function
 * reinitializes the necessary members to create the next process.
 * This function moves the output pipe of the previous command into the
 * input slot of the next command and creates a new pipe for
 * output if necessary. That way, all commands are connected.
 */
static bool	command_make(
		t_command *command,
		char ***argv,
		bool first_command,
		bool last_command
) {
	parse_cli(command, argv, first_command, last_command);
	if (!first_command)
	{
		close(command->pip_in[0]);
		close(command->pip_in[1]);
		ft_memcpy(command->pip_in, command->pip_out, sizeof(int) * 2);
		command->pip_out[0] = -1;
		command->pip_out[1] = -1;
	}
	if (!last_command)
	{
		if (pipe(command->pip_out) < 0)
			return (args_free_contents(&command->args), false);
	}
	return (true);
}

static void	parse_cli(
		t_command *command,
		char ***argv,
		bool is_first_command,
		bool is_last_command
) {
	if (is_first_command)
	{
		if (ft_strncmp(**argv, "here_doc", 9) == 0)
		{
			++*argv;
			command->heredoc_delim = *((*argv)++);
		}
		else
		{
			command->redirection = *((*argv)++);
			command->redirect_fd = STDIN_FILENO;
		}
	}
	parse_args(*((*argv)++), &command->args);
	if (is_last_command)
	{
		command->redirection = *((*argv)++);
		command->redirect_fd = STDOUT_FILENO;
	}
}
