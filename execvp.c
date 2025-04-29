/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execvp.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dmontesd <dmontesd@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/21 21:26:58 by dmontesd          #+#    #+#             */
/*   Updated: 2025/04/29 13:33:09 by dmontesd         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <stdio.h>
#include <limits.h>
#include <errno.h>
#include <string.h>
#include <assert.h>
#include "libft/libft.h"
#include "pipex.h"

#define DEFAULT_PATH "/bin:/usr/bin"

static bool						exec_paths(char *file, char **argv);
static bool						path_tokenizer_make(
									t_path_tokenizer *paths,
									char *file_ptr
									);
static t_path_tokenizer_status	path_tokenizer_next(t_path_tokenizer *paths);

bool	ft_execvp(char *file, char **argv)
{
	if (ft_strchr(file, '/') != NULL)
	{
		execve(file, argv, get_env());
		perror(file);
	}
	else
		exec_paths(file, argv);
	return (false);
}

static bool	path_tokenizer_make(t_path_tokenizer *paths, char *file_ptr)
{
	assert(file_ptr != NULL);
	paths->file_ptr = file_ptr;
	paths->path_ptr = get_env_path();
	paths->file_size = ft_strnlen(file_ptr, NAME_MAX + 1) + 1;
	paths->done = false;
	if (paths->file_size - 1 > NAME_MAX)
	{
		errno = ENAMETOOLONG;
		return (false);
	}
	if (paths->path_ptr == NULL)
		paths->path_ptr = DEFAULT_PATH;
	return (true);
}

static t_path_tokenizer_status	path_tokenizer_next(t_path_tokenizer *paths)
{
	char	*path_prev;
	char	*path_buf_end;

	if (paths->done)
		return (PATH_TOK_STATUS_DONE);
	path_prev = paths->path_ptr;
	paths->path_ptr = ft_strchrnul(paths->path_ptr, ':');
	if (paths->path_ptr - path_prev > PATH_MAX - 1)
		return (errno = ENAMETOOLONG, PATH_TOK_STATUS_ERR);
	path_buf_end = ft_mempcpy(paths->path_buf, path_prev,
			paths->path_ptr - path_prev);
	*path_buf_end = '/';
	path_buf_end = ft_mempcpy(path_buf_end + (paths->path_ptr != path_prev),
			paths->file_ptr, paths->file_size);
	if (*paths->path_ptr != '\0')
		++paths->path_ptr;
	else
		paths->done = true;
	return (PATH_TOK_STATUS_OK);
}

static bool	exec_paths(char *file, char **argv)
{
	t_path_tokenizer		paths;
	t_path_tokenizer_status	status;
	bool					got_eaccess;

	if (!path_tokenizer_make(&paths, file))
		return (ft_fprintf(STDERR_FILENO, "%s: %s\n", argv,
				strerror(ENAMETOOLONG)), false);
	got_eaccess = false;
	status = path_tokenizer_next(&paths);
	while (status == PATH_TOK_STATUS_OK)
	{
		execve(paths.path_buf, argv, get_env());
		if (errno == EACCES)
			got_eaccess = true;
		if (errno != EACCES && errno != ENOENT && errno != ENOTDIR)
			break ;
		status = path_tokenizer_next(&paths);
	}
	if (status == PATH_TOK_STATUS_OK)
		assert(false);
	if (got_eaccess && status != PATH_TOK_STATUS_ERR)
		ft_fprintf(STDERR_FILENO, "%s: %s\n", file, strerror(EACCES));
	else
		ft_fprintf(STDERR_FILENO, "%s: %s\n", file, strerror(errno));
	return (false);
}
