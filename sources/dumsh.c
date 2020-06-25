#include "../headers/dumsh.h"

void dumsh_loop(void) 
{
	char * line;
	char ** args;

	char pwd[PATH_MAX];
	char username[LOGIN_NAME_MAX];
	char * a;
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

	do 
	{
		write(1, prompt, strlen(prompt));
		line = dumsh_read_line();
		args = dumsh_split_line(line);
		/*write(1, line, strlen(line));
		write(1, "\n", strlen("\n"));
		write(1, args[1], strlen(args[1]));
		write(1, "\n", strlen("\n"));*/
	} while (1);
	free(prompt);
	free(args);
	free(line);
}

/*int dumsh_num_builtins(void)
{
	return sizeof(builtin_str) / sizeof(char *);
}*/

int dumsh_cd(char ** args)
{
	if (args[1] == NULL)
		perror("dumsh_cd error");
	else if (chdir(args[1]) != 0)
		perror("dumsh_cd error");

	return 1;
}

int dumsh_exit(char ** args)
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
		//printf("%s\n", token);
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


