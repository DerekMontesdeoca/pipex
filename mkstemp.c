/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mkstemp.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dmontesd <dmontesd@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/22 03:22:32 by dmontesd          #+#    #+#             */
/*   Updated: 2025/04/22 23:17:10 by dmontesd         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <fcntl.h>
#include <unistd.h>
#include "pipex_bonus.h"
#include "libft/libft.h"

#define DEFAULT_TMPDIR "/tmp"
#define RANDOM_BYTES 10

int	ft_mkstemp(t_filename_buf path_buf)
{
	int		fd;
	int		read_bytes;
	int		total_read_bytes;

	ft_strlcpy(path_buf, DEFAULT_TMPDIR, sizeof(t_filename_buf));
	path_buf[4] = '/';
	fd = open("/dev/urandom", O_RDONLY);
	total_read_bytes = 0;
	while (total_read_bytes < RANDOM_BYTES)
	{
		read_bytes = read(fd, path_buf + 5 + total_read_bytes,
				RANDOM_BYTES - total_read_bytes);
		if (read_bytes <= 0)
			return (close(fd), -1);
		total_read_bytes += read_bytes;
	}
	if (close(fd) < 0)
		return (-1);
	ft_strlcpy(path_buf + 4 + 1 + RANDOM_BYTES, ".tmp",
		sizeof(t_filename_buf) - 4 - 1 - RANDOM_BYTES);
	return (open(path_buf, O_CREAT | O_EXCL | O_RDWR, 0600));
}

#undef RANDOM_BYTES
