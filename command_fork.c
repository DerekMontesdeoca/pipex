/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   command_fork.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dmontesd <dmontesd@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/31 14:02:14 by dmontesd          #+#    #+#             */
/*   Updated: 2025/04/15 13:33:38 by dmontesd         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include "args.h"
#include "pipex.h"
#include "libft/libft.h"

static bool	command_make(t_command *command, char ***argv,
				bool first_command, bool last_command);
static void	parse_cli(t_command *command, char ***argv,
				bool is_first_command, bool is_last_command);
static void	command_recycle(t_command *command);

void	command_init(t_command *command)
{
	command->pip_out[0] = -1;
	command->pip_out[1] = -1;
	command->pip_in[0] = -1;
	command->pip_in[1] = -1;
	command->redirection = NULL;
	command->heredoc_delim = NULL;
}

/**
 * @param argv Needs to be a ptr to array of strings (char ***) to be able to 
 * consume argv as they are processed.
 */
int	command_fork(
		t_command *command,
		char ***argv,
		bool is_first_command,
		bool is_last_command
) {
	pid_t	pid;
	bool	err;

	if (!command_make(command, argv, is_first_command, is_last_command))
		return (-1);
	pid = fork();
	if (pid < 0)
		perror("fork");
	else if (pid == 0)
	{
		err = !(child_redirect_fds(command)
			&& child_setup_pipes(command)
			&& child_execvpe(command));
		if (err)
			exit(EXIT_FAILURE);
	}
	command_recycle(command);
	return (pid);
}

static void	command_recycle(t_command *command)
{
	args_free_contents(&command->args);
	command->redirection = NULL;
	command->heredoc_delim = NULL;
	command->redirect_fd = -1;
}

void	command_destroy_contents(t_command *command)
{
	int	**pips;
	int	i;
	int	j;
	
	pips = (int *[2]){command->pip_in, command->pip_out};
	i = 0;
	while (i < 2)
	{
		j = 0;
		while (j < 2)
		{
			fd_close(&pips[i][j]);
			++j;
		}
		++i;
	}
	args_free_contents(&command->args);
	command_init(command);
}

/**
 * First parses the command line from argv. Then, since the command struct is
 * reused for all children, this function reinitializes the necessary members 
 * to create the next process.
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
		ft_memcpy(command->pip_in, command->pip_out, sizeof(command->pip_in));
		command->pip_out[0] = -1;
		command->pip_out[1] = -1;
	}
	if (!last_command)
	{
		if (pipe(command->pip_out) < 0)
		{
			ft_fprintf(STDERR_FILENO, "%s: fork: %s\n",
				command->args.arg_pointers[0], strerror(errno));
			return (false);
		}
	}
	return (true);
}

/**
 * Extract all the information needed for 1 command from argv and store it in
 * the command struct. argv is incremented as it is processed.
 * Information extracted includes redirections, files, heredocs and argv for
 * the subprocess.
 */
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
