/*                                                        :::      ::::::::   */
/*   arg_parse.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dmontesd <dmontesd@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/31 20:13:35 by dmontesd          #+#    #+#             */
/*   Updated: 2025/04/08 21:12:55 by dmontesd         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdbool.h>
#include <stdlib.h>

typedef struct s_args
{
	size_t	n_args;
	size_t	len;
	char	*args;
	char	**split_args;
}	t_args;

void args_init(t_args *args)
{
	args->args = NULL;
	args->split_args = NULL;
	args->len = 0;
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

typedef enum e_parse_state
{
	PARSE_STATE_EMPTY,
	PARSE_STATE_IN_SINGLE_QUOTES,
	PARSE_STATE_IN_DOUBLE_QUOTES,
	PARSE_STATE_IN_WORD,
} t_parse_state;

typedef struct s_arg_parser
{
	t_parse_state	state;
	bool			prev_is_backslash;
	char			*args;
	char			cur;
	size_t			pos;
}	t_arg_parser;

void	arg_parser_reset(t_arg_parser *parser)
{
	parser->pos = 0;
	parser->state = PARSE_STATE_EMPTY;
	parser->prev_is_backslash = false;
}

void	arg_parser_init(t_arg_parser *parser, char *args)
{
	parser->args = args;
	arg_parser_reset(parser);
}

char arg_parser_char(t_arg_parser *parser)
{
	return (parser->args[parser->pos]);
}

bool	arg_parser_next(t_arg_parser *parser) {
	parser->cur = -1;
	while (parser->cur == -1)
	{
		if (arg_parser_char(parser) == '\0' && parser->state == PARSE_STATE_EMPTY)
			return (false);
		else if (arg_parser_char(parser) == '\0')
		{
			parser->state = PARSE_STATE_EMPTY;
			parser->cur = '\0';
		}
		else if (parser->state == PARSE_STATE_EMPTY)
		{
			if (arg_parser_char(parser) == '"')
				parser->state = PARSE_STATE_IN_DOUBLE_QUOTES;
			else if (arg_parser_char(parser) == '\'')
				parser->state = PARSE_STATE_IN_SINGLE_QUOTES;
			else if (arg_parser_char(parser) != ' ')
				parser->state = PARSE_STATE_IN_WORD;
			if (parser->state != PARSE_STATE_IN_WORD)
				++parser->pos;
		}
		else if (parser->state == PARSE_STATE_IN_WORD)
		{
			if (parser->prev_is_backslash)
			{
				parser->prev_is_backslash = false;
				return (parser->cur = parser->args[parser->pos], true);
			}
			else if (arg_parser_char(parser) == '\\')
				parser->prev_is_backslash = true;
			else if (arg_parser_char(parser) == '\'')
				parser->state = PARSE_STATE_IN_SINGLE_QUOTES;
			else if (arg_parser_char(parser) == '"')
				parser->state = PARSE_STATE_IN_DOUBLE_QUOTES;
			else if (arg_parser_char(parser) == ' ')
			{
				parser->state = PARSE_STATE_EMPTY;
				return (parser->cur = '\0', true);
			}
			else
				parser->cur = parser->args[parser->pos];
			++parser->pos;
		}
		else if (parser->state == PARSE_STATE_IN_SINGLE_QUOTES)
		{
			if (arg_parser_char(parser) == '\'')
				parser->state = PARSE_STATE_IN_WORD;
			else
				parser->cur = parser->args[parser->pos];
			++parser->pos;
		}
		else 
		{
			if (parser->prev_is_backslash)
			{
				parser->prev_is_backslash = false;
				parser->cur = parser->args[parser->pos];
			}
			else if (arg_parser_char(parser) == '\\')
				parser->prev_is_backslash = true;
			else if (arg_parser_char(parser) == '"')
				parser->state = PARSE_STATE_IN_WORD;
			++parser->pos;
		}
	}
	return (true);
}

void	split_args(t_arg_parser *parser, t_args *args)
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

bool	parse_command_args(char *raw_args, t_args *args_out)
{
	t_arg_parser	parser;

	if (raw_args == NULL)
		return (false);
	arg_parser_init(&parser, raw_args);
	args_init(args_out);
	while (arg_parser_next(&parser))
	{
		++args_out->len;
		if (parser.cur == '\0')
			++args_out->n_args;
	}
	args_out->args = malloc(args_out->len + 1);
	if (args_out->args == NULL)
		return (NULL);
	args_out->split_args = malloc((args_out->n_args + 1) * sizeof(char *));
	if (args_out->split_args == NULL)
		return (free(args_out->args), NULL);
	arg_parser_reset(&parser);
	split_args(&parser, args_out);
	return (true);
}
