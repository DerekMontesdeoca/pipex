/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dmontesd <dmontesd@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/27 19:06:51 by dmontesd          #+#    #+#             */
/*   Updated: 2025/04/22 23:15:03 by dmontesd         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#ifndef PIPEX_H
# define PIPEX_H

# include <stddef.h>
# include <stdbool.h>
# include <unistd.h>
# include <limits.h>
# include "args.h"

# define MAX_ACTIONS 8

typedef struct s_pos
{
	bool	is_first;
	bool	is_last;
}	t_pos;

/**
 * Struct to return from the fork loop execution.
 * 
 * @member n_forks How many forks were spawned.
 * @member last_pid To emulate shell behavior, we want to return the exit code
 * of only the last piped command. To do this we keep it here to compare the
 * pid's returned by wait(2). 
 */
typedef struct s_execution_result
{
	int		n_processes;
	pid_t	last_pid;
}	t_execution_result;

/**
 * Finds the PATH environment variable in extern environ.
 * 
 * @returns A pointer to the variable without "PATH=" or NULL.
 */
char	*get_env_path(void);

char	**get_env(void);

typedef struct s_pipeline
{
	int			pip_out[2];
	int			pip_in[2];
}	t_pipeline;

void	pipeline_init(t_pipeline *pipeline);

void	pipeline_destroy_contents(t_pipeline *pipeline);

bool	pipeline_next_command(t_pipeline *pipeline, t_pos pos);

typedef struct s_open_action
{
	char	*filename;
	int		fd;
	int		flags;
}	t_open_action;

typedef struct s_dup2_action
{
	int	from_fd;
	int	to_fd;
}	t_dup2_action;

typedef struct s_close_action
{
	int	fd;
}	t_close_action;

typedef enum e_cmd_action_type
{
	CMD_ACTION_OPEN,
	CMD_ACTION_DUP2,
	CMD_ACTION_CLOSE,
}	t_cmd_action_type;

typedef union u_cmd_action_union
{
	t_open_action	open;
	t_close_action	close;
	t_dup2_action	dup2;
}	t_cmd_action_union;

typedef struct s_cmd_action
{
	t_cmd_action_type	action_type;
	t_cmd_action_union	action;
}	t_cmd_action;

typedef enum e_pipe_setup_type
{
	PIPE_SETUP_READ,
	PIPE_SETUP_WRITE,
}	t_pipe_setup_type;

typedef struct s_command
{
	t_args			args;
	t_cmd_action	actions[MAX_ACTIONS];
	int				n_actions;
	int				heredoc_fd;
}	t_command;

bool	command_make(
			t_command *command,
			char ***argv,
			t_pipeline *pipeline,
			t_pos pos);

int		ft_spawnp(t_command *command);

bool	ft_spawnp_child(t_command *command);

void	command_destroy_contents(t_command *command);

void	command_add_close(t_command *command, int fd);

void	command_add_dup2(t_command *command, int from_fd, int to_fd);

void	command_add_open(t_command *command, char *filename, int fd, int flags);

void	fd_close(int *pip);

bool	fd_dup2_and_close_old(int *fd, int fd2, char *context);

typedef struct s_wait_status
{
	pid_t	pid;
	int		wstatus;
	int		last_wstatus;
	int		exit_code;
}	t_wait_status;

bool	ft_execvp(char *file, char **argv);

typedef struct s_path_tokenizer
{
	char	*file_ptr;
	size_t	file_size;
	char	*path_ptr;
	char	path_buf[PATH_MAX];
	bool	done;
}	t_path_tokenizer;

typedef enum e_path_tokenizer_status
{
	PATH_TOK_STATUS_ERR,
	PATH_TOK_STATUS_OK,
	PATH_TOK_STATUS_DONE,
}	t_path_tokenizer_status;

typedef char	t_filename_buf[19];

int		ft_mkstemp(t_filename_buf filename_buf);

int		setup_heredoc(char *delim);

#endif
