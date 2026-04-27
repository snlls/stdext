#pragma once


#define string_starts_with(input, query) (!strncmp((input), (query), strlen(query)))

char *string_goto_nextline(char *str);
