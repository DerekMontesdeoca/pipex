/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dmontesd <dmontesd@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/27 19:06:51 by dmontesd          #+#    #+#             */
/*   Updated: 2025/04/17 18:10:37 by dmontesd         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#ifndef PIPEX_H
# define PIPEX_H

# include <stddef.h>
# include <stdbool.h>
# include <unistd.h>
# include "args.h"

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
	int		n_forks;
	pid_t	last_pid;
}	t_execution_result;

/**
 * t_env_path holds an allocated copy of the envp path variable without "PATH="
 * and with all ':' replaced with '\0'. Each beginning of each path is stored
 * in paths. size holds the size of the paths array. 
 * raw_path must be freed.
 * paths must be freed.
 */
typedef struct s_env_path
{
	char	*raw_path;
	size_t	raw_path_len;
	char	**paths;
	size_t	paths_size;
}	t_env_path;

/**
 * Creates a new env_path.
 */
bool	env_path_make(t_env_path *env_path, char **envp);

/**
 * Joins head and tail by appending the tail to the head and storing the result
 * in result. IMPORTANT: allocate at least len(head) + len(tail) + 1 for '/'
 * + 1 for '\0'.
 * 
 * @param result Out parameter. If result size < len(head) + len(tail) + 1, then
 * path_join returns error.
 * @param size Out parameter. The size of the result.
 * 
 * @return 0 on success and -1 for error.
 */
void	path_join(
			const char *head,
			const char *tail,
			char *result, size_t size
			);

/**
 * Finds the PATH environment variable in a char **envp.
 * 
 * @returns A pointer to the variable without "PATH=" or NULL.
 */
char	*find_path_variable(char **envp);

/**
 * An env path iterator that is consumed once traversed. Every call to next
 * joins the path with a program name specified when created. 
 * t_path_iter must be freed.
 *
 * @param path A buffer of the size of the max len path + / + program_name + \0.
 * @param path_size The size of @param path.
 * @param program_name a pointer to the program name on args.
 */
typedef struct s_path_iter
{
	char		*path;
	size_t		path_size;
	size_t		current;
	t_env_path	env_path;
	char		*program_name;
}	t_path_iter;

void	path_iter_free_contents(t_path_iter *iter_path);

/**
 * Creates a consumable path_iter.
 *
 * @param program_name A pointer to the name of the program that will be
 * concatenated with the path on every call to next().
 */
bool	path_iter_make(t_path_iter *iter, char **environ, char *program_name);

/**
 * Must be called before taking the value from @member path.
 *
 * @returns true when there is a valid value in @member path. When the iter is
 * consumed, it returns false.
 */
bool	path_iter_next(t_path_iter *iter);

void	path_iter_free(t_path_iter *iter);

/**
 * Represents all the data required to execute a piped command in the shell.
 * 
 * @member args Struct containing split args.
 * @member redirection File to redirect input or output to. redirect_fd
 * determines where this file is redirected to after opening.
 * @member pip_out Pipe created for the child's output.
 * @member pip_in Pipe created for the child's input.
 * @member heredoc_delim Stored so child can know when to stop reading from
 * stdin.
 */
typedef struct s_command
{
	t_args		args;
	char		*redirection;
	int			redirect_fd;
	char		*heredoc_delim;
	int			pip_out[2];
	int			pip_in[2];
}	t_command;

/**
 * A zero initialization of the struct. Allows the struct to be ready to be
 * used by each process on every iteration of the fork loop.
 */
void	command_init(t_command *command);

/**
 * Parses the command arguments, populates the command, creates necessary pipes,
 * forks the process, and delegates to the forked process the opening of files,
 * redirection, and closing of fd's. After that, it cleans up any unused fd's in
 * the parent.
 *
 * @param argv A ptr to array of strings (char ***), so args can be consumed
 * as they are processed.
 * 
 * @returns The fd of the newly forked process or -1 on error.
 */
int		command_fork(
			t_command *command,
			char ***argv,
			bool first_command,
			bool last_command
			);

bool	command_child_exec(t_command *command);

void	command_destroy_contents(t_command *command);

void	fd_close(int *pip);

bool	fd_dup2_and_close_old(int *fd, int fd2, char *context);

typedef struct s_wait_status
{
	pid_t	pid;
	int		wstatus;
	int		last_wstatus;
	int		exit_code;
}	t_wait_status;


#endif
