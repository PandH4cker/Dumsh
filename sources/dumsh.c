#include "../headers/dumsh.h"

char * builtin_str[] = {
	"cd",
	"exit",
	"help"
};

int (*builtin_func[]) (char **, int) = {
	&dumsh_cd,
	&dumsh_exit,
	&dumsh_help
};

void dumsh_loop(void) 
{
	char * line;
	char ** args;
	int status;

	char * prompt = dumsh_prompt();

	do 
	{
		write(1, prompt, strlen(prompt));
		line = dumsh_read_line();
		args = dumsh_split_line(line);

		int size = get_size(args);

		status = dumsh_execute(args, size);
		prompt = dumsh_prompt();
	} while (status);

	free(prompt);
	free(args);
	free(line);
}

int dumsh_help(char ** args, int fd)
{
	int filedesc = fd < 0 ? 1 : fd;
	write(filedesc, "Yaniv Benichou's Dumsh\n", strlen("Yaniv Benichou's Dumsh\n"));
	write(filedesc, "Type program names and arguments, and hit enter.\n", strlen("Type program names and arguments, and hit enter.\n"));
	write(filedesc, "The following are built-in:\n", strlen("The following are built-in:\n"));

	for (int i = 0; i < dumsh_num_builtins(); ++i)
	{
		write(filedesc, " ", strlen(" "));
		write(filedesc, builtin_str[i], strlen(builtin_str[i]));
		write(filedesc, "\n", strlen("\n"));
	}

	write(filedesc, "Use the man command for information on other programs.\n", strlen("Use the man command for information on other programs.\n"));

	if (fd > 1)
		close(fd);

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

int dumsh_execute(char ** args, int size)
{
	int fd = -1;
	if (args[0] == NULL)
		return 1;

	for (int i = 0; i < size; ++i) {
		if(strcmp(args[i], ">1") == 0 && (i + 1) < size)
		{
			fd = open(args[i + 1], O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR | S_IXUSR);
			if (fd < 0)
			{
				perror("open() error");
				return 1;
			}
		}
	}


	for (int i = 0; i < dumsh_num_builtins(); ++i)
		if (strcmp(args[0], builtin_str[i]) == 0)
			return (*builtin_func[i])(args, fd);
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

int dumsh_cd(char ** args, int fd)
{
	if (args[1] == NULL)
		perror("dumsh_cd error");
	else if (chdir(args[1]) != 0)
		perror("dumsh_cd error");

	return 1;
}

int dumsh_exit(char ** args, int fd)
{
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


