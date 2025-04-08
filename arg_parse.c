/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   arg_parse.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dmontesd <dmontesd@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/31 20:13:35 by dmontesd          #+#    #+#             */
/*   Updated: 2025/04/07 20:15:43 by dmontesd         ###   ########.fr       */
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

/**
 * Counts the args in @param args while replacing delimitters with '\0'.
 */
static void	parse(
		t_arg_parser *parser,
		void (*on_token_end)(void *data),
		void (*on_char)(char c, void *data)
) {
	while (arg_parser_char(parser) != '\0')
	{
		if (parser->state == PARSE_STATE_EMPTY)
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
				on_char(arg_parser_char(parser), data);
			}
			else if (arg_parser_char(parser) == '\\')
				parser->prev_is_backslash = true;
			else if (arg_parser_char(parser) == '\'')
				parser->state = PARSE_STATE_IN_SINGLE_QUOTES;
			else if (arg_parser_char(parser) == '"')
				parser->state = PARSE_STATE_IN_DOUBLE_QUOTES;
			else if (arg_parser_char(parser) == ' ')
			{
				on_token_end(data);
				parser->state = PARSE_STATE_EMPTY;
			}
			++parser->pos;
		}
		else if (parser->state == PARSE_STATE_IN_SINGLE_QUOTES)
		{
			if (arg_parser_char(parser) == '\'')
				parser->state = PARSE_STATE_IN_WORD;
			else
				on_char(arg_parser_char(parser), data);
			++parser->pos;
		}
		else 
		{
			if (parser->prev_is_backslash)
			{
				parser->prev_is_backslash = false;
				on_char(arg_parser_char(parser), data);
			}
			else if (arg_parser_char(parser) == '\\')
				parser->prev_is_backslash = true;
			else if (arg_parser_char(parser) == '"')
				parser->state = PARSE_STATE_IN_WORD;
			++parser->pos;
		}
	}
}


/**
 * Parses command args by splitting spaces and quoted phrases with simple or
 * double quotes.
 *
 * @param args The command string, including the program name invocation. If 
 * this is NULL, the function returns NULL as well.
 * @param len_out Out parameter to store the length of the resulting array.
 *
 * @return A NULL delimitted array of splitted arguments that must be freed. On
 * error, NULL is returned.
 */
char	**parse_command_args(char *args, size_t *len_out)
{
	t_parse_state	state;

	if (args == NULL)
		return (NULL);
}















