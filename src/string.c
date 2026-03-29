#include <stdext/string.h>


int strpos(const char *src, const char *tok)
{
	int strpos = 0;
	int tokpos = 0;
	while(src[strpos]) {
		int found = 1;
		for(int n = 0; tok[n] && src[strpos + n]; n++) {
			if(tok[n] != src[strpos + n]) {
				found = 0;
				break;
			}
		}
		if(found)
			return strpos;
		strpos++;
	}
	return -1;
}
