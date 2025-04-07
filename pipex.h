/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dmontesd <dmontesd@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/27 19:06:51 by dmontesd          #+#    #+#             */
/*   Updated: 2025/04/07 05:16:02 by dmontesd         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#ifndef PIPEX_H
# define PIPEX_H

# include <stddef.h>
# include <stdbool.h>
# include <unistd.h>

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
 * @member args An array of pointers to main's argv. Like argv, the first param
 * has the invoked name of the executable. In order to create these pointers,
 * the delimiters in argv are changed into NUL bytes.
 * @member redirection File to redirect input or output to. redirect_fd
 * determines where this file is redirected to after opening.
 * @member pip_out Pipe created for the child's output.
 * @member pip_in Pipe created for the child's input.
 */
typedef struct s_command
{
	int			argc;
	char		**args;
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
 * Populates the command, creates necessary pipes, forks the process, and
 * delegates to them the opening of files, redirection, and closing of fd's.
 * After that, it cleans up any unused fd's in the parent.
 * 
 * @returns The fd of the newly forked process or -1 on error.
 */
int		command_fork(
			t_command *command,
			char ***argv,
			bool first_command,
			bool last_command
			);

/**
 * Cleans up the remaining pipes after all forks are done.
 */
void	command_cleanup(t_command *command);

/**
 * To be used on the child process. Execs the executable either by absolute or
 * relative path, or by joining the name with all the path components in the
 * PATH environment variable. This function never returns. On error it exits,
 * and of success it execs.
 */
void	child_execvpe(t_command *command);

/**
 * Print error and exit.
 * 
 * @param str The str passed to perror(3).
 * @param exit_status The parameter passed to exit(3).
 */
void	child_error(char *str, int exit_status);

/**
 * Handles pipe management on the child. After inheriting the pipes, the
 * function closes unused pipe ends, then it dup2s the pipes into the
 * appropriate fd's, and finally closes the old fd's.
 */
void	child_setup_pipes(t_command *command);

/**
 * Handle all child redirection needs beyond pipes.
 */
void	child_redirect_fds(t_command *command);

/**
 * Split a string by a delimitter but instead of doing a malloc for each string,
 * replace the delimitters in the original string with NUL bytes and malloc an
 * array of pointers to the original string. The return value must be freed.
 * Collapses consecutive delimitters. The resulting array is NULL terminated.
 *
 * @param str The string that will be modified by replacing the delimitters with
 * NUL bytes.
 * @param delim The delimitter character.
 * @param len_out An out parameter to store the len of the result. Because it
 * is NULL terminated, the size = len + 1;
 *
 * @returns A dynamically allocated array of pointers to the original string.
 * This string must be freed. This array is NULL terminated.
 */
char	**split_str_ptr_array(char *str, char delim, size_t *len_out);

#endif
