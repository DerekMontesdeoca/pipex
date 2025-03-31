/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dmontesd <dmontesd@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/27 19:06:51 by dmontesd          #+#    #+#             */
/*   Updated: 2025/03/31 03:52:23 by dmontesd         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#ifndef PIPEX_H
# define PIPEX_H

# include <stddef.h>
# include <stdbool.h>

/**
 * t_env_path holds an allocated copy of the envp path variable without "PATH="
 * and with all ':' replaced with '\0'. Each beginning of each path is stored
 * in paths. size holds the size of the paths array. 
 * raw_path must be freed.
 * paths must be freed.
 */
typedef struct s_env_path
{
	char	*raw_path;
	char	**paths;
	int		size;
	int		paths_len;
}	t_env_path;

/**
 * Creates a new env_path.
 */
bool	env_path_make(t_env_path *env_path, const char * const *envp);

/**
 * Joins a path with a basename to form an absolute path to an executable.
 * This is only mean to work for joining files with directories for the purpose
 * of creating an execvpe. It has not been tested to work as a tool to join all
 * kinds of paths.
 */
void	path_join(
			const char *head,
			const char *tail,
			char *result, size_t size
			);

/**
 * Returns the index of the path variable inside an envp.
 * @returns The index of the variable or -1 if not found.
 */
int		find_path_variable(const char **envp);

#endif
