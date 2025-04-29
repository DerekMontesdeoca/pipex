/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   spawnp_child.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dmontesd <dmontesd@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/31 18:05:13 by dmontesd          #+#    #+#             */
/*   Updated: 2025/04/29 23:46:53 by dmontesd         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include <alloca.h>
#include <assert.h>
#include <fcntl.h>
#include <linux/limits.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include "libft/libft.h"
#include "pipex.h"

static bool	open_action(t_open_action *action);
static bool	dup2_action(t_dup2_action *action);
static bool	close_action(t_close_action *action);
static bool	run_spawn_actions(t_command *command);

bool	ft_spawnp_child(t_command *command)
{
	return (run_spawn_actions(command)
		&& ft_execvp(command->args.arg_pointers[0],
			command->args.arg_pointers));
}

static bool	run_spawn_actions(t_command *command)
{
	int				i;
	t_cmd_action	*action;
	bool			err;

	err = false;
	i = 0;
	while (i < command->n_actions)
	{
		action = &command->actions[i];
		if (action->action_type == CMD_ACTION_OPEN)
			err |= !open_action(&action->action.open);
		else if (action->action_type == CMD_ACTION_CLOSE)
			err |= !close_action(&action->action.close);
		else if (action->action_type == CMD_ACTION_DUP2)
			err |= !dup2_action(&action->action.dup2);
		if (err)
			break ;
		++i;
	}
	return (!err);
}

static bool	dup2_action(t_dup2_action *action)
{
	if (dup2(action->from_fd, action->to_fd) < 0)
		return (perror("dup2"), false);
	return (true);
}

static bool	close_action(t_close_action *action)
{
	if (close(action->fd) < 0)
		return (perror("close"), false);
	return (true);
}

static bool	open_action(t_open_action *action)
{
	int		fd;
	bool	err;

	err = false;
	fd = open(action->filename, action->flags, 0666);
	if (fd < 0)
	{
		ft_fprintf(STDERR_FILENO, "open: %s: %s\n", action->filename,
			strerror(errno));
		err = true;
	}
	else
	{
		if (dup2(fd, action->fd) < 0)
		{
			perror("dup2");
			err = true;
		}
		if (close(fd) != 0)
		{
			err = true;
			perror("close");
		}
	}
	return (!err);
}
