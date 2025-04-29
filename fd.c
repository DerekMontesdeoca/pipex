/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   fd.c                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dmontesd <dmontesd@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/15 12:16:23 by dmontesd          #+#    #+#             */
/*   Updated: 2025/04/29 23:46:09 by dmontesd         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include <stddef.h>
#include <stdbool.h>
#include <unistd.h>
#include "libft/libft.h"
#include <string.h>
#include <errno.h>

void	fd_close(int *pip)
{
	if (pip == NULL)
		return ;
	if (*pip >= 0)
	{
		close(*pip);
		*pip = -1;
	}
}

bool	fd_dup2_and_close_old(int *fd, int fd2, char *context)
{
	if (fd == NULL)
		return (true);
	if (*fd != -1)
	{
		if (dup2(*fd, fd2) < 0)
		{
			ft_fprintf(STDERR_FILENO, "%s: dup2: %s\n",
				context, strerror(errno));
			return (false);
		}
		fd_close(fd);
	}
	return (true);
}
