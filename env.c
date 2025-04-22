/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dmontesd <dmontesd@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/28 11:48:29 by dmontesd          #+#    #+#             */
/*   Updated: 2025/04/22 22:11:01 by dmontesd         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include "libft/libft.h"

char	*get_env_path(void)
{
	size_t		i;
	extern char	**environ;

	i = 0;
	while (environ[i] != NULL)
	{
		if (ft_strncmp(environ[i], "PATH=", 5) == 0)
			return (environ[i] + 5);
		++i;
	}
	return (NULL);
}

char	**get_env(void)
{
	extern char	**environ;

	return (environ);
}
