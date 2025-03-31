/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   command.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dmontesd <dmontesd@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/31 05:07:12 by dmontesd          #+#    #+#             */
/*   Updated: 2025/03/31 05:22:37 by dmontesd         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include <unistd.h>
#include "pipex.h"
#include "libft.h"

void	command_init(t_command *command)
{
	command->pip_out[0] = -1;
	command->pip_out[1] = -1;
	command->pip_in[0] = -1;
	command->pip_in[1] = -1;
	command->argv = NULL;
	command->path = NULL;
}

bool	command_make(
		t_command *command,
		char ***argv,
		bool first_command,
		bool last_command
) {
	if (first_command)
	{

	}
	if (last_command)
	{

	}
	else
	{

	}
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
			return (false);
	}
	return (true);
}

void	command_fork(t_command *command)
{
	pid_t	pid;

	pid = fork();
	if (pid != 0)
		return ;
	if (pipe_child_setup(pip_out, pip_in) < 0)
		return (-1);
	ft_execvpe(command->path, command->argv, command->envp, command->env_path);
}
