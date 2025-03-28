/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dmontesd <dmontesd@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/27 19:06:51 by dmontesd          #+#    #+#             */
/*   Updated: 2025/03/28 18:15:30 by dmontesd         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#ifndef PIPEX_H
# define PIPEX_H

# include <stddef.h>

typedef struct s_env_path
{
	char	**paths;
	int		size;
}	t_env_path;

int	env_path_make(t_env_path *env_path, char **envp);

void path_join(const char *head, const char *tail, char *result, size_t size);

#endif
