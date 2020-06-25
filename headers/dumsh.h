#ifndef DUMSH_H
#define DUMSH_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <limits.h>
#include <fcntl.h>
#include "string_utils.h"

#define BUFF_SIZE 1024
#define DUMSH_DELIM " \t\r\n\a"
#define DUMSH_TOK_BUFF_SIZE 64

void dumsh_loop(void);
int dumsh_num_builtins(void);
char * dumsh_prompt(void);
int dumsh_cd(char ** args, int fd);
int dumsh_exit(char ** args, int fd);
int dumsh_help(char ** args, int fd);
char * dumsh_read_line(void);
char ** dumsh_split_line(char * line);
int dumsh_execute(char ** args, int size);
int dumsh_launch(char ** args);

#endif