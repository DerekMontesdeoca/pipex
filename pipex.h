/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dmontesd <dmontesd@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/27 19:06:51 by dmontesd          #+#    #+#             */
/*   Updated: 2025/04/01 02:17:17 by dmontesd         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#ifndef PIPEX_H
# define PIPEX_H

# include <stddef.h>
# include <stdbool.h>

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
	char	**paths;
	size_t	paths_size;
	size_t	raw_path_len;
}	t_env_path;

/**
 * Creates a new env_path.
 */
bool	env_path_make(t_env_path *env_path, const char **envp);

/**
 * Joins head and tail by appending the tail to the head and storing the result
 * in result. IMPORTANT: allocate at least len(head) + len(tail) + 1 for '/'
 * + 1 for '\0'.
 * 
 * @param result Out parameter. If result size < len(head) + len(tail) + 1, then
 * path_join returns error.
 * @param size Out parameter. The size of the result.
 * @return 0 on success and -1 for error.
 */
void	path_join(
			const char *head,
			const char *tail,
			char *result, size_t size
			);

/**
 * Returns the index of the path variable inside an envp.
 * @returns The index of the variable or -1 if not found.
 */
int		find_path_variable(const char **envp);

/**
 * Represents all the data required to execute a piped command in the shell.
 * 
 * @member path A pointer to the command or absolute or relative path to an
 * executable file. This pointer points to an address of the program's argv.
 * 
 * @member args A pointer to the arguments of the command invoked. This points
 * to an address of the program's argv. The argv will have been modified to be
 * delimitted by nul chars and can be traversed using the argc.
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
	char		**envp;
	t_env_path 	*env_path;
}	t_command;

void	command_init(t_command *command, char **envp, t_env_path *env_path);

/**
 * Populates the command and forks it. Handles all the pipe creation and
 * redirection. Cleans up the parent process by closing unused fd's.
 */
int	command_fork(
		t_command *command,
		char ***argv,
		bool first_command,
		bool last_command
);

bool	command_make(
		t_command *command,
		char ***argv,
		bool first_command,
		bool last_command
);

void	command_cleanup(t_command *command);

void	child_execvpe(t_command *command);

void	child_error(char *str, int exit_status);

void	child_setup_pipes(t_command *command);

void	child_redirect_fds(t_command *command);

char	**pointer_split_inplace(char *str, char delim, size_t *size_out);

#endif
