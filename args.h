/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   args.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dmontesd <dmontesd@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/10 01:48:53 by dmontesd          #+#    #+#             */
/*   Updated: 2025/04/14 12:34:31 by dmontesd         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ARGS_H
# define ARGS_H

# include <stdbool.h>
# include <stddef.h>

/**
 * TODO
 */
typedef struct s_args
{
	size_t	n_args;
	size_t	args_data_size;
	char	*args_data;
	char	**arg_pointers;
}	t_args;

void	args_init(t_args *args);

void	args_free_contents(t_args *args);

bool	parse_args(const char *raw_args, t_args *args_out);

#endif
