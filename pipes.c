/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipes.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dmontesd <dmontesd@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/14 22:15:39 by dmontesd          #+#    #+#             */
/*   Updated: 2025/04/14 22:47:34 by dmontesd         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include "pipex.h"
#include "libft/libft.h"

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

bool	pipes_setup_io(t_pipes *pipes)
{
	if (pipes->pip_in[1] != -1)
	{
		close(pipes->pip_in[1]);
		pipes->pip_in[1] = -1;
	}
	if (pipes->pip_out[0] != -1)
	{
		close(pipes->pip_out[0]);
		pipes->pip_out[0] = -1;
	}
	if (pipes->pip_in[0] != -1)
	{
		if (dup2(pipes->pip_in[0], STDIN_FILENO) < 0)
			return (perror("dup2"), false);
		close(pipes->pip_in[0]);
	}
	if (pipes->pip_out[1] != -1)
	{
		if (dup2(pipes->pip_out[1], STDOUT_FILENO) < 0)
			return (perror("dup2"), false);
		close(pipes->pip_out[1]);
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

