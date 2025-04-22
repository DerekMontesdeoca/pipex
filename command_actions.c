/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   command_actions.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dmontesd <dmontesd@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/22 23:10:58 by dmontesd          #+#    #+#             */
/*   Updated: 2025/04/22 23:17:40 by dmontesd         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <assert.h>
#include "pipex.h"

void	command_add_close(t_command *command, int fd)
{
	assert(command->n_actions != MAX_ACTIONS);
	command->actions[command->n_actions++] = (t_cmd_action){
		.action_type = CMD_ACTION_CLOSE,
		.action = (t_cmd_action_union){
		.close = (t_close_action){.fd = fd}
	}};
}

void	command_add_dup2(t_command *command, int from_fd, int to_fd)
{
	assert(command->n_actions != MAX_ACTIONS);
	command->actions[command->n_actions++] = (t_cmd_action){
		.action_type = CMD_ACTION_DUP2,
		.action = (t_cmd_action_union){
		.dup2 = (t_dup2_action){
		.from_fd = from_fd, .to_fd = to_fd
	}}};
}

void	command_add_open(t_command *command, char *filename, int fd, int flags)
{
	assert(filename != NULL);
	assert(command->n_actions != MAX_ACTIONS);
	command->actions[command->n_actions++] = (t_cmd_action)
	{
		.action_type = CMD_ACTION_OPEN, .action = (t_cmd_action_union)
	{
		.open = (t_open_action){
		.filename = filename, .flags = flags, .fd = fd
	}}};
}
