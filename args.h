/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   args.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dmontesd <dmontesd@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/10 01:48:53 by dmontesd          #+#    #+#             */
/*   Updated: 2025/04/10 02:14:41 by dmontesd         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ARGS_H
# define ARGS_H

# include <stdbool.h>
# include <stddef.h>

typedef struct s_args
{
	size_t	n_args;
	size_t	size;
	char	*args;
	char	**split_args;
}	t_args;

void	args_free_contents(t_args *args);

bool	parse_args(const char *raw_args, t_args *args_out);

#endif
