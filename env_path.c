/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_path.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dmontesd <dmontesd@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/28 10:48:14 by dmontesd          #+#    #+#             */
/*   Updated: 2025/03/31 05:03:51 by dmontesd         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include <stdlib.h>
#include <stdbool.h>
#include "pipex.h"

static void count_paths_and_len(const char *str, int *n_paths, size_t *len)
{
	size_t	i;
	bool	is_new_path;

	i = 0;
	*len = 0;
	*n_paths = 0;
	is_new_path = true;
	while (str != NULL && str[i] != '\0')
	{
		if (str[i] == ':')
		{
			if (is_new_path)
				++*len;
			is_new_path = true;
			++*n_paths;
		}
		else
			is_new_path = false;
		++i;
	}
	if (is_new_path)
		++*len;
	*len += i;
	++*n_paths;
}

/**
 * Puts a '\\0' in place of the delimiter at the "end" pointer. It then stores
 * the begin pointer at "path_push_iter".
 * A '.' is added if the path component is empty to signify the cwd.
 * See more on "copy_and_split_path".
 */
static void	split(
		bool *is_new_path,
		char ***path_push_iter,
		char **begin,
		char **end
) {
	if (*is_new_path)
		*((*end)++) = '.';
	*((*end)++) = '\0';
	*((*path_push_iter)++) = *begin;
	*begin = *end;
	*is_new_path = true;
}

static void	copy_char(const char *raw_path, char **end, bool *is_new_path)
{
	*((*end)++) = *raw_path;
	*is_new_path = false;
}

/**
 * The idea is to convert the raw_path into a null char delimitted path and
 * store the beginning of each component on a separate array to do 1 malloc.
 * Example:
 * 
 * /usr/bin:/usr/local/bin:/home/user/.local/bin\0
 * 
 * we convert that into 
 * 
 * /usr/bin\0/usr/local/bin\0/home/user/.local/bin\0
 * ^         ^               ^
 * 
 * With each '^' representing the pointers we store as our path strings.
 * We also make sure to convert ":::" into ".:.:.:.", with each '.' representing
 * the CWD.
 */
static void	copy_and_split_path(
		t_env_path *env_path,
		const char *raw_path
) {
	char	*begin;
	char	*end;
	char	**path_push_iter;
	bool	is_new_path;

	is_new_path = true;
	end = env_path->raw_path;
	begin = env_path->raw_path;
	path_push_iter = env_path->paths;
	while (raw_path != NULL && *raw_path != '\0')
	{
		if (*raw_path == ':')
			split(&is_new_path, &path_push_iter, &begin, &end);
		else
			copy_char(raw_path, &end, &is_new_path);
		++raw_path;
	}
	if (is_new_path)
		split(&is_new_path, &path_push_iter, &begin, &end);
	*(path_push_iter++) = begin;
}

bool	env_path_make(t_env_path *env_path, const char **envp)
{
	int			path_index;
	const char	*raw_path;
	size_t		path_len;

	env_path->paths = NULL;
	env_path->size = 0;
	env_path->paths_len = 0;
	raw_path = NULL;
	path_index = find_path_variable(envp);
	if (path_index >= 0)
		raw_path = envp[path_index] + 5;
	count_paths_and_len(raw_path, &env_path->size, &path_len);
	env_path->paths = malloc(sizeof(char *) * env_path->size);
	if (env_path->paths == NULL)
		return (false);
	env_path->raw_path = malloc(path_len + 1);
	if (env_path->raw_path == NULL)
		return (free(env_path->paths), -1);
	copy_and_split_path(env_path, raw_path);
	return (true);
}
