/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   path.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dmontesd <dmontesd@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/28 11:48:29 by dmontesd          #+#    #+#             */
/*   Updated: 2025/04/17 18:58:35 by dmontesd         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include "libft/libft.h"

/**
 * Copy head to result.
 * Remove trailing slashes.
 * Add 1 trailing slash and NUL.
 * Copy tail to result.
 */
void	path_join(const char *head, const char *tail, char *result, size_t size)
{
	size_t	len;

	len = ft_strlcpy(result, head, size);
	while (len > 1 && result[len - 1] == '/')
	{
		result[len - 1] = '\0';
		--len;
	}
	result[len++] = '/';
	result[len] = '\0';
	ft_strlcpy(result + len, tail, size - len);
}

char	*find_path_variable(char **envp)
{
	size_t	i;

	i = 0;
	while (envp[i] != NULL)
	{
		if (ft_strncmp(envp[i], "PATH=", 5) == 0)
			return (envp[i] + 5);
		++i;
	}
	return (NULL);
}
