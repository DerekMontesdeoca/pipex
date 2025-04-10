/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   arg_parser.h                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dmontesd <dmontesd@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/10 02:10:39 by dmontesd          #+#    #+#             */
/*   Updated: 2025/04/10 15:42:13 by dmontesd         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ARG_PARSER_H
# define ARG_PARSER_H

# include <stdbool.h>
# include <stddef.h>

typedef enum e_parse_state
{
	PARSE_STATE_EMPTY,
	PARSE_STATE_IN_SINGLE_QUOTES,
	PARSE_STATE_IN_DOUBLE_QUOTES,
	PARSE_STATE_IN_WORD,
}	t_parse_state;

typedef struct s_arg_parser
{
	t_parse_state	state;
	bool			prev_is_backslash;
	const char		*args;
	char			cur;
	size_t			pos;
}	t_arg_parser;

char	arg_parser_char(t_arg_parser *parser);

void	arg_parser_reset(t_arg_parser *parser);

void	arg_parser_init(t_arg_parser *parser, const char *args);

bool	arg_parser_next(t_arg_parser *parser);

void	arg_parser_empty_state(t_arg_parser *parser);

void	arg_parser_in_word_state(t_arg_parser *parser);

void	arg_parser_double_quotes_state(t_arg_parser *parser);

void	arg_parser_single_quotes_state(t_arg_parser *parser);

#endif
