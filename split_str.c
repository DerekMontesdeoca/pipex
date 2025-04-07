/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   split_str.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dmontesd <dmontesd@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/31 20:13:35 by dmontesd          #+#    #+#             */
/*   Updated: 2025/04/07 05:18:52 by dmontesd         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include <stdbool.h>
#include <stdlib.h>

static void	split_segments(char *str, char delim, char **push_iter)
{
	char	*end;
	bool	is_empty;
	
	if (str == NULL || push_iter == NULL)
		return ;
	is_empty = true;
	end = str;
	while (*end != '\0')
	{
		if (*end == delim && !is_empty)
		{
			is_empty = true;
			*(push_iter++) = str;
			*end = '\0';
		}
		else if (*end != delim && is_empty)
		{
			str = end;
			is_empty = false;
		}
		++end;
	}
	if (!is_empty)
		*push_iter = str;
}

static size_t	count_split_segments(char *str, char delim)
{
	bool	is_empty;
	size_t	count;

	if (str == NULL)
		return (0u);
	is_empty = true;
	count = 0;
	while (*str != '\0')
	{
		if (*str != delim && is_empty)
			is_empty = false;
		if (*str == delim && !is_empty)
		{
			is_empty = true;
			++count;
		}
		++str;
	}
	count += !is_empty;
	return (count);
}

char	**split_str_ptr_array(char *str, char delim, size_t *len_out)
{
	char	**result;

	if (str == NULL)
		return (NULL);
	*len_out = count_split_segments(str, delim);
	result = malloc(sizeof(char *) * (*len_out + 1));
	if (result == NULL)
		return (NULL);
	result[*len_out] = NULL;
	split_segments(str, delim, result);
	return (result);
}
