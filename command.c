/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   command.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dmontesd <dmontesd@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/21 23:35:48 by dmontesd          #+#    #+#             */
/*   Updated: 2025/04/22 23:21:59 by dmontesd         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <assert.h>
#include <fcntl.h>
#include <unistd.h>
#include "libft/libft.h"
#include "pipex.h"

static void	command_pipe_setup_action(t_command *command, int pip[2],
				t_pipe_setup_type setup_type);

void	command_init(t_command *command)
{
	args_init(&command->args);
	command->heredoc_fd = -1;
	command->n_actions = 0;
}

void	command_destroy_contents(t_command *command)
{
	args_free_contents(&command->args);
	command_init(command);
}

bool	command_make(
		t_command *command,
		char ***argv,
		t_pipeline *pipeline,
		t_pos pos)
{
	command_init(command);
	if (pos.is_first)
	{
		if (ft_strncmp(**argv, "here_doc", 9) == 0)
		{
			++*argv;
			command->heredoc_fd = setup_heredoc(*((*argv)++));
			if (command->heredoc_fd < 0)
				return (false);
			command_add_dup2(command, command->heredoc_fd, STDIN_FILENO);
			command_add_close(command, command->heredoc_fd);
		}
		else
			command_add_open(command, *((*argv)++), STDIN_FILENO, O_RDONLY);
	}
	parse_args(*((*argv)++), &command->args);
	if (pos.is_last)
		command_add_open(command, *((*argv)++), STDOUT_FILENO,
			O_WRONLY | O_CREAT | O_TRUNC);
	if (!pos.is_last)
		command_pipe_setup_action(command, pipeline->pip_out, PIPE_SETUP_WRITE);
	if (!pos.is_first)
		command_pipe_setup_action(command, pipeline->pip_in, PIPE_SETUP_READ);
	return (true);
}

#define PIPE_READ 0
#define PIPE_WRITE 1

static void	command_pipe_setup_action(
		t_command *command,
		int pip[2],
		t_pipe_setup_type setup_type)
{
	int	to_dup;
	int	to_close;
	int	fileno;

	if (setup_type == PIPE_SETUP_READ)
	{
		to_dup = PIPE_READ;
		to_close = PIPE_WRITE;
		fileno = STDIN_FILENO;
	}
	else
	{
		to_dup = PIPE_WRITE;
		to_close = PIPE_READ;
		fileno = STDOUT_FILENO;
	}
	command_add_dup2(command, pip[to_dup], fileno);
	command_add_close(command, pip[to_dup]);
	command_add_close(command, pip[to_close]);
}

#undef PIPE_READ
#undef PIPE_WRITE
