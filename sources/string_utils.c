#include "../headers/string_utils.h"

char * strremove(char * str, const char * sub)
{
	char * p, * q, * r;
	if ((q = r = strstr(str, sub)) != NULL)
	{
		size_t len = strlen(sub);
		while ((r = strstr(p = r + len, sub)) != NULL)
			while (p < r)
				*q++ = *p++;

		while ((*q++ = *p++) != '\0') continue;
	}
	return str;
}

char *append(char before, char *str, char after)
{
    size_t len = strlen(str);
    if(before)
    {
        memmove(str + 1, str, ++len);
        *str = before;
    }
    if(after)
    {
        str[len] = after;
        str[len + 1] = 0;
    }
    return str;
}

int get_size(char ** args)
{
	int size = 0;
	while(args[size])
		size++;
	return size;
}

const char * itoa_buff(char * buff, size_t len, int num)
{
	static char loc_buff[sizeof(int) * __CHAR_BIT__];

	if(!buff)
	{
		buff = loc_buff;
		len = sizeof(loc_buff);
	}

	if(snprintf(buff, len, "%d", num) == -1)
		return "";

	return buff;
}

const char * itoa(int num)
{
	return itoa_buff(NULL, 0, num);
}
