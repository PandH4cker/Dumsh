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
#include <errno.h>
#include "string_utils.h"

#define BUFF_SIZE 1024
#define STDERR_BUFF_SIZE 4096
#define DUMSH_DELIM " \t\r\n\a"
#define DUMSH_TOK_BUFF_SIZE 64

#define color(param) write(1, "\033[1;", strlen("\033[1;")); write(1, itoa(param), strlen(itoa(param))); write(1, "m", strlen("m"))
#define resetColor write(1, "\033[0m", strlen("\033[0m"))
#define RED 31
#define GREEN 32
#define YELLOW 33
#define BLUE 34
#define MAGENTA 35
#define CYAN 36

void dumsh_loop(void);
int dumsh_num_builtins(void);
char * dumsh_prompt(void);
int dumsh_cd(char ** args, int fd, char * stderr, int * stderr_size, int no_redirect, char * err_file);
int dumsh_exit(char ** args, int fd, char * stderr, int * stderr_size, int no_redirect, char * err_file);
int dumsh_help(char ** args, int fd, char * stderr, int * stderr_size, int no_redirect, char * err_file);
char * dumsh_read_line(int fd);
char ** dumsh_split_line(char * line);
int dumsh_execute(char ** args, char * stderr, int * stderr_size, int fd, int no_redirect, char * err_file);
int dumsh_launch(char ** args, int fd, char * stderr, int * stderr_size, int no_redirect, char * err_file);

#endif