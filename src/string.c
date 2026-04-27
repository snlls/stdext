#include <stdext/string.h>

char *string_goto_nextline(char *str)
{
	while(*str && *str != '\n')		
		str++;
	
	if(*str == '\n')
		str++;
	
	return str;
}
