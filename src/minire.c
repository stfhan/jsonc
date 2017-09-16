#include <string.h>

#include "minire.h"

int minire_inarray(const char character, const char *characters) {
	const char *c = characters;
	int ok = 1;
	int fail = 0;
	if (*c == '^')
	{
		c++;
		ok = 0;
		fail = 1;
	}
	while (*c)
	{
		if (*c == character) return ok;
		c++;
	}
	return fail;
}

int minire_match(const char *from, const char *characters) {
	int n = 0;
	const char *c = from;
	while (*c)
	{
		if (!minire_inarray(*c, characters)) 
		{
			break;
		}
		c++;
		n++;
	}
	return n;
}

int minire_matchprefix(const char *str, const char *pref) {
	char *start;
	int start_of_line = 0;

	if (pref[0] == '^')
	{
		pref++;
		start_of_line = 1;
	}

	start = strstr(str, pref); 

	if (!start)
	{
		return 0;
	}
	if (start_of_line && start != str)
	{
		return 0;
	}

	return (start - str) + strlen(pref);
}

int minire_matchsuffix(const char *str, const char *suff) 
{
	char *cont = strstr(str, suff); 

	if (!cont || cont != str)
	{
		return 0;
	}

	return 1;
}

int minire(const char *str, const char *pref, const char *characters, const char *suff, char **ret) 
{
	const char *cursor = str;

	if (pref) 
	{
		int moved = minire_matchprefix(str, pref);

		if (!moved) return 0;

		cursor += moved;
	}

	int len = minire_match(cursor, characters);

	if (!len) return 0;

	if (suff) 
	{
		int ok = minire_matchsuffix(cursor + len, suff);

		if (!ok) return 0;
	}

	*ret = (char*)cursor;

	return len;
}
