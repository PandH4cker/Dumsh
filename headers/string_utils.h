#ifndef STRING_UTILS
#define STRING_UTILS

#include <string.h>

char * strremove(char * str, const char * sub);
char * append(char before, char *str, char after);
int get_size(char ** args);


#endif