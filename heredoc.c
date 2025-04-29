/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dmontesd <dmontesd@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/22 04:03:05 by dmontesd          #+#    #+#             */
/*   Updated: 2025/04/29 23:46:29 by dmontesd         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>
#include <fcntl.h>
#include "libft/libft.h"
#include "pipex_bonus.h"

#define MAX_HEREDOC_DELIM 4096

static bool	write_heredoc_to_tmp(int tmp_fd, char *delim);

int	setup_heredoc(char *delim)
{
	int				tmp_fd;
	t_filename_buf	filename;
	bool			err;

	err = false;
	tmp_fd = ft_mkstemp(filename);
	if (tmp_fd < 0)
		return (-1);
	err |= !write_heredoc_to_tmp(tmp_fd, delim);
	err |= close(tmp_fd) < 0;
	tmp_fd = -1;
	if (!err)
	{
		tmp_fd = open(filename, O_RDONLY);
		err |= tmp_fd < 0;
	}
	err |= unlink(filename);
	if (tmp_fd != -1 && err)
		close(tmp_fd);
	if (err)
		return (-1);
	else
		return (tmp_fd);
}

static bool	write_heredoc_to_tmp(int tmp_fd, char *delim)
{
	char			*line;
	size_t			delim_size;

	delim_size = ft_strnlen(delim, MAX_HEREDOC_DELIM);
	if (delim_size == MAX_HEREDOC_DELIM)
	{
		ft_fprintf(STDERR_FILENO, "pipex: error: here-document delimiter %s is "
			"too long, max length is %d\n", delim, MAX_HEREDOC_DELIM);
		return (false);
	}
	line = get_next_line(STDIN_FILENO);
	while (line != NULL)
	{
		if (ft_strncmp(line, delim, delim_size) == 0)
			return (free(line), reset_buf(), true);
		ft_fprintf(tmp_fd, "%s", line);
		free(line);
		line = get_next_line(STDIN_FILENO);
	}
	ft_fprintf(STDERR_FILENO, "warning: here-document delimited by end-of-file"
		" (wanted `%s')\n", delim);
	return (true);
}
