#include "../headers/dumsh.h"

char * builtin_str[] = {
	"cd",
	"exit",
	"help"
};

int (*builtin_func[]) (char **, int, char *, int *) = {
	&dumsh_cd,
	&dumsh_exit,
	&dumsh_help
};

void dumsh_loop(void) 
{
	char * stderr = malloc(STDERR_BUFF_SIZE * sizeof(char));
	int stderr_size = STDERR_BUFF_SIZE;
	int fd = -1;

	char * line;
	char ** args;
	int status;

	char * prompt = dumsh_prompt();

	do 
	{
		int is_redirect = 0;
		write(1, prompt, strlen(prompt));
		line = dumsh_read_line();
		args = dumsh_split_line(line);

		int size = get_size(args);
		for (int i = 0; i < size; ++i)
			if(strcmp(args[i], ">1") == 0 && (i + 1) < size)
			{
				is_redirect = 1;
				if (fd < 0)
					fd = open(args[i + 1], O_WRONLY | O_CREAT | O_APPEND | O_TRUNC, S_IRUSR | S_IWUSR);
				if (fd < 0)
					perror("open() error");
			}

		status = dumsh_execute(args, size, stderr, &stderr_size, 
									(is_redirect || strcmp(args[0], "exit") == 0) 
									? fd : 1);
		prompt = dumsh_prompt();
	} while (status);
	close(fd);
	free(prompt);
	free(args);
	free(line);
	free(stderr);
}

int dumsh_help(char ** args, int fd, char * stderr, int * stderr_size)
{
	char * lines[] = 
	{
		"Yaniv Benichou's Dumsh\n",
		"Type program names and arguments, and hit enter.\n",
		"The following are built-in:\n",
		"Use the man command for information on other programs.\n"
	};

	write(1, lines[0], strlen(lines[0]));
	write(1, lines[1], strlen(lines[1]));
	write(1, lines[2], strlen(lines[2]));

	if(fd > 2)
	{
		write(fd, lines[0], strlen(lines[0]));
		write(fd, lines[1], strlen(lines[1]));
		write(fd, lines[2], strlen(lines[2]));
	}

	for (int i = 0; i < dumsh_num_builtins(); ++i)
	{
		write(1, " ", strlen(" "));
		write(1, builtin_str[i], strlen(builtin_str[i]));
		write(1, "\n", strlen("\n"));

		if(fd > 2)
		{
			write(fd, " ", strlen(" "));
			write(fd, builtin_str[i], strlen(builtin_str[i]));
			write(fd, "\n", strlen("\n"));
		}
	}

	write(1, lines[3], strlen(lines[3]));
	if(fd > 2) 	write(fd, lines[3], strlen(lines[3]));

	return 1;
}

int dumsh_launch(char ** args)
{
	pid_t pid;
	int status;

	pid = fork();
	if (pid == 0)
	{
		if (execvp(args[0], args) == -1)
			perror("dumsh_launch() error");
		exit(EXIT_FAILURE);
	}

	else if (pid < 0)
		perror("dumsh_launch() error");
	else
		do 
		{
			waitpid(pid, &status, WUNTRACED);
		} while (!WIFEXITED(status) && !WIFSIGNALED(status));
	return 1;
}

int dumsh_execute(char ** args, int size, char * stderr, int * stderr_size, int fd)
{
	if (args[0] == NULL)
		return 1;

	/*for (int i = 0; i < size; ++i)
		if(strcmp(args[i], ">1") == 0 && (i + 1) < size)
		{
			*fd = open(args[i + 1], O_WRONLY | O_CREAT | O_APPEND | O_TRUNC, S_IRUSR | S_IWUSR);
			if (*fd < 0)
			{
				perror("open() error");
				return 1;
			}
		}*/


	for (int i = 0; i < dumsh_num_builtins(); ++i)
		if (strcmp(args[0], builtin_str[i]) == 0)
			return (*builtin_func[i])(args, fd, stderr, stderr_size);
	return dumsh_launch(args);
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

int dumsh_cd(char ** args, int fd, char * stderr, int * stderr_size)
{
	if (args[1] == NULL || strcmp(args[1], ">1") == 0)
	{
		char * error = "ERR dumsh_cd: cd need at least one argument\n";
		write(2, error, strlen(error));
		if (fd > 2) 
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
	}
	else if (chdir(args[1]) != 0)
		perror("dumsh_cd error");

	return 1;
}

int dumsh_exit(char ** args, int fd, char * stderr, int * stderr_size)
{
	if (fd > 2)
	{
		write(fd, "\n", strlen("\n"));
		for (int i = 0; i < 80; ++i)
			write(fd, "#", strlen("#"));
		write(fd, "\n", strlen("\n"));
		write(fd, stderr, strlen(stderr));
	}
	return 0;
}

char * dumsh_read_line(void)
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
		if(read(0, &c, 1) == -1)
			perror("read() error");

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


