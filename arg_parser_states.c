/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   arg_parser_states.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dmontesd <dmontesd@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/10 01:25:56 by dmontesd          #+#    #+#             */
/*   Updated: 2025/04/10 15:47:45 by dmontesd         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "arg_parser.h"

inline char	arg_parser_char(t_arg_parser *parser)
{
	return (parser->args[parser->pos]);
}

void	arg_parser_empty_state(t_arg_parser *parser)
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

void	arg_parser_in_word_state(t_arg_parser *parser)
{
	if (parser->prev_is_backslash)
	{
		parser->prev_is_backslash = false;
		parser->cur = parser->args[parser->pos];
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
		parser->cur = '\0';
	}
	else
		parser->cur = parser->args[parser->pos];
	++parser->pos;
}

void	arg_parser_double_quotes_state(t_arg_parser *parser)
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
	else
		parser->cur = parser->args[parser->pos];
	++parser->pos;
}

void	arg_parser_single_quotes_state(t_arg_parser *parser)
{
	if (arg_parser_char(parser) == '\'')
		parser->state = PARSE_STATE_IN_WORD;
	else
		parser->cur = parser->args[parser->pos];
	++parser->pos;
}
