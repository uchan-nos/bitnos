#include <stdio.h>
#include <stddef.h>

//=============================================================================
// search the last occur of C in D
//=============================================================================
char* strrchr (char *d, int c)
{
	char *tmp = d;

	while ('\0' != *d)
		d++;

	while (tmp <= d) {
		if (c == *d)
			return d;
		d--;
	}

	return NULL;
}
