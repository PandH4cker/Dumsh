#include "../headers/dumsh.h"

char * builtin_str[] = {
	"cd",
	"exit",
	"help"
};

int (*builtin_func[]) (char **, int, char *, int *, int, char *) = {
	&dumsh_cd,
	&dumsh_exit,
	&dumsh_help
};

void dumsh_loop(void) 
{
	char * stderr = malloc(STDERR_BUFF_SIZE * sizeof(char));
	int stderr_size = STDERR_BUFF_SIZE;
	int fd = -1;
	char * filename;
	char * filename_err = NULL;

	char * line;
	char ** args;
	int status;

	char * prompt = dumsh_prompt();

	do 
	{
		int is_redirect = 0;
		color(BLUE); write(1, prompt, strlen(prompt)); resetColor;
		line = dumsh_read_line(0);
		args = dumsh_split_line(line);

		int size = get_size(args);
		for (int i = 0; i < size; ++i)
		{
			if(strcmp(args[i], ">1") == 0 && (i + 1) < size)
			{
				is_redirect = 1;
				if (fd < 0)
					fd = open(args[i + 1], O_WRONLY | O_CREAT | O_APPEND | O_TRUNC, S_IRUSR | S_IWUSR);
				if (fd < 0)
					perror("open() error");
				args[i + 1] = NULL;
				args[i] = NULL;
				break;
			}
			else if(strcmp(args[i], ">2") == 0 && (i + 1) < size)
			{
				is_redirect = 2;
				if (fd < 0)
					fd = open(args[i + 1], O_WRONLY | O_CREAT | O_APPEND | O_TRUNC, S_IRUSR | S_IWUSR);
				if (fd < 0)
					perror("open() error");
				filename = args[i + 1];
				filename_err = malloc(strlen(filename) + strlen(".err") + 1);
				strcpy(filename_err, filename);
				strcat(filename_err, ".err");
				args[i + 1] = NULL;
				args[i] = NULL;
				break;
			}
		}
			
		if(size > 0 && strcmp(args[0], "exit") == 0)
			is_redirect = 1;

		status = dumsh_execute(args, stderr, &stderr_size, 
							(size > 0 && is_redirect) ? fd : 1, is_redirect, filename_err);
		prompt = dumsh_prompt();
	} while (status);
	close(fd);
	free(prompt);
	free(args);
	free(line);
	if(filename_err != NULL)
		free(filename_err);
	free(stderr);
}

int dumsh_help(char ** args, int fd, char * stderr, int * stderr_size, int no_redirect, char * err_file)
{
	char * lines[] = 
	{
		"Raphael Dray's Dumsh\n",
		"Type program names and arguments, and hit enter.\n",
		"The following are built-in:\n",
		"Use the man command for information on other programs.\n"
	};

	color(GREEN);
	write(1, lines[0], strlen(lines[0]));
	write(1, lines[1], strlen(lines[1]));
	write(1, lines[2], strlen(lines[2]));
	resetColor;

	if(fd > 2)
	{
		write(fd, lines[0], strlen(lines[0]));
		write(fd, lines[1], strlen(lines[1]));
		write(fd, lines[2], strlen(lines[2]));
	}

	for (int i = 0; i < dumsh_num_builtins(); ++i)
	{
		color(GREEN);
		write(1, " ", strlen(" "));
		write(1, builtin_str[i], strlen(builtin_str[i]));
		write(1, "\n", strlen("\n"));
		resetColor;

		if(fd > 2)
		{
			write(fd, " ", strlen(" "));
			write(fd, builtin_str[i], strlen(builtin_str[i]));
			write(fd, "\n", strlen("\n"));
		}
	}

	color(GREEN); write(1, lines[3], strlen(lines[3])); resetColor;
	if(fd > 2) 	write(fd, lines[3], strlen(lines[3]));

	return 1;
}

int dumsh_launch(char ** args, int fd, char * stderr, int * stderr_size, int no_redirect, char * err_file)
{
	pid_t pid;
	int status;

	int fd_temp;
	if(fd > 2)
	{	
		if(err_file == NULL)
			fd_temp = open("err.tmp", O_WRONLY | O_CREAT | O_APPEND | O_TRUNC, S_IRUSR | S_IWUSR);
		else
			fd_temp = open(err_file, O_WRONLY | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR);
		if(fd_temp < 0)
			perror("open() error");
	}
	
	pid = fork();
	if (pid == 0)
	{
		if(fd > 2)
		{
			dup2(fd, STDOUT_FILENO);
			close(fd);
			dup2(fd_temp, STDERR_FILENO);
			close(fd_temp);
		}
		color(GREEN);
		if (execvp(args[0], args) == -1)
			perror("dumsh_launch() error");
		resetColor;
		exit(EXIT_FAILURE);
	}

	else if (pid < 0)
		perror("fork() error");
	else 
	{
		if (fd < 2)
			do 
			{
				waitpid(pid, &status, WUNTRACED);
			} while (!WIFEXITED(status) && !WIFSIGNALED(status));
		else
		{
			close(fd_temp);
			return 1;
		}
	}
	return 1;
}

int dumsh_execute(char ** args, char * stderr, int * stderr_size, int fd, int no_redirect, char * err_file)
{
	if (args[0] == NULL)
		return 1;

	for (int i = 0; i < dumsh_num_builtins(); ++i)
		if (strcmp(args[0], builtin_str[i]) == 0)
			return (*builtin_func[i])(args, fd, stderr, stderr_size, no_redirect, err_file);
	return dumsh_launch(args, fd, stderr, stderr_size, no_redirect, err_file);
}

char * dumsh_prompt(void)
{
	char pwd[PATH_MAX];
	char username[LOGIN_NAME_MAX];
	if(getcwd(pwd, sizeof(pwd)) == NULL) 
	{
		perror("getcwd() error");
		exit(EXIT_FAILURE);
	}

	if(getlogin_r(username, sizeof(username)) != 0)
	{
		perror("getlogin_r() error");
		exit(EXIT_FAILURE);
	}

	char * prompt = malloc(strlen(pwd) + strlen(">> ") + 1);
	strcpy(prompt, pwd);
	strcat(prompt, ">> ");
	if(strstr(prompt, "/home/")) 
	{
		char * home_directory = malloc(strlen("/home/") + strlen(username) + 1);
		strcpy(home_directory, "/home/");
		strcat(home_directory, username);
		strremove(prompt, home_directory);
		append('~', prompt, 0);
		free(home_directory);
	}

	return prompt;
}


int dumsh_num_builtins(void)
{
	return sizeof(builtin_str) / sizeof(char *);
}

int dumsh_cd(char ** args, int fd, char * stderr, int * stderr_size, int no_redirect, char * err_file)
{
	if (args[1] == NULL || strcmp(args[1], ">1") == 0)
	{
		char * error = "ERR dumsh_cd: cd need at least one argument\n";
		color(RED); write(2, error, strlen(error)); resetColor;
		if (fd > 2 && no_redirect == 1) 
		{
			if (strlen(stderr) + strlen(error) >= *stderr_size)
			{
				while (strlen(stderr) + strlen(error) >= *stderr_size)
					*stderr_size += STDERR_BUFF_SIZE;
				stderr = realloc(stderr, *stderr_size);
				if(!stderr)
				{
					perror("realloc() error");
					exit(EXIT_FAILURE);
				}

			}
			strcat(stderr, error);
		}
		else if(no_redirect == 2 && err_file != NULL)
		{
			int fd_temp = open(err_file, O_WRONLY | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR);
			if (fd_temp < 0)
				perror("open() error");
			write(fd_temp, error, strlen(error));
			close(fd_temp);
		}
	}
	else if (chdir(args[1]) != 0)
		perror("dumsh_cd error");

	return 1;
}

int dumsh_exit(char ** args, int fd, char * stderr, int * stderr_size, int no_redirect, char * err_file)
{
	if (fd > 2 && no_redirect == 1 && err_file == NULL)
	{
		write(fd, "\n", strlen("\n"));
		for (int i = 0; i < 80; ++i)
			write(fd, "#", strlen("#"));
		write(fd, "\n", strlen("\n"));
		write(fd, stderr, strlen(stderr));

		int fd_temp = open("err.tmp", O_RDONLY);
		if(fd_temp < 0)
			perror("open() error");

		char * line;
		while((line = dumsh_read_line(fd_temp)) != NULL)
		{
			write(fd, line, strlen(line));
			write(fd, "\n", strlen("\n"));
		}
		free(line);
		close(fd_temp);
		remove("err.tmp");
	}
	return 0;
}

char * dumsh_read_line(int fd)
{
	int buffer_size = BUFF_SIZE;
	int position = 0;
	char * buffer = malloc(buffer_size * sizeof(char));
	char c;

	if(!buffer)
	{
		perror("malloc() error");
		exit(EXIT_FAILURE);
	}

	while (1)
	{
		ssize_t bytes_read;
		if((bytes_read = read(fd, &c, 1)) == -1)
			perror("read() error");

		if(bytes_read == 0)
			break;

		if(c == '\n') 
		{
			buffer[position] = '\0';
			return buffer;
		}

		else 
			buffer[position] = c;


		position++;

		if (position >= buffer_size)
		{
			buffer_size += BUFF_SIZE;
			buffer = realloc(buffer, buffer_size);
			if (!buffer)
			{
				perror("realloc() error");
				exit(EXIT_FAILURE);
			}
		}
	}
	return NULL;
}

char ** dumsh_split_line(char * line)
{
	int buffer_size = DUMSH_TOK_BUFF_SIZE;
	int position = 0;
	char ** tokens = malloc(buffer_size * sizeof(char *));
	char * token, ** tokens_backup;

	if (!tokens)
	{
		perror("malloc() error");
		exit(EXIT_FAILURE);
	}

	token = strtok(line, DUMSH_DELIM);
	while (token != NULL)
	{
		tokens[position] = token;
		position++;

		if (position >= buffer_size)
		{
			buffer_size += DUMSH_TOK_BUFF_SIZE;
			tokens_backup = tokens;
			tokens = realloc(tokens, buffer_size * sizeof(char *));
			if (!tokens)
			{
				free(tokens_backup);
				perror("realloc() error");
				exit(EXIT_FAILURE);
			}
		}

		token = strtok(NULL, DUMSH_DELIM);
	}

	tokens[position] = NULL;
	return tokens;
}


