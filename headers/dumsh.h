#ifndef DUMSH_H
#define DUMSH_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <limits.h>
#include "string_utils.h"

#define BUFF_SIZE 1024
#define DUMSH_DELIM " \t\r\n\a"
#define DUMSH_TOK_BUFF_SIZE 64

/*char * builtin_str[] = {
	"cd",
	"exit",
	"help"
};

int (*builtin_func[]) (char **) = {
	&dumsh_cd,
	&dumsh_exit,
	&dumsh_help
};*/

void dumsh_loop(void);
int dumsh_num_builtins(void);
int dumsh_cd(char ** args);
int dumsh_exit(char ** args);
int dumsh_help(char ** args);
char * dumsh_read_line(void);
char ** dumsh_split_line(char * line);




#endif