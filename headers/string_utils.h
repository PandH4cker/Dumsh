#ifndef STRING_UTILS
#define STRING_UTILS

#include <string.h>
#include <stdio.h>

char * strremove(char * str, const char * sub);
char * append(char before, char *str, char after);
int get_size(char ** args);
const char * itoa_buff(char * buff, size_t len, int num);
const char * itoa(int num);

#endif