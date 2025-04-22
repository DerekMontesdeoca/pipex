/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipeline.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dmontesd <dmontesd@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/21 23:26:49 by dmontesd          #+#    #+#             */
/*   Updated: 2025/04/22 22:06:35 by dmontesd         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"
#include "libft/libft.h"

void	pipeline_init(t_pipeline *pipeline)
{
	pipeline->pip_out[0] = -1;
	pipeline->pip_out[1] = -1;
	pipeline->pip_in[0] = -1;
	pipeline->pip_in[1] = -1;
}

void	pipeline_destroy_contents(t_pipeline *pipeline)
{
	int	**pips;
	int	i;
	int	j;

	pips = (int *[2]){pipeline->pip_in, pipeline->pip_out};
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
	pipeline_init(pipeline);
}

bool	pipeline_next_command(t_pipeline *pipeline, t_pos pos)
{
	if (!pos.is_first)
	{
		fd_close(&pipeline->pip_in[0]);
		fd_close(&pipeline->pip_in[1]);
		ft_memcpy(pipeline->pip_in, pipeline->pip_out,
			sizeof(pipeline->pip_in));
		pipeline->pip_out[0] = -1;
		pipeline->pip_out[1] = -1;
	}
	if (!pos.is_last)
	{
		if (pipe(pipeline->pip_out) < 0)
			return (false);
	}
	return (true);
}
