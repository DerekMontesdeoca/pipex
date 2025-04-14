/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_arg.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dmontesd <dmontesd@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/10 15:42:54 by dmontesd          #+#    #+#             */
/*   Updated: 2025/04/14 12:15:01 by dmontesd         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdbool.h>
#include <stdlib.h>
#include "args.h"
#include "arg_parser.h"

static void	arg_pointers(t_arg_parser *parser, t_args *args);

bool	parse_args(const char *raw_args, t_args *args_out)
{
	t_arg_parser	parser;

	if (raw_args == NULL)
		return (false);
	arg_parser_init(&parser, raw_args);
	args_init(args_out);
	while (arg_parser_next(&parser))
	{
		++args_out->args_data_size;
		if (parser.cur == '\0')
			++args_out->n_args;
	}
	args_out->args_data = malloc(args_out->args_data_size);
	if (args_out->args_data == NULL)
		return (NULL);
	args_out->arg_pointers = malloc((args_out->n_args + 1) * sizeof(char *));
	if (args_out->arg_pointers == NULL)
		return (free(args_out->args_data), NULL);
	arg_parser_reset(&parser);
	arg_pointers(&parser, args_out);
	return (true);
}

void	args_init(t_args *args)
{
	args->args_data = NULL;
	args->arg_pointers = NULL;
	args->args_data_size = 0;
	args->n_args = 0;
}

void	args_free_contents(t_args *args)
{
	if (args->args_data != NULL)
		free(args->args_data);
	if (args->arg_pointers != NULL)
		free(args->arg_pointers);
	args_init(args);
}

static void	arg_pointers(t_arg_parser *parser, t_args *args)
{
	char	**arr_insert;
	char	*arg_insert;

	arr_insert = args->arg_pointers;
	arg_insert = args->args_data;
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
