/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   path_iter.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dmontesd <dmontesd@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/07 04:54:53 by dmontesd          #+#    #+#             */
/*   Updated: 2025/04/08 20:25:01 by dmontesd         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>
#include "pipex.h"
#include "libft/libft.h"

/**
 * Calculate max length from array of strings.
 */
static size_t	max_len(char **strs, size_t size)
{
	size_t	i;
	size_t	max;
	size_t	cur;

	i = 0;
	max = 0;
	while (i < size)
	{
		cur = ft_strlen(strs[i]);
		if (cur > max)
			max = cur;
		++i;
	}
	return (max);
}

bool	path_iter_make(
		t_path_iter *path_iter,
		char **environ,
		char *program_name
) {
	path_iter->program_name = program_name;
	if (!env_path_make(&path_iter->env_path, environ))
		return (false);
	path_iter->path_size = max_len(path_iter->env_path.paths,
			path_iter->env_path.paths_size) + ft_strlen(program_name) + 2;
	path_iter->path = malloc(path_iter->path_size);
	if (path_iter->path == NULL)
		return (free(path_iter->env_path.paths),
			free(path_iter->env_path.raw_path), false);
	path_iter->current = 0;
	return (true);
}

bool	path_iter_next(t_path_iter *iter)
{
	if (iter->current >= iter->env_path.paths_size)
		return (false);
	path_join(iter->env_path.paths[iter->current],
		iter->program_name, iter->path, iter->path_size);
	++iter->current;
	return (true);
}
