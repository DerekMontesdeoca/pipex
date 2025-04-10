/*                                                        :::      ::::::::   */
/*   parse_arg.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dmontesd <dmontesd@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/31 20:13:35 by dmontesd          #+#    #+#             */
/*   Updated: 2025/04/08 21:12:55 by dmontesd         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdbool.h>
#include <stdlib.h>
#include "args.h"
#include "arg_parser.h"

static void	split_args(t_arg_parser *parser, t_args *args);

bool	parse_args(const char *raw_args, t_args *args_out)
{
	t_arg_parser	parser;

	if (raw_args == NULL)
		return (false);
	arg_parser_init(&parser, raw_args);
	args_init(args_out);
	while (arg_parser_next(&parser))
	{
		++args_out->size;
		if (parser.cur == '\0')
			++args_out->n_args;
	}
	args_out->args = malloc(args_out->size);
	if (args_out->args == NULL)
		return (NULL);
	args_out->split_args = malloc((args_out->n_args + 1) * sizeof(char *));
	if (args_out->split_args == NULL)
		return (free(args_out->args), NULL);
	arg_parser_reset(&parser);
	split_args(&parser, args_out);
	return (true);
}

void args_init(t_args *args)
{
	args->args = NULL;
	args->split_args = NULL;
	args->size = 0;
	args->n_args = 0;
}

void	args_free_contents(t_args *args)
{
	if (args->args != NULL)
		free(args->args);
	if (args->split_args != NULL)
		free(args->split_args);
	args_init(args);
}

static void	split_args(t_arg_parser *parser, t_args *args)
{
	char	**arr_insert;
	char	*arg_insert;

	arr_insert = args->split_args;
	arg_insert = args->args;
	while (arg_parser_next(parser))
	{
		*(arr_insert++) = arg_insert;
		*(arg_insert++) = parser->cur;
		while (parser->cur != '\0')
		{
			arg_parser_next(parser);
			*(arg_insert++) = parser->cur;
		}
	}
	*(arr_insert++) = NULL;
}

