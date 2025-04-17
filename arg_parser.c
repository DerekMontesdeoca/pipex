/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   arg_parser.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dmontesd <dmontesd@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/10 01:22:36 by dmontesd          #+#    #+#             */
/*   Updated: 2025/04/17 20:21:50 by dmontesd         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdbool.h>
#include <stddef.h>
#include "arg_parser.h"

void	arg_parser_init(t_arg_parser *parser, const char *args)
{
	parser->args = args;
	arg_parser_reset(parser);
}

bool	arg_parser_next(t_arg_parser *parser)
{
	parser->cur = -1;
	while (parser->cur == -1)
	{
		if (arg_parser_char(parser) == '\0'
			&& parser->state == PARSE_STATE_EMPTY)
			return (false);
		else if (arg_parser_char(parser) == '\0')
		{
			parser->state = PARSE_STATE_EMPTY;
			parser->cur = '\0';
		}
		else if (parser->state == PARSE_STATE_EMPTY)
			arg_parser_empty_state(parser);
		else if (parser->state == PARSE_STATE_IN_WORD)
			arg_parser_in_word_state(parser);
		else if (parser->state == PARSE_STATE_IN_SINGLE_QUOTES)
			arg_parser_single_quotes_state(parser);
		else
			arg_parser_double_quotes_state(parser);
	}
	return (true);
}

void	arg_parser_reset(t_arg_parser *parser)
{
	parser->pos = 0;
	parser->state = PARSE_STATE_IN_WORD;
	parser->prev_is_backslash = false;
}
