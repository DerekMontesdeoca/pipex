/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_path.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dmontesd <dmontesd@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/28 10:48:14 by dmontesd          #+#    #+#             */
/*   Updated: 2025/03/28 11:48:00 by dmontesd         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include <stdlib.h>
#include "pipex.h"
#include "libft.h"

static int count_paths(const char *str)
{
	const char	*begin;
	const char	*end;
	int			count;

	begin = str;
	end = str;
	count = 0;
	while (*end != '\0')
	{
		if (*end == ':')
		{
			if (end != begin)
				++count;
			begin = end + 1;
		}
		++end;
	}
	if (begin != end)
		++count;
	return (count);
}

static int	find_path_variable(char **envp)
{
	size_t	i;

	i = 0;
	while (envp[i] != NULL)
	{
		if (ft_strncmp(envp[i], "PATH=", 5) == 0)
			return (i);
		++i;
	}
	return (-1);
}

static void	env_path_split(t_env_path *env_path, char *raw_path)
{
	char	*begin;
	char	*end;
	size_t	i;
	
	begin = raw_path;
	end = raw_path;
	i = 0;
	while (*end != '\0')
	{
		if (*end == ':')
		{
			*end = '\0';
			if (end != begin)
				env_path->paths[i++] = begin;
			begin = end + 1;
		}
		++end;
	}
	if (begin != end)
		env_path->paths[i++] = begin;
}

int	env_path_make(t_env_path *env_path, char **envp)
{
	int		path_index;
	char	*raw_path;

	env_path->paths = NULL;
	env_path->size = 0;
	path_index = find_path_variable(envp);
	if (path_index < 0)
		return (0);
	raw_path = envp[path_index] + 5;
	if (raw_path[0] == '\0')
		return (0);
	env_path->size = count_paths(raw_path);
	env_path->paths = malloc(env_path->size);
	if (env_path->paths == NULL)
		return (-1);
	env_path_split(env_path, raw_path);
	return (0);
}

// path_join()
